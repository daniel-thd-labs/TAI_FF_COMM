
// Configuration for PIC24F Serial Bootloader

#ifndef CONFIG_H
#define CONFIG_H

//Includes-------------------------------------------------------------------------- 
#include <p24FJ256GB106.h>
#include "pic24fj256gb106_typeDefs.h"
//---------------------------------------------------------------------------------

//Configuration Defines 

//Bootloader device configuration
#define DEV_HAS_WORD_WRITE		//Device has word write capability (24FJxxx devices)
//#define DEV_HAS_USB			//Device is a USB capable device with a 96MHz PLL


//Bootloader feature configuration
#define USE_HI_SPEED_BRG	//Use BRGH=1, UART high speed mode
//#define USE_AES //Use encryption
//#define USE_RESET_SAVE    //Restores the reset vector without using USE_BOOT_PROTECT

//Bootloader Operation Configuration
#define MAJOR_VERSION		0x01	//Bootloader FW version
#define MINOR_VERSION		0x00

#define MAX_PACKET_SIZE		261	//Max packet size

//USER_PROG_RESET should be the location of a pointer to the start of user code, 
//not the location of the first instruction of the user application.
#define USER_PROG_RESET     0x100	//User app reset vector location
#define DELAY_TIME_ADDR 	0x102	//BL entry delay location

#define BOOT_ADDR_LOW 		0x400	//start of BL protection area
#ifndef USE_AES
	#define BOOT_ADDR_HI  		0xBFF	//end of BL protection area 
#else
 	#define BOOT_ADDR_HI  		0x13FF	//end of BL protection area ** USE 0x13FF for AES support
#endif

//If using encryption, set the AES encryption key
#ifdef USE_AES
	#define AES_KEY {0x0100,0x0302,0x0504,0x0706,0x0908,0x0B0A,0x0D0C,0x0F0E}
#endif

//If using RPx pins multiplexed with ANx functions, uncomment these lines to configure AD1PCFG
#define UTX_ANA		AD1PCFGbits.PCFG2
#define URX_ANA		AD1PCFGbits.PCFG4

//Define UART pins for PPS devices, by default Explorer 16 settings are used
#define PPS_UTX_PIN		RPOR9bits.RP19R			//UART TX pin sur pin RP19
#define PPS_URX_PIN 	32//10					//UART RX pin
#define PPS_URX_REG    	RPINR19bits.U2RXR

	
//Device Flash Memory Configuration
//Size of flash memory sections in bytes
//includes upper phantom byte (32-bit wide memory)
#define PM_INSTR_SIZE 		4		//bytes per instruction 
#define PM_ROW_SIZE 		256  	//user flash row size in bytes 
#define PM_PAGE_SIZE 		2048 	//user flash page size in bytes

//Vector section is either 0 to 0x200 or 0 to end of first page, whichever is larger
#define VECTOR_SECTION      ((0x200>(PM_PAGE_SIZE/2))?0x200:(PM_PAGE_SIZE/2)) 

#define CONFIG_START 		0x2ABFA	
#define CONFIG_END   		0x2ABFE

//Self-write NVMCON opcodes	
#define PM_PAGE_ERASE 		0x4042	//NVM page erase opcode
#define PM_ROW_WRITE 		0x4001	//NVM row write opcode

#ifdef DEV_HAS_WORD_WRITE
	#define PM_WORD_WRITE		0x4003	//NVM word write opcode
#endif

//------------------------------------------------------------------------
// UART Baud Rate Calculation and hi speed mode select
//------------------------------------------------------------------------
//an other way of saying it...regareding baud rate
#define XTFREQ          8000000        		//On-board FRC
#define PLLMODE         2               		//On-chip PLL setting
#define FCY2            XTFREQ*PLLMODE        //Instruction Cycle Frequency
#define BAUDRATE1MHZ    1000000
#define BRGVAL1MHZ     (FCY2/(4*BAUDRATE1MHZ))-1

#define UARTNUM     2       //Which device UART to use
#define FCY     16000000	//Instruction clock speed (Fosc/2)
#define BAUDRATE    1000000
#define BRG_DIV 4 //for HI speed...
#define BAUDRATEREG    (FCY/(4*BAUDRATE))-1
//#define BAUDRATEREG    ((FCY + (BRG_DIV/2*BAUDRATE))/BRG_DIV/BAUDRATE-1)
#define BAUD_ACTUAL    (FCY/BRG_DIV/(BAUDRATEREG+1))
#define BAUD_ERROR          ((BAUD_ACTUAL > BAUDRATE) ? BAUD_ACTUAL-BAUDRATE : BAUDRATE-BAUD_ACTUAL)
#define BAUD_ERROR_PRECENT  ((BAUD_ERROR*100+BAUDRATE/2)/BAUDRATE)
#if (BAUD_ERROR_PRECENT > 3)
    #error "UART frequency error is worse than 3%"
#elif (BAUD_ERROR_PRECENT > 2)
    #warning "UART frequency error is worse than 2%"
#endif

#endif
//**********************************************************************************

//Constant Defines *****************************************************************
//Protocol Commands
#define RD_VER 		0x00
#define RD_FLASH 	0x01
#define WT_FLASH	0x02
#define ER_FLASH	0x03
#define	RD_EEDATA	0x04
#define WT_EEDATA	0x05
#define RD_CONFIG	0x06
#define WT_CONFIG	0x07
#define VERIFY_OK	0x08
#define EXIT_PT		0x09
#define RESET		0x0a


//Communications Control bytes
#define STX 0x55
#define ETX 0x04
#define DLE 0x05
//**********************************************************************************

//**********************************************************************************

//UART Configuration ***************************************************************
#define UARTREG2(a,b)     U##a##b
#define UARTREG(a,b)    UARTREG2(a,b)
#define UxMODE      UARTREG(UARTNUM,MODE)
#define UxBRG       UARTREG(UARTNUM,BRG)
#define UxSTA       UARTREG(UARTNUM,STA)
#define UxRXREG     UARTREG(UARTNUM,RXREG)
#define UxTXREG     UARTREG(UARTNUM,TXREG)
#define UxMODEbits  UARTREG(UARTNUM,MODEbits)
#define UxSTAbits   UARTREG(UARTNUM,STAbits)
#define UxTX_IO UARTREG(UARTNUM,TX_IO)
//**********************************************************************************

//Function Prototypes **************************************************************


//**********************************************************************************

//#endif //ifdef CONFIG_H

int PTMain(void);

void PTStopServices(void);
void PTRestoreServices(void);

void PTResetIo(void);
void PTRestoreIoState(void);

void PTEIA485Init(void);
void PTEIA232StopInterrupts(void);
void PTEIA232StartInterrupts(void);
void PTTimerInit(void);
void PTResetDevice(WORD);

void PTGetEIA232Char(BYTE *);
void PTPutEIA232Char(BYTE txChar);
void PTGetEIA485Char(BYTE *);
void PTPutEIA485Char(BYTE txChar);

int PTGetCommand(void);
int PTHandleCommand(void);
int PTGetResponse(void);
int PTHandleResponse(void);
