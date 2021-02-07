
//2020_11_27 Code dev day 2: 
//	+ Put things in place
//	+ Clean-up old code section

//2020_11_26 Initial code
// 	+ Creat .c and .h files in project
//Includes --------------------------------------
#include "TAI_FF_Versionner.h"
#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_passThroughMode.h"
//-----------------------------------------------
//Globals -------------------------------------------------------
//DWORD_VAL userTimeout; //bootloader entry timeout value
//Variables for storing runaway code protection keys
WORD responseBytes; //number of bytes in command response
DWORD_VAL delay;
unsigned short t1State;
unsigned short t2State;
unsigned short t3State;
unsigned short t4State;
unsigned short t5State;
unsigned short ioState;
//Transmit/Recieve Buffer
BYTE buffer[MAX_PACKET_SIZE + 1];
BYTE txResponseBuffer[MAX_PACKET_SIZE + 1];
unsigned short EIA232TxDataCount;
unsigned short EIA232RxDataCount;
//--------------------------------------------------------------
int PTMain(void)
{
int state;
state=0;
responseBytes=0;
EIA232TxDataCount=0;
EIA232RxDataCount=0;
EIA485PassThroughTxMsgCnt=0;
EIA485PassThroughRxMsgCnt=0;
PTStopServices();
//PTTimerInit();
PTResetIo();
//PTEIA485Init();
PTEIA232StopInterrupts();
while (state<5)
	{
	switch(state)
		{
		case 0:
			state=PTGetCommand();
		break; 							//Get full AN851 command from UART
		case 1:
			state=PTHandleCommand();
		break; 							//Echo full AN851 command to SPI
		case 2:
			state=PTGetResponse();
		break; 						//Get full AN851 Response from SPI
		case 3:
			state=PTHandleResponse();
		break; 		//Echo full AN851 Response To UART
		case 4:
			passThroughRx485Flag=0;
			passThroughRx485Index=0;
			passThroughTx485Index=0;
			EIA485PassThroughTxMsgCnt=0;
			EIA485PassThroughRxMsgCnt=0;
				
			state=0;
			TESTPIN1_WRITE=0;
			TESTPIN2_WRITE=0;
			TESTPIN3_WRITE=0;
			TESTPIN4_WRITE=0;
		break; 		//Echo full AN851 Response To UART
		default:break;
		}
	}
if(state==5)
	{
	PTRestoreIoState();
	PTEIA232StartInterrupts();
	PTRestoreServices();
	return 0;
	}
Nop();
__asm__ volatile("RESET");
return 0;
}

//--------------------------------------------------------------
//stopServices: timers,spi,uart,usb,ad etc
//--------------------------------------------------------------
void PTStopServices(void)
{
t1State=T1CON;
t2State=T2CON;
t3State=T3CON;
t4State=T4CON;
t5State=T5CON;

T1CONbits.TON = 0;
//T2CONbits.TON = 0; //dont stop EIA485 Timer
T3CONbits.TON = 0;
T4CONbits.TON = 0;
T5CONbits.TON = 0;
//U1MODEbits.UARTEN=0;
//U1MODEbits.UARTEN=0;
SPI1STATbits.SPIEN=0;
SPI2STATbits.SPIEN=0;
}

//--------------------------------------------------------------
//void PTRestoreServices(void): timers,spi,uart,usb,ad etc
//--------------------------------------------------------------
void PTRestoreServices(void)
{
T1CON=t1State;
T2CON=t2State;
T3CON=t3State;
T4CON=t4State;
T5CON=t5State;

//T1CONbits.TON = 1;
//T2CONbits.TON = 0; //dont stop EIA485 Timer
//T3CONbits.TON = 1;
//T4CONbits.TON = 1;
//T5CONbits.TON = 0;
//U1MODEbits.UARTEN=0;
//U1MODEbits.UARTEN=0;
SPI1STATbits.SPIEN=0;
SPI2STATbits.SPIEN=0;
}
//--------------------------------------------------------------
//Timer2 Init
//--------------------------------------------------------------
void PTTimerInit(void)
{
T2CONbits.TON = 0;
T2CONbits.T32 = 1; // Setup Timer 2/3 as 32 bit timer incrementing every clock, (T3 config regs. slaved to T2)
IFS0bits.T3IF = 0; // Clear the Timer3 Interrupt Flag (because the T2-3 interrupt appears as the T3) 
IEC0bits.T3IE = 0; // Disable Timer3 Interrupt Service Routine (same)


}

//------------------------------------------------------------------------
// PTINITIO
//------------------------------------------------------------------------
void PTResetIo(void)
{
ioState=AD1PCFGL; 
AD1PCFGL = 0xFFFF; //TOUT LES PINS ANALOG SONT OFF
}

//------------------------------------------------------------------------
// PTINITIO
//------------------------------------------------------------------------
void PTRestoreIoState(void)
{
AD1PCFGL = ioState; //TOUT LES PINS ANALOG SONT OFF
}

//------------------------------------------------------------------------
//PTEIA485 INIT
//------------------------------------------------------------------------
void PTEIA485Init(void)
{
//no change in UART2 mode
//IFS1bits.U2RXIF = 0;		//Rx interrupt flag clear
//IEC1bits.U2RXIE = 0;		//Stop Rx interrupt enable bit and run in polled mode
}

//------------------------------------------------------------------------
//PTUART1 INIT
//------------------------------------------------------------------------
void PTEIA232StopInterrupts(void)
{
IFS0bits.U1RXIF = 0;		//Rx interrupt flag clear
IEC0bits.U1RXIE = 0;		//No Rx interrupt 
IFS0bits.U1TXIF = 0;		//Tx interrupt flag clear
IEC0bits.U1TXIE = 0;		//No Tx interrupt
}

//------------------------------------------------------------------------
//PTUART1 INIT
//------------------------------------------------------------------------
void PTEIA232StartInterrupts(void)
{
IFS0bits.U1RXIF = 0;		//Rx interrupt flag clear
IEC0bits.U1RXIE = 1;		//No Rx interrupt 
IFS0bits.U1TXIF = 0;		//Tx interrupt flag clear
IEC0bits.U1TXIE = 1;		//No Tx interrupt
}

//------------------------------------------------------------------------
// Function: 	int PTGetCommand()
// Precondition: UART Setup
// Input: 		None.
// Output:		None.
// Side Effects:	None.
// Overview: 	Polls the UART to recieve a complete AN851 command.
//			 	Fills buffer[1024] with recieved data.
//			
// Note:		 	None.
//------------------------------------------------------------------------
int PTGetCommand()
{
BYTE RXByte;
BYTE checksum;
WORD dataCount;
dataCount=0;
EIA232RxDataCount=0;
//int test;
while(1)
	{
	PTGetEIA232Char(&RXByte); //Get first STX
	if (RXByte == STX)
		{
		//T2CONbits.TON = 0; //disable timer - data received
		buffer[EIA232RxDataCount++]=RXByte;
		PTGetEIA232Char(&RXByte); //Read second byte
		if (RXByte == STX)
			{
           	//2 STX, beginning of data
			checksum = 0; //reset data and checksum
			dataCount=0;
			buffer[EIA232RxDataCount++]=RXByte;
			while (dataCount <= MAX_PACKET_SIZE + 1)
				{ //maximum num bytes
                PTGetEIA232Char(&RXByte);
                switch (RXByte)
					{
					case STX: //Start over if STX
					    checksum = 0;
					    dataCount=0;
						buffer[EIA232RxDataCount++]=RXByte;
					    break;
					
					case ETX: //End of packet if ETX
					    checksum = ~checksum + 1; //test checksum
					    Nop();
					    if (checksum == 0)
							{
							buffer[EIA232RxDataCount++]=RXByte;
							passThroughRx485Flag=1;
							return 1; //return if OK
							}
					    EIA232RxDataCount=0;
						dataCount = 0xFFFF; //otherwise restart
					    break;
					
					case DLE: //If DLE, treat next as data
					    PTGetEIA232Char(&RXByte);
					default: //get data, put in buffer
					    checksum += RXByte;
					    //buffer[dataCount++] = RXByte;
						buffer[EIA232RxDataCount++]=RXByte;
					    break;
					}
				}
			}
		}
	}				
return 0;
}

//------------------------------------------------------------------------
//  Function: 	int PTHandleCommand()
//  Precondition: data in buffer
//  Input: 		None.
//  Output:		None.
//  Side Effects:	None.
//  Overview: 	Handles commands received from host
//  Note:		 	None.
//------------------------------------------------------------------------
int PTHandleCommand()
{
int dataSentFlag,n;
dataSentFlag=0;
BYTE Command;


//variables used in EE and CONFIG read/writes


//--------------------------------------------------------RESET Command
if (EIA232RxDataCount == 0x00)
	return 0;
//--------------------------------------------------------
Command = buffer[2]; //get command from buffer
//Handle Commands		
switch (Command)
	{
	case EXIT_PT:return 5; //Return to RUN code
	break;
	case RESET:return 6; //Reboot device
	break;
	default:
		while(1)
			{
			while(EIA485PassThroughTxMsgCnt);//wait for EIA485 transmit to finish last message
				{
				for(n=0;n<EIA232RxDataCount;n++)
					PushPassThroughData(buffer[n]);
				EIA485PassThroughTxMsgCnt=EIA232RxDataCount;
				passThroughRx485Flag=2;
				}
				//for test PTHandleResponse(length);
			return 2; //return 0; = loop back messsages test mode...change to "return 2;" in normal mode
			}
	break;
    }
return 0;
}

//------------------------------------------------------------------------
// Function: 	int PTGetResponse(void)
// Precondition: UART and spi Setup, data in buffer
//  Input: 		None.
// Output:		None.
// Side Effects:	None.
// Overview: 	Transmits responseBytes bytes from SPI of data from buffer 
//              to UART as a response to received command.
// Note:		None.
//------------------------------------------------------------------------
int PTGetResponse(void)
{
int n;

while(passThroughRx485Flag!=6);

for(n=0;n<passThroughRx485Index;n++)
	txResponseBuffer[n]=PassThroughRx485Buffer[n];

responseBytes=passThroughRx485Index;
return 3;
}

//------------------------------------------------------------------------
// Function: 		int PTHandleResponse(void)
// Precondition:	UART and SPI Setup, data in buffer
// Input: 			reponse lenght tp transmit.
// Output:			None.
// Side Effects:	None.
// Overview: 		Transmits responseBytes bytes of data from buffer 
//              	to UART as a response to received command from SPI.
// Note:			None.
//------------------------------------------------------------------------
int PTHandleResponse(void)
{
int n;

for (n = 0; n < responseBytes; n++)
    PTPutEIA232Char(txResponseBuffer[n]); //get data from response buffer

while (!U1STAbits.TRMT); //wait for transmit to finish

return 4;				//back to get command from UART
}

//--------------------------------------------------------------------
// Function:        void GetEIA232Char(BYTE * ptrChar)
// PreCondition:    UART Setup
// Input:			ptrChar - pointer to character received
// Output:		
// Side Effects:	Puts character into destination pointed to by ptrChar.
//					Clear WDT
// Overview:		Receives a character from UART1.  (EIA232)
// Note:			None
//--------------------------------------------------------------------
void PTGetEIA232Char(BYTE * ptrChar)
{
BYTE dummy;
char tempcar;
while (1)
	{
    asm("clrwdt"); //looping code, so clear WDT (watchdog timer)
	//check for receive errors
	if ((U1STA & 0x000E) != 0x0000)
		{
	    dummy = U1RXREG; //dummy read to clear FERR/PERR
	    U1STAbits.OERR = 0; //clear OERR to keep receiving
		}
	//get the data
	if (U1STAbits.URXDA == 1)
		{
	    tempcar = U1RXREG; //get data from UART RX FIFO
	    * ptrChar = tempcar;
	    TESTPIN1_WRITE^=1;
		break;
		}
    }
}

//--------------------------------------------------------------------
// Function: 	void PutEIA232Char(BYTE txChar)
// Precondition: EIA232 Setup
// Input: 		Char - Character to transmit
// Output: 		None
// Side Effects:Puts character into destination pointed to by ptrChar.
// Overview: 	Transmits a character on UART1. (EIA232)
//	 			Waits for an empty spot in TXREG FIFO.
// Note:		 	None
//--------------------------------------------------------------------
void PTPutEIA232Char(BYTE txChar)
{
while (U1STAbits.UTXBF); //wait for FIFO space
U1TXREG = txChar; //put character onto UART FIFO to transmit
TESTPIN4_WRITE^=1;
}

//--------------------------------------------------------------------
// Function:        void PTGetEIA485Char(BYTE * ptrChar)
// PreCondition:    EIA485 Setup
// Input:			ptrChar - pointer to character received
// Output:			None
// Side Effects:	Puts character into destination pointed to by ptrChar.
//					Clear WDT
// Overview:		Receives a character from EIA485.  
// Note:			None
//--------------------------------------------------------------------
void PTGetEIA485Char(BYTE * ptrChar)
{

}

//--------------------------------------------------------------------
// Function:        void PTPutEIA485Char(BYTE txChar)
// PreCondition:    SPI Setup
// Input:			Char - Character to transmit
// Output:		
// Side Effects:	Puts character into destination pointed to by ptrChar.
//					Clear WDT
// Overview:		Transmits a character to EIA485.
//					Waits for an empty spot in EIA485 Tx Buffer.
// Note:			None
//--------------------------------------------------------------------
void PTPutEIA485Char(BYTE txChar)
{
while (U1STAbits.UTXBF); //wait for FIFO space
U2TXREG = txChar; //put character onto UART FIFO to transmit
}

//--------------------------------------------------------------------
// Function:    void ResetDevice(WORD addr);
// PreCondition:None.
// Input:		addr 	- 16-bit address to vector to
// Output:		None.
// Side Effects:None
// Overview:	used to vector to user code
// Note:		None
//--------------------------------------------------------------------
void PTResetDevice(WORD addr)
{
	asm("goto %0" : : "r"(addr));
}
