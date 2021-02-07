
//Includes --------------------------------------
#include "bootloader_test.h"
#include "TAI_FF_Versionner.h"

//-----------------------------------------------
#ifdef __BOOTLOADER
//Globals -------------------------------------------------------
WORD responseBytes; //number of bytes in command response
DWORD_VAL globalSourceAddr; //general purpose address variable
DWORD_VAL userReset; //user code reset vector
DWORD_VAL userTimeout; //bootloader entry timeout value
WORD userResetRead; //bool - for relocating user reset vector

//Variables for storing runaway code protection keys
DWORD_VAL delay;

//Transmit/Recieve Buffer
BYTE buffer[MAX_PACKET_SIZE + 1];
//---------------------------------------------

//Configuration bits ----------------------------------------
//
//	These configuration settings are not required for proper 
//	bootloader operation and can be modified as desired.  
//
//	Refer to AN1157 troubleshooting for configuration suggestions
//-----------------------------------------------------------

//--------------------------------------------------------------
void BLmain(void)
{
int state;

//Setup bootloader entry delay
globalSourceAddr.Val = DELAY_TIME_ADDR; //bootloader timer address
delay.Val = ReadLatch(globalSourceAddr.word.HW, globalSourceAddr.word.LW); //read BL timeout

//Setup user reset vector
globalSourceAddr.Val = USER_PROG_RESET;
userReset.Val = ReadLatch(globalSourceAddr.word.HW, globalSourceAddr.word.LW);

//Prevent bootloader lockout - if no user reset vector, reset to BL start
if (userReset.Val == 0xFFFFFF)
    userReset.Val = BOOT_ADDR_LOW;
	
userResetRead = 0;
TimerInit();
IoInit();
UartInit();


while (1)
	{
	GetCommand(); 					//Get full AN851 command from UART
	state=HandleCommand(); 				//Handle the command READ/WRITE/ERASE/VERIFY
	if(state)
		PutResponse(responseBytes); 	//Respond to sent command
	}
}

//--------------------------------------------------------------
//Timer2 Init
//--------------------------------------------------------------
void TimerInit(void)
{
T2CONbits.TON = 0;
T2CONbits.T32 = 1; // Setup Timer 2/3 as 32 bit timer incrementing every clock, (T3 config regs. slaved to T2)
IFS0bits.T3IF = 0; // Clear the Timer3 Interrupt Flag (because the T2-3 interrupt appears as the T3) 
IEC0bits.T3IE = 0; // Disable Timer3 Interrupt Service Routine (same)

//Enable timer if not in always-BL mode
/*
if ((delay.Val & 0x000000FF) != 0xFF)
	{
	//Convert seconds into timer count value 
	delay.Val = ((DWORD) (FCY)) * ((DWORD) (delay.v[0]));
	
	PR3 = delay.word.HW; //setup timer timeout value
	PR2 = delay.word.LW;
	
	TMR2 = 0;
	TMR3 = 0;
	T2CONbits.TON = 1; //enable timer
    }
*/
}

//------------------------------------------------------------------------
// INITIO
//------------------------------------------------------------------------
void IoInit(void) {
    AD1PCFGL = 0xFFFF; //TOUT LES PINS ANALOG SONT OFF
    TRISBbits.TRISB2 = 0;
    LATB = 0x0004;
}

//------------------------------------------------------------------------
//UART1 INIT
//------------------------------------------------------------------------
void UartInit(void)
{
//Clear the IOLOCK bit
__builtin_write_OSCCONL(OSCCON & 0xFFBF);

//INPUTS **********************
TRISFbits.TRISF5 = 1;
RPINR18bits.U1RXR=17; //U1RX = RP17

//OUTPUTS *********************
TRISGbits.TRISG8 = 0;
RPOR9bits.RP19R=3; //RP19(32) = U1TX   

//Lock the IOLOCK bit so that the IO is not accedentally changed.
__builtin_write_OSCCONL(OSCCON | 0x0040);

//SETUP UART COMMS: 
//No parity, 
//one stop bit, 
//polled
U1MODE=0;
U1BRG = BAUDRATEREG;
U1MODE=0x8008; //enable uart
U1STA = 0x0400; //Enable TX
}

//------------------------------------------------------------------------
// Function: 	void GetCommand()
// Precondition: UART Setup
// Input: 		None.
// Output:		None.
// Side Effects:	None.
// Overview: 	Polls the UART to recieve a complete AN851 command.
//			 	Fills buffer[1024] with recieved data.
//			
// Note:		 	None.
//------------------------------------------------------------------------
void GetCommand()
{
BYTE RXByte;
BYTE checksum;
WORD dataCount;
//int test;
while(1)
	{
	GetChar(&RXByte); //Get first STX
	if (RXByte == STX)
		{
		//T2CONbits.TON = 0; //disable timer - data received
		GetChar(&RXByte); //Read second byte
		if (RXByte == STX)
			{
           	//2 STX, beginning of data
			checksum = 0; //reset data and checksum
			dataCount = 0;
			while (dataCount <= MAX_PACKET_SIZE + 1)
				{ //maximum num bytes
                GetChar(&RXByte);
                switch (RXByte)
					{
					case STX: //Start over if STX
					    checksum = 0;
					    dataCount = 0;
					    break;
					
					case ETX: //End of packet if ETX
					    checksum = ~checksum + 1; //test checksum
					    Nop();
					    if (checksum == 0) return; //return if OK
					    dataCount = 0xFFFF; //otherwise restart
					    break;
					
					case DLE: //If DLE, treat next as data
					    GetChar(&RXByte);
					default: //get data, put in buffer
					    checksum += RXByte;
					    buffer[dataCount++] = RXByte;
					    break;
					}
				}
			}
		}
	}				
}

//------------------------------------------------------------------------
//  Function: 	void HandleCommand()
//  Precondition: data in buffer
//  Input: 		None.
//  Output:		None.
//  Side Effects:	None.
//  Overview: 	Handles commands received from host
//  Note:		 	None.
//------------------------------------------------------------------------
int HandleCommand()
{
BYTE Command;
BYTE length;

//variables used in EE and CONFIG read/writes

Command = buffer[0]; //get command from buffer
length = buffer[1]; //get data length from buffer

//--------------------------------------------------------RESET Command
if (length == 0x00)
	return 0;
//--------------------------------------------------------
//get 24-bit address from buffer
globalSourceAddr.v[0] = buffer[2];
globalSourceAddr.v[1] = buffer[3];
globalSourceAddr.v[2] = buffer[4];
globalSourceAddr.v[3] = 0;

//Handle Commands		
switch (Command)
	{
	case RD_VER: //Read version	
	    buffer[2] = MINOR_VERSION;
	    buffer[3] = MAJOR_VERSION;
	    responseBytes = 4; //set length of reply
	    break;
	
	case RD_FLASH: //Read flash memory
	    ReadPM(length, globalSourceAddr);
	    responseBytes = length * PM_INSTR_SIZE + 5; //set length of reply    									  
	    break;
	
	case WT_FLASH: //Write flash memory
	    WritePM(length, globalSourceAddr);
	    responseBytes = 1; //set length of reply
	    break;
	
	case ER_FLASH: //Erase flash memory
	    ErasePM(length, globalSourceAddr);
	    responseBytes = 1; //set length of reply
	    break;
	
	case VERIFY_OK:
	    WriteTimeout();
	    responseBytes = 1; //set length of reply
	    break;
	
	default:return 0;break;
    }
return 1;
}

//------------------------------------------------------------------------
// Function: 	void PutResponse()
// Precondition: UART Setup, data in buffer
//  Input: 		None.
// Output:		None.
// Side Effects:	None.
// Overview: 	Transmits responseBytes bytes of data from buffer 
//              with UART as a response to received command.
// Note:		 	None.
//------------------------------------------------------------------------
void PutResponse(WORD responseLen) {

WORD i;
BYTE data;
BYTE checksum;

U1STAbits.UTXEN = 1; //make sure TX is enabled

PutChar(STX); //Put 2 STX characters
PutChar(STX);

//Output buffer as response packet
checksum = 0;
for (i = 0; i < responseLen; i++)
	{
    asm("clrwdt"); //looping code so clear WDT

    data = buffer[i]; //get data from response buffer
    checksum += data; //accumulate checksum

    //if control character, stuff DLE
    if (data == STX || data == ETX || data == DLE)
		PutChar(DLE);
    
    PutChar(data); //send data
	}

checksum = ~checksum + 1; //keep track of checksum

//if control character, stuff DLE
if (checksum == STX || checksum == ETX || checksum == DLE)
	PutChar(DLE);
    
PutChar(checksum); //put checksum
PutChar(ETX); //put End of text

while (!U1STAbits.TRMT); //wait for transmit to finish

}

/********************************************************************
 * Function: 	void PutChar(BYTE Char)
 *
 * Precondition: UART Setup
 *
 * Input: 		Char - Character to transmit
 *
 * Output: 		None
 *
 * Side Effects:	Puts character into destination pointed to by ptrChar.
 *
 * Overview: 	Transmits a character on UART2. 
 *	 			Waits for an empty spot in TXREG FIFO.
 *
 * Note:		 	None
 ********************************************************************/
void PutChar(BYTE txChar)
{
while (U1STAbits.UTXBF); //wait for FIFO space
U1TXREG = txChar; //put character onto UART FIFO to transmit
}

//--------------------------------------------------------------------
// Function:        void GetChar(BYTE * ptrChar)
// PreCondition:    UART Setup
// Input:			ptrChar - pointer to character received
// Output:		
// Side Effects:	Puts character into destination pointed to by ptrChar.
//				Clear WDT
// Overview:		Receives a character from UART2.  
// Note:			None
//--------------------------------------------------------------------
void GetChar(BYTE * ptrChar) {
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
	    break;
		}
//		if timer expired, jump to user code
//      if (IFS0bits.T3IF == 1)
//      	ResetDevice(userReset.Val);
    }
}

/********************************************************************
 * Function:     void ReadPM(WORD length, DWORD_VAL sourceAddr)
 *
 * PreCondition: None
 *
 * Input:		length		- number of instructions to read
 *				sourceAddr 	- address to read from
 *
 * Output:		None
 *
 * Side Effects:	Puts read instructions into buffer.
 *
 * Overview:		Reads from program memory, stores data into buffer. 
 *
 * Note:			None
 ********************************************************************/
void ReadPM(WORD length, DWORD_VAL sourceAddr)
{
WORD bytesRead = 0;
DWORD_VAL temp;

//Read length instructions from flash
while (bytesRead < length * PM_INSTR_SIZE)
	{
    //read flash
    temp.Val = ReadLatch(sourceAddr.word.HW, sourceAddr.word.LW);

    buffer[bytesRead + 5] = temp.v[0]; //put read data onto 
    buffer[bytesRead + 6] = temp.v[1]; //response buffer
    buffer[bytesRead + 7] = temp.v[2];
    buffer[bytesRead + 8] = temp.v[3];

    //4 bytes per instruction: low word, high byte, phantom byte
    bytesRead += PM_INSTR_SIZE;

    sourceAddr.Val = sourceAddr.Val + 2; //increment addr by 2
	}
}

/********************************************************************
 * Function:     void WritePM(WORD length, DWORD_VAL sourceAddr)
 *
 * PreCondition: Page containing rows to write should be erased.
 *
 * Input:		length		- number of rows to write
 *				sourceAddr 	- row aligned address to write to
 *
 * Output:		None.
 *
 * Side Effects:	None.
 *
 * Overview:		Writes number of rows indicated from buffer into
 *				flash memory
 *
 * Note:			None
 ********************************************************************/
void WritePM(WORD length, DWORD_VAL sourceAddr)
{
WORD bytesWritten;
DWORD_VAL data;

bytesWritten = 0; //first 5 buffer locations are cmd,len,addr	

//write length rows to flash
while ((bytesWritten) < length * PM_ROW_SIZE)
	{
    asm("clrwdt");
	//get data to write from buffer
	data.v[0] = buffer[bytesWritten + 5];
	data.v[1] = buffer[bytesWritten + 6];
	data.v[2] = buffer[bytesWritten + 7];
	data.v[3] = buffer[bytesWritten + 8];
	
	//4 bytes per instruction: low word, high byte, phantom byte
	bytesWritten += PM_INSTR_SIZE;//++4
	//-----------------------------------------------------------------
	//Flash configuration word handling
	//Mask of bit 15 of CW1 to ensure it is programmed as 0
	//as noted in PIC24FJ datasheets
	if (sourceAddr.Val == CONFIG_END)
		data.Val &= 0x007FFF;
	//-----------------------------------------------------------------
	if (sourceAddr.Val == 0x0)
		{
	    userReset.Val = data.Val & 0xFFFF;
	    userResetRead = 1;
		}

	//-----------------------------------------------------------------
	//get user app reset vector	hi byte
	if (sourceAddr.Val == 0x2)
		{
	    userReset.Val |= ((DWORD) (data.Val & 0x00FF)) << 16;
	    userResetRead = 1;
		}
	
	//-----------------------------------------------------------------
	//put information from reset vector in user reset vector location
	if (sourceAddr.Val == USER_PROG_RESET)			//dans l'adress 0x100 du AIVT
		{
	    if (userResetRead) 							//has reset vector been grabbed from location 0x0?
	        data.Val = userReset.Val;				//if yes, use that reset vector
		else
	        userReset.Val = data.Val;	       		//if no, use the user's indicated reset vector
		}
	
	//-----------------------------------------------------------------
	//If address is delay timer location, store data and write empty word
	if (sourceAddr.Val == DELAY_TIME_ADDR)
		{
	    userTimeout.Val = data.Val;
	    data.Val = 0xFFFFFF;
		}
	//-----------------------------------------------------------------
	WriteLatch(sourceAddr.word.HW, sourceAddr.word.LW,data.word.HW, data.word.LW);	//write data into latches
	if ((bytesWritten % PM_ROW_SIZE) == 0)         									//write to flash memory if complete row is finished
		WriteMem(PM_ROW_WRITE);														//execute write sequence

	sourceAddr.Val = sourceAddr.Val + 2; //increment addr by 2
    }
}

/********************************************************************
 * Function:     void ErasePM(WORD length, DWORD_VAL sourceAddr)
 *
 * PreCondition: 
 *
 * Input:		length		- number of pages to erase
 *				sourceAddr 	- page aligned address to erase
 *
 * Output:		None.
 *
 * Side Effects:	None.
 *
 * Overview:		Erases number of pages from flash memory
 *
 * Note:			None
 ********************************************************************/
void ErasePM(WORD length, DWORD_VAL sourceAddr) {
    WORD i = 0;

    while (i < length)
		{
        i++;
       	//perform erase
        Erase(sourceAddr.word.HW, sourceAddr.word.LW, PM_PAGE_ERASE);
        sourceAddr.Val += PM_PAGE_SIZE / 2; //increment by a page
    }
}

/********************************************************************
 * Function:     void WriteTimeout()
 *
 * PreCondition: The programmed data should be verified prior to calling
 * 				this funtion.
 *
 * Input:		None.
 *
 * Output:		None.
 *
 * Side Effects:	None.
 *
 * Overview:		This function writes the stored value of the bootloader
 *				timeout delay to memory.  This function should only
 *				be called after sucessful verification of the programmed
 *				data to prevent possible bootloader lockout
 *
 * Note:			None
 ********************************************************************/
void WriteTimeout() {

    //Write timeout value to memory
    WriteLatch((DELAY_TIME_ADDR & 0xFF0000) >> 16,   //write data into latches
             (DELAY_TIME_ADDR & 0x00FFFF),
            userTimeout.word.HW, userTimeout.word.LW);

    //Perform write to enable BL timeout
    //execute write sequence
    WriteMem(PM_WORD_WRITE);
}

/********************************************************************
; Function: 	void WriteMem(WORD cmd)
;
; PreCondition: Appropriate data written to latches with WriteLatch
;
; Input:    	cmd - type of memory operation to perform
;                               
; Output:   	None.
;
; Side Effects: 
;
; Overview: 	Write stored registers to flash memory
;*********************************************************************/
void WriteMem(WORD cmd)
{
	NVMCON = cmd;
	__builtin_write_NVM();
	while(NVMCONbits.WR == 1);
}

/********************************************************************
; Function: 	void WriteLatch(WORD page, WORD addrLo, 
;				        		WORD dataHi, WORD dataLo)
;
; PreCondition: None.
;
; Input:    	page 	- upper byte of address
;				addrLo 	- lower word of address
;				dataHi 	- upper byte of data
;				addrLo	- lower word of data
;                               
; Output:   	None.
;
; Side Effects: TBLPAG changed
;
; Overview: 	Stores data to write in hardware latches
;*********************************************************************/	
void WriteLatch(WORD page, WORD addrLo, WORD dataHi, WORD dataLo)
{
	TBLPAG = page;
	__builtin_tblwtl(addrLo,dataLo);
	__builtin_tblwth(addrLo,dataHi);
	
}	

/********************************************************************
; Function: 	DWORD ReadLatch(WORD page, WORD addrLo)
;
; PreCondition: None.
;
; Input:    	page 	- upper byte of address
;				addrLo 	- lower word of address
;                               
; Output:   	data 	- 32-bit data in W1:W0
;
; Side Effects: TBLPAG changed
;
; Overview: 	Read from location in flash memory
;*********************************************************************/
DWORD ReadLatch(WORD page, WORD addrLo)
{
	DWORD_VAL temp;
	TBLPAG = page;
	temp.word.LW = __builtin_tblrdl(addrLo);
	temp.word.HW = __builtin_tblrdh(addrLo);
	return temp.Val;
}

/*********************************************************************
; Function: 	void ResetDevice(WORD addr);
;
; PreCondition: None.
;
; Input:    	addr 	- 16-bit address to vector to
;                               
; Output:   	None.
;
; Side Effects: None.
;
; Overview: 	used to vector to user code
;**********************************************************************/
void ResetDevice(WORD addr)
{
	asm("goto %0" : : "r"(addr));
}

/********************************************************************
; Function: 	void Erase(WORD page, WORD addrLo, WORD cmd);
;
; PreCondition: None.
;
; Input:    	page 	- upper byte of address
;				addrLo 	- lower word of address
;				cmd		- type of memory operation to perform
;                               
; Output:   	None.
;
; Side Effects: TBLPAG changed
;
; Overview: 	Erases page of flash memory at input address
*********************************************************************/	
void Erase(WORD page, WORD addrLo, WORD cmd)
{
WORD temp;	

temp = TBLPAG;
TBLPAG = page;
NVMCON = PM_PAGE_ERASE;//0x4042;

__builtin_tblwtl(addrLo,0x0000);
__builtin_write_NVM();

while(NVMCONbits.WR == 1);
TBLPAG = temp;
}
#endif
