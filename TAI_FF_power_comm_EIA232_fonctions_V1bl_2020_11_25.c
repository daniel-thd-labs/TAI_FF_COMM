#include "TAI_FF_Versionner.h"
#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
#include "TAI_FF_power_comm_EIA232_functions_V1.h"
#include "TAI_FF_power_comm_config_manager_fonctions.h"
#include "TAI_FF_power_comm_REAPER_Process_Functions.h"
#include "TAI_FF_power_comm_INJEKTOR_Process_Functions.h"
#include "TAI_FF_globales_externes.h"
#include "Debug.h"


//Local globals
#define CANCEL_FLAG 0x0001
#define DOT_FLAG 0x0002
#define ENTER_FLAG 0x0004
#define SRC_RASP 5
#define RECALLIT 4
unsigned char tempRxBuffer[100];
unsigned char tempIdx;

unsigned char Rasp_Tx_data_buf[RASP_BUFSIZE];
unsigned short Rasp_Tx_data_buf_top;
unsigned short Rasp_Tx_data_buf_bot;
unsigned short Rasp_Tx_data_buf_quantity;
unsigned short Rasp_Tx_data_buf_maxquantity;
unsigned short Rasp_Tx_data_buf_size;

unsigned char Rasp_Rx_data_buf[RASP_BUFSIZE];
unsigned short Rasp_Rx_data_buf_top;
unsigned short Rasp_Rx_data_buf_bot;
unsigned short Rasp_Rx_data_buf_quantity;
unsigned short Rasp_Rx_data_buf_maxquantity;
unsigned short Rasp_Rx_data_buf_size;

unsigned char RaspSingleDataBuf[16];
unsigned char RaspSingleIndexBuf[16];
unsigned char RaspSingleDataBuf_top;
unsigned char RaspSingleDataBuf_bot;
unsigned char RaspSingleDataBuf_quantity;
unsigned char RaspSingleDataBuf_maxquantity;
unsigned char RaspSingleDataBuf_size;

unsigned char Temp_data[48], MsgData[48],MsgIndex;
unsigned char skipit=0;
/*****************************************************
*le message transmit par les slave et accessoires
* est formé de:
* l'adresse de destination: x0101 (= master)
* l'adresse de source ex 0x0106 (LCD + Recall_it)
* le data number
* le code de crtl du message
* le data
ex: message type
0x0101
0x0106
0x06
0x50
0x12
0xff
****************************************************/

/****************************************************************

Adressages du système Tangerine automation

0x0100 globale talk to all
0x0101 MASTER
0x0102 MASTER
0x0103 MASTER
0x0104 RECALL_IT
0x0105 LCD
0x0106 LCD(+RECALL_IT)

0x0110 a 0x0117 motor faders

RECALL_IT
0x0104 0x0000 stop, Enable= on
0x0104 0x0001 start, Enable = off
0x0104 0x0002 Set Control Adresse

Codes des fonctions
LCD pages
0x0105 0x050 page 0
0x0105 0x051 page 1
0x0105 0x052 page 2
0x0105 0x053 page 3
0x0105 0x054 page 4

LCD pages+recalit
0x0106 0x050 page 0
0x0106 0x051 page 1
0x0106 0x052 page 2
0x0106 0x053 page 3
0x0106 0x054 page 4

0x0106 0x0000 stop, Enable= on
0x0106 0x0001 start, Enable = off
0x0106 0x0002 Set Control Adresse

*/
/****************************************************************
*****************************************************************
|FONCTIONS:	void Init_RASP_Uart1(void)							|
|Notes:C'est le UART du Raspberry pi baud rate 115200			|
|																|
|Arguments:														|
|Variables locals:												|
|																|
|Variables globals:												|
|																|
*****************************************************************
****************************************************************/
void Init_RASP_Uart1(void)
{	
//version sortie sur pin de PORT RASPBERRY PI
unsigned short n;
for(n=0;n<48;n++)
	{
	Temp_data[n]=0;
	MsgData[n]=0;
	}
MsgIndex=0;
for(n=0;n<RASP_BUFSIZE;n++)
	{
	Rasp_Tx_data_buf[n] = 0;
	Rasp_Rx_data_buf[n] = 0;
	}
Rasp_Tx_data_buf_top=0;
Ack_flag_response=0;
Rasp_Tx_data_buf_bot=0;
Rasp_Tx_data_buf_quantity=0;
Rasp_Tx_data_buf_maxquantity=0;

Rasp_Tx_data_buf_size=RASP_BUFSIZE;
Rasp_Rx_data_buf_bot=0;
Rasp_Rx_data_buf_quantity=0;
Rasp_Rx_data_buf_size=RASP_BUFSIZE;

UART1_RX_PIN_DIR=1;			//RASP RX RG9/RP27/pin8
UART1_TX_PIN_DIR=0;			//MASTER TX RG8/RP19/pin6
Delais(5);
UART1_RX_PIN_SELECT;		//RG9/RP27 assigné a Rx ( voir TAG#1 Tai_V1201_Comm_Hardware_Mapping.h)
UART1_TX_PIN_SELECT;		//Output Tx sur RP19/RG8, sur pin 5 ( voir TAG#1 Tai_V1201_Comm_Hardware_Mapping.h)

Delais(5);
//IPC2bits.U1RXIP=6;
//IPC3bits.U1TXIP=5;
IPC2bits.U1RXIP=4;
IPC3bits.U1TXIP=4;
#ifdef __HISPEED232	
U1BRG=BRGVAL1MHZ;			// Set baud rate to 1MHz
U1MODE=0x8008; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)
#else
U1BRG=BRGVAL_RASP+1;		// Set baud rate 112500 baud
U1MODE=0x8000; 				//Reset UART to 8-n-1, alt pins, and enable 
#endif

U1STA=0x04c0; 				//Reset status register and enable TX & RX, Rx interrupt on 4 bytes received
							//0x00c0=mode 4bits msg
IFS0bits.U1RXIF=0;			//Rx interrupt flag clear
IFS0bits.U1TXIF=0;			//Tx interrupt flag clear
IEC0bits.U1TXIE=0;			//clear Tx interrupt enable bit
IEC0bits.U1RXIE=0;			//Set Rx interrupt enable bit

RaspCommInited=1;

}
/****************************************************************
*****************************************************************
|FONCTIONS:	void Init_RASP_Uart2(void)							|
|Notes:C'est le UART du Raspberry pi baud rate 115200			|
|																|
|Arguments:														|
|Variables locals:												|
|																|
|Variables globals:												|
|																|
*****************************************************************
****************************************************************/
void Init_RASP_Uart2(void)
{	
//version sortie sur pin de PORT RASPBERRY PI
unsigned short n;
for(n=0;n<48;n++)
	{
	Temp_data[n]=0;
	MsgData[n]=0;
	}
MsgIndex=0;
for(n=0;n<RASP_BUFSIZE;n++)
	{
	Rasp_Tx_data_buf[n] = 0;
	Rasp_Rx_data_buf[n] = 0;
	}
Rasp_Tx_data_buf_top=0;
Ack_flag_response=0;
Rasp_Tx_data_buf_bot=0;
Rasp_Tx_data_buf_quantity=0;
Rasp_Tx_data_buf_size=RASP_BUFSIZE;
Rasp_Rx_data_buf_bot=0;
Rasp_Rx_data_buf_quantity=0;
Rasp_Rx_data_buf_size=RASP_BUFSIZE;

UART1_RX_PIN_DIR=1;			//RASP RX RG9/RP27/pin8
UART1_TX_PIN_DIR=0;			//MASTER TX RG8/RP19/pin6
UART1_RX_PIN_SELECT;		//RG9/RP27 assigné a Rx ( voir TAG#1 Tai_V1201_Comm_Hardware_Mapping.h)
UART1_TX_PIN_SELECT;		//Output Tx sur RP19/RG8, sur pin 5 ( voir TAG#1 Tai_V1201_Comm_Hardware_Mapping.h)
IPC2bits.U1RXIP=4;
IPC3bits.U1TXIP=5;
U1BRG=BRGVAL1MHZ;			// Set baud rate to 1MHz
U1MODE=0x8008; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)

U1STA=0x04c0; 				//Reset status register and enable TX & RX, Rx interrupt on 4 bytes received
							//mode 4bits msg
IFS0bits.U1RXIF=0;			//Rx interrupt flag clear
IFS0bits.U1TXIF=0;			//Tx interrupt flag clear
IEC0bits.U1TXIE=0;			//clear Tx interrupt enable bit
IEC0bits.U1RXIE=1;			//Set Rx interrupt enable bit

RaspCommInited=2;

}

void setRaspUartBootloadMode(void)
{
U1MODEbits.UARTEN=0; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)
U1STAbits.URXISEL=0; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)
IFS0bits.U1RXIF=0;			//Rx interrupt flag clear
IFS0bits.U1TXIF=0;			//Tx interrupt flag clear
IEC0bits.U1TXIE=0;			//clear Tx interrupt enable bit
IEC0bits.U1RXIE=1;			//Set Rx interrupt enable bit
U1MODEbits.UARTEN=1; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)
}

void setRaspUartRunMode(void)
{
U1MODEbits.UARTEN=0; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)
U1STAbits.URXISEL=3; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)
IFS0bits.U1RXIF=0;			//Rx interrupt flag clear
IFS0bits.U1TXIF=0;			//Tx interrupt flag clear
IEC0bits.U1TXIE=0;			//clear Tx interrupt enable bit
IEC0bits.U1RXIE=1;			//Set Rx interrupt enable bit
U1MODEbits.UARTEN=1; 				//Reset UART to 8-n-1, alt pins, and enable + high baud rate (brgh=1)
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void ResetRASPUart(void)							|
|Notes:C'est le UART du Raspberry pi baud rate 115200			|
|																|
|Arguments:														|
|Variables locals:												|
|																|
|Variables globals:												|
|																|
*****************************************************************
****************************************************************/
void ResetRASPUart(void)
{
unsigned short n;

RaspCommInited=0;
IFS0bits.U1RXIF=0;			//Rx interrupt flag clear
IFS0bits.U1TXIF=0;			//Tx interrupt flag clear
IEC0bits.U1TXIE=0;			//clear Tx interrupt enable bit
IEC0bits.U1RXIE=1;			//Set Rx interrupt enable bit

for(n=0;n<RASP_BUFSIZE;n++)
	{
	Rasp_Tx_data_buf[n] = 0;
	Rasp_Rx_data_buf[n] = 0;
	}
Rasp_Tx_data_buf_quantity=0;
Rasp_Rx_data_buf_quantity=0;
Rasp_Tx_data_buf_top=0;
Rasp_Rx_data_buf_top=0;
Rasp_Tx_data_buf_bot=0;
Rasp_Rx_data_buf_bot=0;
	

//New_Rx485_data_flag=0;

Init_RASP_Uart1();
}


/******************************************************************************
 * Function:        void Stop_Uart(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        cette routine s'occupe d'initialiser le UART
 *                  initialization that is required.
 *
 * Note:
 *
 *****************************************************************************/
void Stop_RASP_Uart1(void)
{	
	U1MODE 	= 0x0000; 		// Reset UART to 8-n-1, alt pins, and enable 
}

//----------------------------------------------------------------
//----------------------------------------------------------------
// Function:        char Rasp_Msg_Manager(void)
// PreCondition:    None
//
// Input:           None
// Output:          None
//
// Side Effects:    None
//
// Overview:        gestion des message en RX du port RS485 EXT
//
// Note:            None
//----------------------------------------------------------------
//----------------------------------------------------------------
char RaspMsgParser(void)
{
unsigned char tempdatabuf[4];
unsigned char msgtype,start_flag,data_cnt;
char errcode,exit_flag;
if(NewRaspMsgFlag)
	{
	errcode=0;
	msgtype=0;
	start_flag=0;
	exit_flag=0;
	data_cnt=0;
	do{
		Pull_Rasp_Rx_Data(tempdatabuf);			//Va chercher l'entete du message
		if(!start_flag)
			{
			if(((tempdatabuf[0]&0xf0)>0x80)&&((tempdatabuf[0]&0xf0)<0xf0))
				msgtype=1;
			else
			if(tempdatabuf[0]==0xfd)
				msgtype=2;
			}
		if(msgtype==1)		
			{
			Temp_data[data_cnt++]=tempdatabuf[0];
			Temp_data[data_cnt++]=tempdatabuf[1];
			Temp_data[data_cnt++]=tempdatabuf[2];
			Temp_data[data_cnt++]=tempdatabuf[3];
			exit_flag=1;
			}
		else
		if(msgtype==2)		
			{
			if(tempdatabuf[0]!=0xfd)
				Temp_data[data_cnt++]=tempdatabuf[0];
			Temp_data[data_cnt++]=tempdatabuf[1];
			Temp_data[data_cnt++]=tempdatabuf[2];
			if(tempdatabuf[3]!=0xfe)
				Temp_data[data_cnt++]=tempdatabuf[3];
			else
				exit_flag=1;
			}
		start_flag=1;
		}while(!exit_flag);

		if(NewRaspMsgFlag)
			NewRaspMsgFlag--;
		//if(msgtype==1)
		//	TAIMsgParser(Temp_data);
		//else
		if(msgtype==2)
			WWWMsgParser(Temp_data);
	}	
return 1;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	unsigned char getRaspTxBufferQuantity(void)
//
//Notes:	pousse une donnée dans un buffer circulaire
//		sans test d'overflow
//arguments	:n le data
//			:le No. du buffer correspondant au No.slave
//
//
//
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char getRaspTxBufferQuantity(void)
{
return Rasp_Tx_data_buf_quantity;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	unsigned char getRaspTxBufferMaxQuantity(void)
//
//Notes:	pousse une donnée dans un buffer circulaire
//		sans test d'overflow
//arguments	:n le data
//			:le No. du buffer correspondant au No.slave
//
//
//
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char getRaspTxBufferMaxQuantity(void)
{
return Rasp_Tx_data_buf_maxquantity;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	unsigned char getRaspRxBufferQuantity(void)
//
//Notes:	pousse une donnée dans un buffer circulaire
//		sans test d'overflow
//arguments	:n le data
//			:le No. du buffer correspondant au No.slave
//
//
//
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char getRaspRxBufferQuantity(void)
{
return Rasp_Rx_data_buf_quantity;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	unsigned char getRaspRxBufferMaxQuantity(void)
//
//Notes:	pousse une donnée dans un buffer circulaire
//		sans test d'overflow
//arguments	:n le data
//			:le No. du buffer correspondant au No.slave
//
//
//
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char getRaspRxBufferMaxQuantity(void)
{
return Rasp_Rx_data_buf_maxquantity;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	unsigned char getRaspSingleDataBufferQuantity(void)
//
//Notes:	pousse une donnée dans un buffer circulaire
//		sans test d'overflow
//arguments	:n le data
//			:le No. du buffer correspondant au No.slave
//
//
//
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char getRaspSingleDataBufferQuantity(void)
{
return RaspSingleDataBuf_quantity;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	unsigned char getRaspSingleDataBufferMaxQuantity(void)
//
//Notes:	pousse une donnée dans un buffer circulaire
//		sans test d'overflow
//arguments	:n le data
//			:le No. du buffer correspondant au No.slave
//
//
//
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char getRaspSingleDataBufferMaxQuantity(void)
{
return RaspSingleDataBuf_maxquantity;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
// Fonction: Gestion des messages Data a transmettre via la buffer
// vers le RASP
//
//
//
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char Send_Rasp_Data(unsigned char TxData)
{
#ifdef __RASP
if(U1STAbits.TRMT)
	{
	U1TXREG=TxData;
	IEC0bits.U1TXIE = 1;	// Set Tx interrupt enable bit
	}
else
	{
	Push_Rasp_Tx_Data(TxData);
	IEC0bits.U1TXIE = 1;	// Set Tx interrupt enable bit
	}
#endif

return 1;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char Push_Rasp_Tx_Data(unsigned char n)
|Notes:	pousse une donnée dans un buffer circulaire
|		sans test d'overflow
|arguments	:n le data
|			:le No. du buffer correspondant au No.slave
|
|
|
*****************************************************************
****************************************************************/
unsigned char Push_Rasp_Tx_Data(unsigned char n)
{
if (Rasp_Tx_data_buf_quantity>=RASP_BUFSIZE)
	return 0;
Rasp_Tx_data_buf[Rasp_Tx_data_buf_top] = n;			//sauve la valeur dans le buffer
Rasp_Tx_data_buf_top++;
if (Rasp_Tx_data_buf_top >= RASP_BUFSIZE)	//test si on doit rouler au début
	Rasp_Tx_data_buf_top = 0;						//si oui roule au début

Rasp_Tx_data_buf_quantity++;
if(Rasp_Tx_data_buf_maxquantity<Rasp_Tx_data_buf_quantity)
	Rasp_Tx_data_buf_maxquantity=Rasp_Tx_data_buf_quantity;
if (Rasp_Tx_data_buf_quantity>=RASP_BUFSIZE)						//
	DebugSet(0x0080);

return 1;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char PushRaspSingleData(unsigned char n)
|Notes:	pousse une donnée dans un buffer circulaire
|		sans test d'overflow
|arguments	:n le data
|			:le No. du buffer correspondant au No.slave
|
|
|
*****************************************************************
****************************************************************/
unsigned char PushRaspSingleData(unsigned char index,unsigned char data)
{
if (RaspSingleDataBuf_quantity>=16)
	return 0;
RaspSingleIndexBuf[RaspSingleDataBuf_top] = index;			//sauve la valeur dans le buffer
RaspSingleDataBuf[RaspSingleDataBuf_top] = data;			//sauve la valeur dans le buffer
RaspSingleDataBuf_top++;
if (RaspSingleDataBuf_top >= 16)	//test si on doit rouler au début
	RaspSingleDataBuf_top = 0;						//si oui roule au début
RaspSingleDataBuf_quantity++;
if(RaspSingleDataBuf_maxquantity<RaspSingleDataBuf_quantity);
	RaspSingleDataBuf_maxquantity=RaspSingleDataBuf_quantity;
if (RaspSingleDataBuf_quantity>=16)						//
	DebugSet(0x0080);
return 1;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char PullRaspSingleData(unsigned char n)
|Notes:	pousse une donnée dans un buffer circulaire
|		sans test d'overflow
|arguments	:n le data
|			:le No. du buffer correspondant au No.slave
|
|
|
*****************************************************************
****************************************************************/
unsigned short PullRaspSingleData(void)
{
unsigned short data,index;
if(RaspSingleDataBuf_quantity)
	{
	index=RaspSingleIndexBuf[RaspSingleDataBuf_bot];
	data=RaspSingleDataBuf[RaspSingleDataBuf_bot];
	RaspSingleDataBuf_quantity--;
	RaspSingleDataBuf_bot++;	
	if (RaspSingleDataBuf_bot >= 16)		//test si on doit rouler au début
		RaspSingleDataBuf_bot = 0;			//si oui roule au début
	return (data|(index<<8));
	}
return -1;
}
/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char Pull_Rasp_Tx_Data(void)
|
|Notes:	retir une donnée dans un buffer circulaire
|		sans test d'overflow			
|arguments 	:n la donnéela strucutre de midi message
|			:Buffer_num le buffer de messag midi en Rx ou Tx
|
|
*****************************************************************
****************************************************************/
short Pull_Rasp_Tx_Data(void)
{
unsigned short data;
if(Rasp_Tx_data_buf_quantity)
	{
	data=Rasp_Tx_data_buf[Rasp_Tx_data_buf_bot];
	if(Rasp_Tx_data_buf_quantity)
		Rasp_Tx_data_buf_quantity--;
	Rasp_Tx_data_buf_bot++;	
	if (Rasp_Tx_data_buf_bot >= RASP_BUFSIZE)		//test si on doit rouler au début
		Rasp_Tx_data_buf_bot = 0;							//si oui roule au début
	return data;
	}
return -1;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char Push_Rasp_Rx_Data(unsigned char n)
|Notes:	pousse une donnée dans un buffer circulaire
|		sans test d'overflow
|arguments	:n le data
|			:le No. du buffer correspondant au No.slave
|
|
|
*****************************************************************
****************************************************************/
unsigned char Push_Rasp_Rx_Data(unsigned char *databuffer)
{
RaspRxBufQnt=Rasp_Rx_data_buf_quantity;
if (Rasp_Rx_data_buf_quantity>=RASP_BUFSIZE)
	return 0;
Rasp_Rx_data_buf[Rasp_Rx_data_buf_top++]=databuffer[0];
Rasp_Rx_data_buf[Rasp_Rx_data_buf_top++]=databuffer[1];
Rasp_Rx_data_buf[Rasp_Rx_data_buf_top++]=databuffer[2];
Rasp_Rx_data_buf[Rasp_Rx_data_buf_top++]=databuffer[3];
if (Rasp_Rx_data_buf_top >= RASP_BUFSIZE)	//test si on doit rouler au début
	Rasp_Rx_data_buf_top = 0;						//si oui roule au début
Rasp_Rx_data_buf_quantity=Rasp_Rx_data_buf_quantity+4;
if (Rasp_Rx_data_buf_quantity>=RASP_BUFSIZE)
	DebugSet(0x0040);					//
return 1;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned int Pull_Rasp_Rx_Data(unsigned char *databuffer)
|
|Notes:	retir une donnée dans un buffer circulaire
|		sans test d'overflow			
|arguments 	:n la donnéela strucutre de midi message
|			:Buffer_num le buffer de messag midi en Rx ou Tx
|
|
*****************************************************************
****************************************************************/
short Pull_Rasp_Rx_Data(unsigned char *databuffer)
{
if(Rasp_Rx_data_buf_quantity>3)
	{
	databuffer[0]=Rasp_Rx_data_buf[Rasp_Rx_data_buf_bot];
	databuffer[1]=Rasp_Rx_data_buf[Rasp_Rx_data_buf_bot+1];
	databuffer[2]=Rasp_Rx_data_buf[Rasp_Rx_data_buf_bot+2];
	databuffer[3]=Rasp_Rx_data_buf[Rasp_Rx_data_buf_bot+3];
	Rasp_Rx_data_buf_bot=Rasp_Rx_data_buf_bot+4;
	if(Rasp_Rx_data_buf_quantity>3)
		Rasp_Rx_data_buf_quantity=Rasp_Rx_data_buf_quantity-4;
	else
		return -1;
	if (Rasp_Rx_data_buf_bot >= RASP_BUFSIZE)				//test si on doit rouler au début
		Rasp_Rx_data_buf_bot = 0;							//si oui roule au début
	return 1;
	}
return -1;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	interruption du UART TX #1							|
|Notes:interrupt de Tx vers le RASP															|
|																|
|Arguments:														|
|Variables locals:												|
|																|
|Variables globals:												|
|																|
*****************************************************************
****************************************************************/
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) 
{
if (!U1STAbits.OERR)
	{
	if(Rasp_Tx_data_buf_quantity)	//Si pas d'erreur et buffer pas vide
		{
		if(!U1STAbits.UTXBF) 
			{
			U1TXREG = Rasp_Tx_data_buf[Rasp_Tx_data_buf_bot];
			Rasp_Tx_data_buf_bot++;
			Rasp_Tx_data_buf_quantity--;
			if (Rasp_Tx_data_buf_bot == Rasp_Tx_data_buf_size)
				Rasp_Tx_data_buf_bot =0;
			}
		}
	else
 		{
 		while(!U1STAbits.TRMT);
			IEC0bits.U1TXIE = 0;		// Arreter Tx interrupt enable bit
		}
	}
else
	{
 	while(!U1STAbits.TRMT);
		IEC0bits.U1TXIE = 0;			// Arreter Tx interrupt enable bit
	}
IFS0bits.U1TXIF=0;
  }

/****************************************************************
*****************************************************************
|FONCTIONS:	interruption du UART RX #1(RASP)					|
|Notes:	Interrupt des Rx du RASP								|
|																|
|Arguments:														|
|Variables locals:												|
|																|
|Variables globals:												|
|																|
*****************************************************************
****************************************************************/
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
unsigned char data[4],n;
unsigned char errchk;
static char MsgStarted;

if(!passThroughMode)
	{
	data[0] =U1RXREG;
	data[1] =U1RXREG;
	data[2] =U1RXREG;
	data[3] =U1RXREG;
	}

if(U1STAbits.FERR)
	{
	MsgStarted=0;
	MsgIndex=0;
	U1STAbits.OERR=0;
	U1STAbits.FERR=0;
	}
else
if(U1STAbits.OERR)
	{
	MsgStarted=0;
	MsgIndex=0;
	U1STAbits.OERR=0;
	U1STAbits.FERR=0;
	}
else
if(BootStateFlag<4)
	{
	RaspCommInited=2;
	MsgStarted=0;
	MsgIndex=0;
	U1STAbits.OERR=0;
	U1STAbits.FERR=0;
	}
else
	{
	if(!passThroughMode)
		{
		if((data[0]==0xfd)&&(data[3]==0xfe))
			{
			MsgData[MsgIndex++]=data[0];
			MsgData[MsgIndex++]=data[1];
			MsgData[MsgIndex++]=data[2];
			MsgData[MsgIndex++]=data[3];
			if((Rasp_Rx_data_buf_quantity+MsgIndex)<=RASP_BUFSIZE)
				{
				for(n=0;n<MsgIndex;n=n+4)
					Push_Rasp_Rx_Data(MsgData+n);
				NewRaspMsgFlag++;
				}
			else
				errchk=0;			
			}
		MsgIndex=0;
		}	
	else
		{
		while(U1STAbits.URXDA)
			PushPassThroughData(U1RXREG);
		}	
	}
IFS0bits.U1RXIF=0;
}
