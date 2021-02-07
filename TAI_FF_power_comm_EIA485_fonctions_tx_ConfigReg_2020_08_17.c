#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
#include "TAI_FF_power_comm_EIA485_functions_V1.h"
#include "TAI_FF_power_comm_config_manager_fonctions.h"
#include "TAI_FF_power_comm_REAPER_Process_Functions.h"
#include "TAI_FF_globales_externes.h"
#include "Debug.h"

//Local globals
unsigned short Tx485_Buffer[EI485BUFSIZE];
unsigned short Tx485_Buffer_Bot;
unsigned short Tx485_Buffer_Top;
unsigned short Tx485_Buffer_Quant;
unsigned short Rx485_Buffer[EI485BUFSIZE];
unsigned short Rx485_Buffer_Bot;
unsigned short Rx485_Buffer_Top;
unsigned short Rx485_Buffer_Quant;
unsigned char Rx485_Buffer_Over_run;
unsigned char New_Rx485_data_flag;

unsigned short Rx_From_Message_address;
unsigned short Rx_To_Message_address;
unsigned char Rx_Msg_lenght;
unsigned char Tx485_Msg_Cnt;
unsigned char last_data_flag;
unsigned char Rx_Message_Code;
unsigned short Rx485_Data;
unsigned char Active_msg_flag;
unsigned char EIA485data_cnt;
unsigned char RxChan;
unsigned char txCfgIdx;
unsigned char LocalConfigIndex;
unsigned char LocalConfigData;

unsigned char eiaTestBuffer[16];
unsigned char eiaTestIndex=0;;

#define CANCEL_FLAG 0x0001
#define DOT_FLAG 0x0002
#define ENTER_FLAG 0x0004
#define SRC_RASP 5
#define RECALLIT 4
#define SEKA_CARD_ID 0x0106	
#define POWER_COMM_ID 0x0100	

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
|FONCTIONS:void EIA485_Init(void)
|Notes:						
|
|Arguments:
|Variables locals:
|
|Variables globals:
|
*****************************************************************
****************************************************************/
void EIA485_Init(void)
{
//unsigned char m;
unsigned short n;
n = EI485BUFSIZE;
txCfgIdx=0;
while (n) // initialise le Data tx buffer
	{
	n--;
	Tx485_Buffer[n]=0;
	}
Tx485_Buffer_Bot= 0;
Tx485_Buffer_Top= 0;
Rx485_Buffer_Bot= 0;
Rx485_Buffer_Top= 0;

passThroughRx485Flag=0;
passThroughTx485Index=0;
passThroughRx485Index=0;
passThroughTx232Index=0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	Inititialisation du UART2,le port RS485				|
|Notes:															|
|																|
|Arguments:														|
|Variables locals:												|
|																|
|Variables globals:												|
|																|
*****************************************************************
****************************************************************/
void StartEIA485Uart(void)
{

UART2_RX_PIN_DIR= 1;		//UART2_RX pin 3
UART2_TX_PIN_DIR= 0;		//UART2_TX pin 2
UART2_RTS_DIR =0;
UART2_RTS=0;				//pin= 0, RE/DE pret a recevoirpour le RS485

UART2_RX_PIN_SELECT;		//Make RP6 U2RX on pin3 (voir TAG#2 Tai_V1201_Comm_Hardware_Mapping.h)
UART2_TX_PIN_SELECT;		//Make RP5 U2TX on pin2 (voir TAG#2 Tai_V1201_Comm_Hardware_Mapping.h)

IPC7bits.U2RXIP=6;			//5Gestion de la priorité des interrupts de l'UART #2 en RX
IPC7bits.U2TXIP=6;			//7Gestion de la priorité des interrupts de l'UART #2 en TX

U2BRG  	= BRGVAL_EIA485;			//Set baud rate
U2MODE 	= 0x801e; 			//Set UART to 9-n-1, alt pins, and enable 
U2STA  	= 0x6400; 			//Reset status register and enable TX & RX, iddle 
IFS1bits.U2RXIF = 0;		//Rx interrupt flag clear
IEC1bits.U2RXIE = 1;		//Set Rx interrupt enable bit
IFS1bits.U2TXIF = 0;		//Tx interrupt flag clear
IEC1bits.U2TXIE = 0;		//clear Tx interrupt enable bit

}

/********************************************************************
* Function:        unsigned char RS485CommManager(unsigned char slave_numb)

* PreCondition:    None
*
* Input:           None
*
* Output:          None
*
* Side Effects:    None
*
* Overview:        gestion des message en RX du port RS485 EXT
*                  
*                  
*
* Note:            None
*******************************************************************/
unsigned char EIA485MsgParser(void)
{
unsigned short Temp_data[24];
unsigned short n,err;

if(New_Rx485_data_flag)
	{
	Pull_Rx485_Data();			//Va chercher l'adresse du message
	Temp_data[0]=Rx485_Data;	//sauve cette adresss
	Pull_Rx485_Data();			//Le code du message
	Temp_data[1]=Rx485_Data;	//sauve la longueur du message
	for(n=0;n<Temp_data[1];n++)
		{
		Pull_Rx485_Data();			//Le code du message
		Temp_data[n+2]=Rx485_Data;	//sauve le code du message
		if(n>20)
			err=1;
		}
	switch(Temp_data[0])
		{
		case 0x0106:if(Temp_data[1]==2)
						{
						if(Temp_data[2]==0xa1)			//si le code du message du transport SSL
							{
							//Manage transport switch from SSL
							if((Temp_data[3]>=0xe0)&&(Temp_data[3]<=0xef))
								{
								HUIMsgParser(Temp_data[3]);
								}
							}
						else									
						if(Temp_data[2]==0xa0)			//si le code du message du KBD SSL
							{
							SSLKbdMsgParser(Temp_data[3]);
							}
						}
					else
					if(Temp_data[1]==1)
						{
						if(Temp_data[2]>0xe0)
							err=0;
						}
					else
						err=0;

		break;
		//Prévoir les message du Motorfader pack
		default:break;
		}
	if(!Rx485_Buffer_Quant)
		New_Rx485_data_flag=0;
	return 1;
	}
return 0;
}

/*****************************************************************
*****************************************************************/
// Fonction: Gestion des messages Data a transmettre via la buffer
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned char Send_One_485(unsigned int TxData)
{
Push_Tx485_Data(TxData);
if(TxData==0x1fe)
	{
	Tx485_Msg_Cnt++;
	}
return 1;
}

/*****************************************************************
*****************************************************************/
// Fonction: Gestion des messages Data a transmettre via la buffer
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned char Get485MsgCnt(void)
{
return Tx485_Msg_Cnt;
}

/*****************************************************************
*****************************************************************/
// Fonction: unsigned char Push_RS485_Tx_Data(unsigned short data)
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned char Push_Tx485_Data(unsigned short Data)
{
if(Tx485_Buffer_Quant >=EI485BUFSIZE)
	return 0; 								// il y a un buffer overflow
Tx485_Buffer[Tx485_Buffer_Top] = Data;
Tx485_Buffer_Top++; 						// incrémente le buffer	
if(Tx485_Buffer_Top >= EI485BUFSIZE)
	Tx485_Buffer_Top=0;
Tx485_Buffer_Quant++;						//ajuste la quantité de données dans le buffer de Tx
if(Tx485_Buffer_Quant >=EI485BUFSIZE)
	DebugSet(0x0008);

return 1;
}
/*****************************************************************
*****************************************************************/
// Fonction: unsigned char Get_Tx485_Data_Quantity(void)
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned short Get_Tx485_Data_Quantity(void)
{
return Tx485_Buffer_Quant;
}

/*****************************************************************
*****************************************************************/
// Fonction: unsigned char Pull_Tx485_Data(void)
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned short Pull_Tx485_Data(void)
{
unsigned char TempTop;
TempTop = Tx485_Buffer_Top;
if(TempTop != Tx485_Buffer_Bot)
	{
	Tx485_Data=Tx485_Buffer[Tx485_Buffer_Bot];
	Tx485_Buffer_Bot++; 
	if(Tx485_Buffer_Bot == EI485BUFSIZE)
		Tx485_Buffer_Bot=0;
	Tx485_Buffer_Quant--;	 								// il y a un buffer overflow
	return Tx485_Buffer_Quant;
	}
return 0;
}

/*****************************************************************
*****************************************************************/
// Fonction: unsigned char Push_Rx485_Data(unsigned short data)
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned char Push_Rx485_Data(unsigned short Data)
{
if(Rx485_Buffer_Quant >= EI485BUFSIZE)
	return 0; 								// il y a un buffer overflow
Rx485_Buffer[Rx485_Buffer_Top] = Data;
Rx485_Buffer_Top++; 						// incrémente le buffer	
if(Rx485_Buffer_Top >= EI485BUFSIZE)
	Rx485_Buffer_Top=0;

Rx485_Buffer_Quant++;						//ajuste la quantité de données dans le buffer de Tx
if(Rx485_Buffer_Quant >=EI485BUFSIZE)
	DebugSet(0x0004);
return 1;
}

/*****************************************************************
*****************************************************************/
// Fonction: unsigned char Get_Rx485_Data_Quantity(void)
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned short Get_Rx485_Data_Quantity(void)
{
return Rx485_Buffer_Quant;
}

/*****************************************************************
*****************************************************************/
// Fonction: unsigned short Pull_Rx485_Data(void)
// transmition via le port seriel #2 le port RS485
//
//
//
/*****************************************************************
*****************************************************************/
unsigned short Pull_Rx485_Data(void)
{
unsigned char TempTop;
TempTop = Rx485_Buffer_Top;
if(TempTop != Rx485_Buffer_Bot)
	{
	Rx485_Data = Rx485_Buffer[Rx485_Buffer_Bot];
	Rx485_Buffer_Bot++; 		// monte d'une coche le buffer
	Rx485_Buffer_Quant--;	//ajuste la quantité de donné dans le buffer de Rx	
	if (Rx485_Buffer_Bot == EI485BUFSIZE)
		Rx485_Buffer_Bot =0;				// roule au debut du buffer
	return Rx485_Buffer_Quant;
	}
return 0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	interruption du UART TX #2(rs485 ext)				|
|Notes:															|
|																|
|Arguments:														|
|Variables locals:												|
|																|
|Variables globals:												|
|																|
*****************************************************************
****************************************************************/
void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) 
{
int chnidx,done;
unsigned char data;
T5CON = 0x0000;										//Reset le compteur 
IFS1bits.T5IF=0;
IEC1bits.T5IE=0;
if ((!U2STAbits.UTXBF)&&(!U2STAbits.OERR))
	{
	if(Tx_From_Message_address==0x101)	
		{
		if(Tx_data_count<3)
			{
			switch(Tx_data_count)
				{
				case 1:
					U2TXREG=MsgLenght;//MsgLenght=29
					Tx_data_count=2;
				break;
				case 2:
					U2TXREG=TxChan;
					Tx_data_count=3;
					TxDataIndex=0;
				break;
				}
			}
		else
		if(TxDataIndex<(MsgLenght-2))//MsgLenght= moins de 27
			{
			chnidx=TxChan+(TxDataIndex/3);
			if(TxDataIndex<(MsgLenght-5))//MsgLenght= moins de 24
				{
				switch(TxDataIndex%3)
					{
					case 0:
						U2TXREG=txToSekaFaderData[chnidx]&0x00ff;
					break;
					case 1:
						U2TXREG=(txToSekaFaderData[chnidx])>>8;
					break;
					case 2:
						U2TXREG=txToSekaFaderLedState[chnidx]^((ledFlashState1[chnidx]&flashByte1)|(ledFlashState2[chnidx]&flashByte2));
					break;
					default:break;
					}
				}
			else
				{
				switch(TxDataIndex%3)
					{
					case 0:
						if(!passThroughMode)
							{
							if(Config_registers[0])
								U2TXREG=Config_registers[0];
							else
								U2TXREG=txCfgIdx;
							}
						else
							U2TXREG=0xa5;
					break;
					case 1:
						if(!passThroughMode)
							{
							if(Config_registers[0])
								{
								U2TXREG=Config_registers[Config_registers[0]];
								Config_registers[0]=0;
								}
							else
								{
								U2TXREG=Config_registers[txCfgIdx];
								txCfgIdx++;
								if(txCfgIdx>=128)
									txCfgIdx=1;			
								}
							}
						else
							{
							if((passThroughRx485Flag==2)
								&&(PullPassThroughData(&data)!=-1))	//if not empty
								{
								TESTPIN2_WRITE^=1;
								U2TXREG=data;					//passthrough data#1 from buffer
								if(data==0x04)
									passThroughRx485Flag=3;
								}
							else
								{
								if(passThroughRx485Flag==2)
									passThroughRx485Flag=0;
								U2TXREG=0x00;
								}
							}	
					break;
					case 2:	
						switch(TxChan)
							{
							case 0:U2TXREG=MCMALedState[0];break;
							case 8:U2TXREG=MCMALedState[1];break;
							case 16:U2TXREG=MCMALedState[2];break;
							case 24:U2TXREG=MCMALedState[3];break;
							default:U2TXREG=0;break;
							}
					break;
					}
				}
			TxDataIndex++;
			}
		else
			{
			while(!U2STAbits.TRMT);
				{
				UART2_RTS=0;
				IEC1bits.U2TXIE = 0;					// Arreter Tx interrupt enable bit
				Tx_data_count=0;
				}
			}
		}
	}
IFS1bits.U2TXIF = 0;						//clear Tx interrupt flag
}

//---------------------------------------------------------------
//---------------------------------------------------------------
//FONCTIONS:	interruption du UART RX #2(rs485 ext)
//Notes:	msg #1 = destination adresse
//		msg #2 = source adresse
//		msg #3 = data 1 (msg type)
//		msg #4 = data 2	(msg lenght)
//Arguments:
//Variables locals:
//
//Variables globals:
//
//---------------------------------------------------------------
//---------------------------------------------------------------
void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt( void )
{
unsigned short err,data;
if (U2STAbits.OERR)
	{
	U2STAbits.OERR=0;	
	err=U2RXREG;
	err=U2STA;
	}
else
	{
	while(U2STAbits.URXDA)
		{
		data= U2RXREG;
		if(data>=POWER_COMM_ID)															//Test si message de type adresse
			{
			if (SEKA_CARD_ID == data)													//si adresse du master comm.
				{
				Rx_To_Message_address=data;
				Active_msg_flag=1;												//Passe en mode actif=1;
				EIA485data_cnt=1;												//reset compteur de message
				Ack_flag_response=1;											// met le Ack response flag a 1
				RxDataIndex=0;
				}
			else
			if(Active_msg_flag==1)												//Si 1er donnée, c'est l'adresse de source, la mettre en mémoire temporaire
				{
				Active_msg_flag=0;
				Rx_Msg_lenght=0;
				EIA485data_cnt=0;												//compteur de message a 1
				Ack_flag_response=0;											// met le Ack response flag a 1
				}
			else
				err=1;
			}
		else
		if(Active_msg_flag)				//Si pas une addresse, gérer le data qui suit
			{
			data=data&0x00ff;
			if(EIA485data_cnt<3)
				{
				switch(EIA485data_cnt)
					{
					case 1:
						EIA485data_cnt=2;			//EIA485data_cnt =1 la longueur de message
						Rx_Msg_lenght=data-2;
					break;
			
					case 2:	EIA485data_cnt=3;		//data_cnt =2 le code du message
							RxChan=data;
					break;
					}
				}
			else
			if(RxDataIndex<Rx_Msg_lenght)
				{
				if(RxDataIndex<(Rx_Msg_lenght-3))
					{
					switch(RxDataIndex%3)
						{
						case 0:	
							TempsRxmsg1=data;
						break;
						case 1:	
							rxFromSekaFaderData[RxChan+(RxDataIndex/3)]=TempsRxmsg1|(data<<8);
						break;
						case 2:	
							rxFromSekaSwtData[RxChan+(RxDataIndex/3)]=data;
						break;
						default:break;	
						}
					}
				else
					{
					switch(RxDataIndex%3)
						{
						case 0:
							if(!passThroughMode)
								LocalConfigIndex=data;
						break;
						case 1:
							if(!passThroughMode)
								{
								if(LocalConfigIndex)
									{
									if(LocalConfigIndex<128)
										Rx_Config_Data_Manager(LocalConfigIndex,data,1);
									}
								}
							else
								{
								switch(passThroughRx485Flag)
									{
									case 3:	
									case 4:
										if(data==0x55)
											{
											eiaTestBuffer[eiaTestIndex++]=data;
											passThroughRx485Flag++;
											PassThroughRx485Buffer[passThroughRx485Index++]=data;
											TESTPIN3_WRITE^=1;
											}
									break;
									case 5:
										if(data==0x55)
											{
											eiaTestIndex=0;	
											passThroughRx485Flag=0;
											passThroughRx485Index=0;
											}
										else
											{
											PassThroughRx485Buffer[passThroughRx485Index++]=data;
											TESTPIN3_WRITE^=1;
											if(data==0x04)
												{
												passThroughRx485Flag=6;
												passThroughTx232Index=0;
												}
											}
									break;
									}
								}	
						break;
						case 2:
							switch(RxChan)
								{
								case 0 :MCMASwitch[0]=data;break;
								case 8 :MCMASwitch[1]=data;break;
								case 16:MCMASwitch[2]=data;break;
								case 24:MCMASwitch[3]=data;break;
								default:break;
								}
						break;
						default:break;	
						}
					}
				RxDataIndex++;
				
				if(RxDataIndex==Rx_Msg_lenght)
					{
					SEKA_ONLINE_FLAG=1;
					WAIT_FOR_ACK_FLAG=0;
					EIA485data_cnt=0;										//data_cnt =2 le code du message
					Active_msg_flag=0;										//Passe en mode actif=1;
					}
				}
			}
		}
	}
IFS1bits.U2RXIF=0;
}
