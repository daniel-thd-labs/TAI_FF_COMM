// Version FlyingFader
#include "p24FJ256GB106.h"
#include "TAI_FF_Versionner.h"

//-----------------------------------------------------------
//
//
//
//-----------------------------------------------------------
#ifdef TAI_SSL

	#ifdef PICPLUG_V2
		//------Mapping for PicPlug_V2 ------
		//------ (smd + eeprom on board) ------
		//-------- EEPROM HARDWARE MAPPING --------
		#define CS_EEPROM_DIR	 		TRISBbits.TRISB6 		//RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK_DIR		TRISBbits.TRISB8 		//RB8-RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT_DIR		TRISBbits.TRISB9 		//RB9-RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN_DIR		TRISBbits.TRISB7		 //RB7-RP7 EEPROM_SPI_MISO
		
		#define CS_EEPROM	 			LATBbits.LATB6 			//RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK			PORTBbits.RB8 			//RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT			PORTBbits.RB9 			//RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN			PORTBbits.RB7 			//RP7 EEPROM_SPI_MISO
		
		#define USBSENSE	PORTFbits.RF3
		
		//-------- SPI MASTER HARDWARE MAPPING --------
		#define  DATAOUT1_DIR		TRISDbits.TRISD10
		#define  DATAIN1_DIR		TRISDbits.TRISD8
		#define  CLOCK1_DIR    		TRISDbits.TRISD0
		
		#define SPI1_RX_PIN_SELECT	RPINR20bits.SDI1R = 2		//Data in du SPI1 sur RP2
		#define SPI1_TX_PIN_SELECT	RPOR1bits.RP3R = 7			//Data out du SPI1 sur RP3
		#define SPI_CLK_PIN_SELECT 	RPOR5bits.RP11R = 8			//clock du SPI1 sur pin RP12
		
		#define  CS1_DIR			TRISGbits.TRISG8
		#define  CS1				LATGbits.LATG8
		
		#define  CS2_DIR			TRISGbits.TRISG?
		#define  CS2				LATGbits.LATG6
		
		#define  CS3_DIR			TRISDbits.TRISD4
		#define  CS3				LATDbits.LATD4
		
		//------ MASTER FADER RELAIS ------
		#define MASTER_RELAIS 		LATFbits.LATF4
		#define MASTER_RELAIS_DIR 	TRISFbits.TRISF4
		
		//------ UART1/RASP HARDWARE MAPPING ------
		#define UART1_RX_PIN_DIR 		TRISFbits.TRISF5		//Pour la version 2 du pic plug
		#define UART1_RX_PIN_SELECT 	RPINR18bits.U1RXR=17	//Pour la version 2 du pic plug
		
		#define UART1_TX_PIN_DIR 		TRISDbits.TRISD9
		#define UART1_TX_PIN_SELECT  	RPOR2bits.RP4R=3
		
		//------- UART2/RS485 HARDWARE MAPPING -------
		#define UART2_RTS_DIR	TRISDbits.TRISD3				//Pour la version 2 du pic plug
		#define UART2_RTS		LATDbits.LATD3					//Pour la version 2 du pic plug
		
		#define UART2_RX_PIN_DIR TRISBbits.TRISB15				//Pour la version 2 du pic plug
		#define UART2_RX_PIN_SELECT RPINR19bits.U2RXR = 29		//Pour la version 2 du pic plug
		
		#define UART2_TX_PIN_DIR TRISBbits.TRISB14
		#define UART2_TX_PIN_SELECT RPOR7bits.RP14R=5
		
	#endif
#else
	#ifdef PICPLUG_V1c
		//Mapping for PicPlug_V1C
		//------ EEPROM HARDWARE MAPPING ------
		#define CS_EEPROM_DIR			TRISBbits.TRISB5	//En output
		#define CS_EEPROM 				LATBbits.LATB5
		
		#define	EEPROM_CLOCK_DIR   		TRISBbits.TRISB3	//En output
		#define EEPROM_CLOCK 			PORTBbits.RB3
		#define	EEPROM_DATA_IN_DIR		TRISBbits.TRISB4	//En input
		#define EEPROM_DATA_IN 			PORTBbits.RB4
		#define	EEPROM_DATA_OUT_DIR		TRISBbits.TRISB8		//pin 21 En output
		#define EEPROM_DATA_OUT			PORTBbits.RB8
		#define EEPROM_TX_PIN_DIR  		TRISBbits.TRISB6=0		//Pour la version 2 du pic plug
		#define EEPROM_TX_PIN_SELECT  	RPOR3bits.RP6R= 3		//Pour la version 2 du pic plug		
		
		#define USBSENSE	PORTFbits.RF3
		
		//------ SPI MASTER HARDWARE MAPPING ------
		#define  DATAOUT1_DIR		TRISDbits.TRISD10
		#define  DATAIN1_DIR		TRISDbits.TRISD2
		#define  CLOCK1_DIR    		TRISDbits.TRISD11
		
		#define SPI1_RX_PIN_SELECT	RPINR20bits.SDI1R = 23		//Data in du SPI1 sur RP23
		#define SPI1_TX_PIN_SELECT	RPOR1bits.RP3R = 7			//Data out du SPI1 sur RP3
		#define SPI_CLK_PIN_SELECT 	RPOR6bits.RP12R = 8			//clock du SPI1 sur pin RP12
		
		#define  CS1_DIR			TRISDbits.TRISD3
		#define  CS1				LATDbits.LATD3
		
		#define  CS2_DIR			TRISDbits.TRISD9
		#define  CS2				LATDbits.LATD9
		
		#define  CS3_DIR			TRISDbits.TRISD8
		#define  CS3				LATDbits.LATD8
		
		//------ MASTER FADER RELAIS ------
		#define MASTER_RELAIS 		LATDbits.LATD1
		#define MASTER_RELAIS_DIR 	TRISDbits.TRISD1
		
		//------ UART1/RASP HARDWARE MAPPING ------
		#define UART1_RX_PIN_DIR 		TRISFbits.TRISF5		//Pour la version 2 du pic plug
		#define UART1_RX_PIN_SELECT 	RPINR18bits.U1RXR=17	//Pour la version 2 du pic plug
		
		#define UART1_TX_PIN_DIR 		TRISGbits.TRISG8
		#define UART1_TX_PIN_SELECT  	RPOR9bits.RP19R=3
		
		//------- UART2/RS485 HARDWARE MAPPING -------
		#define UART2_RTS_DIR	TRISBbits.TRISB2		//Pour la version 2 du pic plug
		#define UART2_RTS		LATBbits.LATB2			//Pour la version 2 du pic plug
		
		#define UART2_RX_PIN_DIR TRISBbits.TRISB9		//Pour la version 2 du pic plug
		#define UART2_RX_PIN_SELECT RPINR19bits.U2RXR = 9		//Pour la version 2 du pic plug
		
		#define UART2_TX_PIN_DIR TRISGbits.TRISG7
		#define UART2_TX_PIN_SELECT RPOR13bits.RP26R=5
	
	#endif
#endif

//-----------------------------------------------------------
//-----------------------------------------------------------
//
//
//
//-----------------------------------------------------------
//-----------------------------------------------------------
#ifdef FLYINGFADER

	#ifdef PICPLUG_V2

		//------ Mapping for PicPlug_V2 ------
		//------ EEPROM HARDWARE MAPPING ------

		#define CS_EEPROM_DIR	 		TRISBbits.TRISB6 		//RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK_DIR		TRISBbits.TRISB8 		//RB8-RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT_DIR		TRISBbits.TRISB9 		//RB9-RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN_DIR		TRISBbits.TRISB7		 //RB7-RP7 EEPROM_SPI_MISO
		
		#define CS_EEPROM	 			LATBbits.LATB6 			//RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK			PORTBbits.RB8 			//RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT			PORTBbits.RB9 			//RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN			PORTBbits.RB7 			//RP7 EEPROM_SPI_MISO

		#define USBSENSE	PORTFbits.RF3
		
		//------ UART1/RASP HARDWARE MAPPING ------
		#define UART1_RX_PIN_DIR 		TRISFbits.TRISF5		//Pour la version 2 du pic plug
		#define UART1_RX_PIN_SELECT 	RPINR18bits.U1RXR=17	//Pour la version 2 du pic plug
		
		#define UART1_TX_PIN_DIR 		TRISDbits.TRISD9
		#define UART1_TX_PIN_SELECT  	RPOR2bits.RP4R=3
		
		//------- UART2/RS485 HARDWARE MAPPING -------
		#define UART2_RTS_DIR	TRISDbits.TRISD3				//Pour la version 2 du pic plug
		#define UART2_RTS		LATDbits.LATD3					//Pour la version 2 du pic plug
		
		#define UART2_RX_PIN_DIR TRISBbits.TRISB15				//Pour la version 2 du pic plug
		#define UART2_RX_PIN_SELECT RPINR19bits.U2RXR = 29		//Pour la version 2 du pic plug
		
		#define UART2_TX_PIN_DIR TRISBbits.TRISB14
		#define UART2_TX_PIN_SELECT RPOR7bits.RP14R=5
	
		//------- TEST PINS MAPPING -------
		#define TESTPIN1_DIR		TRISGbits.TRISG8	//Pin1
		#define TESTPIN2_DIR		TRISDbits.TRISD10	//Pin2
		#define TESTPIN3_DIR		TRISDbits.TRISD4	//Pin3
		
		#define TESTPIN1_WRITE		LATGbits.LATG8
		#define TESTPIN2_WRITE		LATDbits.LATD10
		#define TESTPIN3_WRITE		LATDbits.LATD4
		
		#define TESTPIN1_READ		PORTGbits.RG8
		#define TESTPIN2_READ		PORTDbits.RD10
		#define TESTPIN3_READ		PORTDbits.RD4
	
	#endif

	#ifdef PICPLUG_V1c

		//------ Mapping for PicPlug_V1C ------
		//------ EEPROM HARDWARE MAPPING ------

		#define CS_EEPROM_DIR			TRISBbits.TRISB5	//En output
		#define CS_EEPROM 				LATBbits.LATB5
		#define	EEPROM_CLOCK_DIR   		TRISBbits.TRISB3	//En output
		#define EEPROM_CLOCK 			PORTBbits.RB3
		#define	EEPROM_DATA_IN_DIR		TRISBbits.TRISB4	//En input
		#define EEPROM_DATA_IN 			PORTBbits.RB4
		#define	EEPROM_DATA_OUT_DIR		TRISBbits.TRISB8		//pin 21 En output
		#define EEPROM_DATA_OUT			PORTBbits.RB8
		#define EEPROM_TX_PIN_DIR  		TRISBbits.TRISB6=0		//Pour la version 2 du pic plug
		#define EEPROM_TX_PIN_SELECT  	RPOR3bits.RP6R= 3		//Pour la version 2 du pic plug		
		
		#define USBSENSE	PORTFbits.RF3
		
		//------ UART1/RASP HARDWARE MAPPING ------
		
		#define UART1_RX_PIN_DIR 		TRISFbits.TRISF5		//Pour la version 2 du pic plug
		#define UART1_RX_PIN_SELECT 	RPINR18bits.U1RXR=17	//Pour la version 2 du pic plug
		
		#define UART1_TX_PIN_DIR 		TRISGbits.TRISG8
		#define UART1_TX_PIN_SELECT  	RPOR9bits.RP19R=3
		
		//------- UART2/RS485 HARDWARE MAPPING -------
		#define UART2_RTS_DIR	TRISBbits.TRISB2		//Pour la version 2 du pic plug
		#define UART2_RTS		LATBbits.LATB2			//Pour la version 2 du pic plug
		
		#define UART2_RX_PIN_DIR TRISBbits.TRISB9		//Pour la version 2 du pic plug
		#define UART2_RX_PIN_SELECT RPINR19bits.U2RXR = 9		//Pour la version 2 du pic plug
		
		#define UART2_TX_PIN_DIR TRISGbits.TRISG7
		#define UART2_TX_PIN_SELECT RPOR13bits.RP26R=5
		
		//------- TEST PINS MAPPING -------
		#define TESTPIN1_DIR		TRISDbits.TRISD11	//Pin1
		#define TESTPIN2_DIR		TRISDbits.TRISD3	//Pin2
		#define TESTPIN3_DIR		TRISDbits.TRISD10	//Pin3
		#define TESTPIN4_DIR		TRISDbits.TRISD2	//Pin4
		
		#define TESTPIN1_WRITE		LATDbits.LATD11
		#define TESTPIN2_WRITE		LATDbits.LATD3
		#define TESTPIN3_WRITE		LATDbits.LATD10
		#define TESTPIN4_WRITE		LATDbits.LATD2
		
		#define TESTPIN1_READ		PORTDbits.RD11
		#define TESTPIN2_READ		PORTDbits.RD3
		#define TESTPIN3_READ		PORTDbits.RD10
		#define TESTPIN4_READ		PORTDbits.RD2
	
	#endif

#endif

//------ Mapping for PicPlug_V1C ------
//------ EEPROM HARDWARE MAPPING ------
	#ifdef PICPLUG_V1c

	#define CS_EEPROM_DIR			TRISBbits.TRISB5	//En output
	#define CS_EEPROM 				LATBbits.LATB5
	#define	EEPROM_CLOCK_DIR   		TRISBbits.TRISB3	//En output
	#define EEPROM_CLOCK 			PORTBbits.RB3
	#define	EEPROM_DATA_IN_DIR		TRISBbits.TRISB4	//En input
	#define EEPROM_DATA_IN 			PORTBbits.RB4
	#define	EEPROM_DATA_OUT_DIR		TRISBbits.TRISB8		//pin 21 En output
	#define EEPROM_DATA_OUT			PORTBbits.RB8
	#define EEPROM_TX_PIN_DIR  		TRISBbits.TRISB6=0		//Pour la version 2 du pic plug
	#define EEPROM_TX_PIN_SELECT  	RPOR3bits.RP6R= 3		//Pour la version 2 du pic plug		
	
	#define USBSENSE	PORTFbits.RF3
	
	//------ UART1/RASP HARDWARE MAPPING ------
	
	#define UART1_RX_PIN_DIR 		TRISFbits.TRISF5		//Pour la version 2 du pic plug
	#define UART1_RX_PIN_SELECT 	RPINR18bits.U1RXR=17	//Pour la version 2 du pic plug
	
	#define UART1_TX_PIN_DIR 		TRISGbits.TRISG8
	#define UART1_TX_PIN_SELECT  	RPOR9bits.RP19R=3
	
	//------- UART2/RS485 HARDWARE MAPPING -------
	#define UART2_RTS_DIR	TRISBbits.TRISB2		//Pour la version 2 du pic plug
	#define UART2_RTS		LATBbits.LATB2			//Pour la version 2 du pic plug
	
	#define UART2_RX_PIN_DIR TRISBbits.TRISB9		//Pour la version 2 du pic plug
	#define UART2_RX_PIN_SELECT RPINR19bits.U2RXR = 9		//Pour la version 2 du pic plug
	
	#define UART2_TX_PIN_DIR TRISGbits.TRISG7
	#define UART2_TX_PIN_SELECT RPOR13bits.RP26R=5
	
	//------- TEST PINS MAPPING -------
	#define TESTPIN1_DIR		TRISDbits.TRISD11	//Pin1
	#define TESTPIN2_DIR		TRISDbits.TRISD3	//Pin2
	#define TESTPIN3_DIR		TRISDbits.TRISD10	//Pin3
	#define TESTPIN4_DIR		TRISDbits.TRISD2	//Pin4
	
	#define TESTPIN1_WRITE		LATDbits.LATD11
	#define TESTPIN2_WRITE		LATDbits.LATD3
	#define TESTPIN3_WRITE		LATDbits.LATD10
	#define TESTPIN4_WRITE		LATDbits.LATD2
	
	#define TESTPIN1_READ		PORTDbits.RD11
	#define TESTPIN2_READ		PORTDbits.RD3
	#define TESTPIN3_READ		PORTDbits.RD10
	#define TESTPIN4_READ		PORTDbits.RD2

#endif


//-----------------------------------------------------------
//V2 picplug mapping
//-----------------------------------------------
//PICPLUG V2 MAPPING FOR FPMB_COMM_OEM
//-----------------------------------------------
#ifdef __FPMBOEM

	#ifdef PICPLUG_V2

		//------- USB PIN -------
		#define USBSENSE	PORTFbits.RF3
	
		//------- UART1/RASP HARDWARE MAPPING for compatibility -------
		#define UART1_RX_PIN_DIR 		TRISFbits.TRISF4		//Pour la version 2 du pic plug pin 31
		#define UART1_RX_PIN_SELECT 	RPINR18bits.U1RXR=10	//RF4/RP10
	
		#define UART1_TX_PIN_DIR 		TRISFbits.TRISF5		//Pour la version 2 du pic plug pin 32
		#define UART1_TX_PIN_SELECT  	RPOR8bits.RP17R=3		//RF5/RP17
	
		//------- UART2/EIA485 HARDWARE MAPPING -------
		//UART1 I/O's
		#define UART2_RTS_DIR		TRISDbits.TRISD9
		#define UART2_RTS			LATDbits.LATD9
		#define UART2_RTS_STATE		PORTDbits.RD9
		#define UART2_RX_PIN_DIR	TRISDbits.TRISD11
		#define UART2_RX			PORTDbits.RD11
		#define UART2_TX_PIN_DIR	TRISDbits.TRISD10
		#define UART2_TX			LATDbits.LATD10
		
		#define UART2_RX_PIN_SELECT RPINR19bits.U2RXR = 12			// Make RP12 U1RX pin45 Rx
		#define UART2_TX_PIN_SELECT RPOR1bits.RP3R = 5				// Make RP3 U1TX pin44 Tx
		
		//----- SPI MASTER TO FADER CONTROLLER HARDWARE MAPPING -------
		//------- SPI1 I/O's -------
		#define FADER_SPI1_CLOCK_DIR 	TRISGbits.TRISG8			//FADER SPI1 CLOCK ON CONN.PIN 22 (PIC #6/RG8/RP19) 	
		#define FADER_SPI1_MOSI_DIR 	TRISGbits.TRISG9			//FADER SPI1 OUT ON CONN. PIN 23 (PIC #8/RG9/RP27)  
		#define FADER_SPI1_MISO_DIR 	TRISBbits.TRISB5			//ENABLE RB5 AS INPUT FOR SPI1 IN ON CONN. PIN 26 (Pic PIN #11/RB5/RP18)
	
		#define FADER_SPI1_CLOCK 		LATGbits.LATG8			//FADER SPI1 CLOCK ON CONN.PIN 22 (PIC #6/RG8/RP19) 	18
		#define FADER_SPI1_MOSI 		LATGbits.LATG9			//FADER SPI1 OUT ON CONN. PIN 23 (PIC #8/RG9/RP27)  
		#define FADER_SPI1_MISO 		PORTBbits.RB5			//ENABLE RB5 AS INPUT FOR SPI1 IN ON CONN. PIN 26 (Pic PIN #11/RB5/RP18)19
	
		//------- Assign SPI1 RP PIN ------- 
		#define ASSIGN_FADER_SPI1_CLOCK	RPOR9bits.RP19R = 8		//RP19<=>
		#define ASSIGN_FADER_SPI1_MOSI	RPOR13bits.RP27R = 7	//RP27<=>SD01 =7
		#define ASSIGN_FADER_SPI1_MISO	RPINR20bits.SDI1R=	18	//FADER SPI1 IN on RP18 (PIC PIN#11)
	
		//------- SPI Fader Selects -------
		#define faderSelect1_DIR	TRISGbits.TRISG7
		#define faderSelect2_DIR	TRISGbits.TRISG6
		#define faderSelect3_DIR	TRISDbits.TRISD5
		#define faderSelect4_DIR	TRISDbits.TRISD4
		#define faderSelect5_DIR	TRISDbits.TRISD3
		#define faderSelect6_DIR	TRISDbits.TRISD2
		#define faderSelect7_DIR	TRISDbits.TRISD1
		#define faderSelect8_DIR	TRISDbits.TRISD0
	
		#define faderSelect1		LATGbits.LATG7
		#define faderSelect2		LATGbits.LATG6
		#define faderSelect3		LATDbits.LATD5
		#define faderSelect4		LATDbits.LATD4
		#define faderSelect5		LATDbits.LATD3
		#define faderSelect6		LATDbits.LATD2
		#define faderSelect7		LATDbits.LATD1
		#define faderSelect8		LATDbits.LATD0
	
		//------- SPI2 I/O's TO EEPROM -------
		#define CS_EEPROM_DIR	 		TRISBbits.TRISB6 //RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK_DIR		TRISBbits.TRISB8 //RB8-RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT_DIR		TRISBbits.TRISB9 //RB9-RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN_DIR		TRISBbits.TRISB7 //RB7-RP7 EEPROM_SPI_MISO
		
		#define CS_EEPROM	 			LATBbits.LATB6 //RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK			LATBbits.LATB8 //RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT			LATBbits.LATB9 //RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN			PORTBbits.RB7 //RP7 EEPROM_SPI_MISO
	
	
		#define Ext_Data_CS_DIR			TRISBbits.TRISB10
		#define setExtDataCS			LATBbits.LATB10		//AN10(also test pin)
	
		//------- Ext Comm I/O's -------
		#define expPortPin5_DIR			TRISBbits.TRISB14
		#define expPortPin6_DIR			TRISBbits.TRISB15
		#define expPortPin7_DIR			TRISFbits.TRISF4	//used for RASP RX RS232
		#define expPortPin8_DIR			TRISFbits.TRISF5	//used for RASP TX RS232
		#define expPortPin9_DIR			TRISDbits.TRISD8
		#define expPortPin10_DIR		TRISBbits.TRISB11	///RB11/AN11
	
		#define getExpPortPin5			PORTBbits.RB14		//RP14(also test pin)
		#define getExpPortPin6			PORTBbits.RB15		//RP29
		#define getExpPortPin7			PORTFbits.RF4		//RP10(used for RASP RX RS232)
		#define getExpPortPin8			PORTFbits.RF5		//RP17(used for RASP TX RS232)
		#define getExpPortPin9			PORTDbits.RD8		//RP2
		#define getExpPortPin10			PORTBbits.RB11		//AN11
	
		#define setExpPortPin5			LATBbits.LATB14		//RP14(also test pin)
		#define setExpPortPin6			LATBbits.LATB15		//RP29
		#define setExpPortPin7			LATFbits.LATF4		//RP10(used for RASP RX RS232)
		#define setExpPortPin8			LATFbits.LATF5		//RP17(used for RASP TX RS232)
		#define setExpPortPin9			LATDbits.LATD8		//RP2
		#define setExpPortPin10			LATBbits.LATB11		//AN11
	
		#define PULSEDIR TRISBbits.TRISB11
		#define PULSE LATBbits.LATB11
	
		#define TESTPIN1DIR	TRISBbits.TRISB14
		#define GETESTPIN1	PORTBbits.RB14
		#define SETESTPIN1	LATBbits.LATB14
	
	#endif
#endif


//-----------------------------------------------------------
//V2 picplug mapping
//-----------------------------------------------
//PICPLUG V2 MAPPING FOR FPMB_COMM_OEM
//-----------------------------------------------
#ifdef FPMBSSL

	#ifdef PICPLUG_V2
		//---------- USB PIN ----------
		#define USBSENSE	PORTFbits.RF3
	
		//---------- UART1/RASP HARDWARE MAPPING for compatibility --------
		#define UART1_RX_PIN_DIR 		TRISFbits.TRISF4		//Pour la version 2 du pic plug pin 31
		#define UART1_RX_PIN_SELECT 	RPINR18bits.U1RXR=10	//RF4/RP10
	
		#define UART1_TX_PIN_DIR 		TRISFbits.TRISF5		//Pour la version 2 du pic plug pin 32
		#define UART1_TX_PIN_SELECT  	RPOR8bits.RP17R=3		//RF5/RP17
	
		//----- UART1/EIA485 HARDWARE MAPPING -----
		//UART1 I/O's
		#define UART2_RTS_DIR		TRISDbits.TRISD9
		#define UART2_RTS			LATDbits.LATD9
		#define UART2_RX_PIN_DIR	TRISDbits.TRISD11
		#define UART2_RX			LATDbits.LATD11
		#define UART2_TX_PIN_DIR	TRISDbits.TRISD10
		#define UART2_TX			LATDbits.LATD10
		
		#define UART2_RX_PIN_SELECT RPINR19bits.U2RXR = 12			// Make RP12 U1RX pin45 Rx
		#define UART2_TX_PIN_SELECT RPOR1bits.RP3R = 5				// Make RP3 U1TX pin44 Tx
	
	
		//------- SPI MASTER TO FADER CONTROLLER HARDWARE MAPPING -------
		//------- SPI1 I/O's -------
		#define FADER_SPI1_ClOCK_DIR 	TRISGbits.TRISG8			//FADER SPI1 CLOCK ON CONN.PIN 22 (PIC #6/RG8) 	
		#define FADER_SPI1_MOSI_DIR 	TRISGbits.TRISG9			//FADER SPI1 OUT ON CONN. PIN 23 (PIC #8/RG9)  
		#define FADER_SPI1_MISO_DIR 	TRISBbits.TRISB5			//ENABLE RB5 AS INPUT FOR SPI1 IN ON CONN. PIN 26 (Pic PIN #11/RB5/RP18)
	
		//------- Assign SPI1 RP PIN -------
		#define ASSIGN_FADER_SPI1_ClOCK	RPOR9bits.RP19R = 8		//RP19<=>
		#define ASSIGN_FADER_SPI1_MOSI	RPOR13bits.RP27R = 7	//RP27<=>SD01 =7
		#define ASSIGN_FADER_SPI1_MISO	RPINR20bits.SDI1R=	18	//FADER SPI1 IN on RP18 (PIC PIN#11)
	
		//------SPI Fader Selects ------
		#define faderSelect1_DIR	TRISGbits.TRISG7
		#define faderSelect2_DIR	TRISGbits.TRISG6
		#define faderSelect3_DIR	TRISDbits.TRISD5
		#define faderSelect4_DIR	TRISDbits.TRISD4
		#define faderSelect5_DIR	TRISDbits.TRISD3
		#define faderSelect6_DIR	TRISDbits.TRISD2
		#define faderSelect7_DIR	TRISDbits.TRISD1
		#define faderSelect8_DIR	TRISDbits.TRISD0
	
		#define faderSelect1		LATGbits.LATG7
		#define faderSelect2		LATGbits.LATG6
		#define faderSelect3		LATDbits.LATD5
		#define faderSelect4		LATDbits.LATD4
		#define faderSelect5		LATDbits.LATD3
		#define faderSelect6		LATDbits.LATD2
		#define faderSelect7		LATDbits.LATD1
		#define faderSelect8		LATDbits.LATD0
	
		//------- SPI2 I/O's TO EEPROM -------
		#define CS_EEPROM_DIR	 		TRISBbits.TRISB6 //RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK_DIR		TRISBbits.TRISB8 //RB8-RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT_DIR		TRISBbits.TRISB9 //RB9-RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN_DIR		TRISBbits.TRISB7 //RB7-RP7 EEPROM_SPI_MISO
		
		#define CS_EEPROM	 			LATBbits.LATB6 //RB6-RP6 EEPROM_CS
		#define EEPROM_CLOCK			PORTBbits.RB8 //RP8 EEPROM_SPI_CLK
		#define EEPROM_DATA_OUT			PORTBbits.RB9 //RP9 EEPROM_SPI_MOSI
		#define EEPROM_DATA_IN			PORTBbits.RB7 //RP7 EEPROM_SPI_MISO
	
		// if using RP feature, Assign SPI2 RP PIN 
		//#define ASSIGN_EEPROM_SPI2_ClOCK	RPOR4bits.RP8R = 11		//RP8<=>SCK2OUT Pin 21
		//#define ASSIGN_EEPROM_SPI2_MOSI		RPOR4bits.RP9R = 10		//RP9<=>SD02(RPOR7)
		//#define ASSIGN_EEPROM_SPI2_MISO		RPINR22bits.SDI2R=	7	//FADER SPI1 IN on RP9 (PIC PIN#11)
	
		//#define EEPROM_TX_PIN_DIR			TRISBbits.TRISB9=0
		//#define EEPROM_TX_PIN_SELECT		RPOR4bits.RP9R = 10
	
		//------Ext Comm I/O's ------
		#define expPortPin5_DIR			TRISBbits.TRISB14
		#define expPortPin6_DIR			TRISBbits.TRISB15
		#define expPortPin7_DIR			TRISFbits.TRISF4	//used for RASP RX RS232
		#define expPortPin8_DIR			TRISFbits.TRISF5	//used for RASP TX RS232
		#define expPortPin9_DIR			TRISDbits.TRISD8
		#define expPortPin10_DIR		TRISBbits.TRISB11	///RB11/AN11
	
		#define expPortPin5				LATBbits.LATB14		//RP14(also test pin)
		#define expPortPin6				LATBbits.LATB15		//RP29
		#define expPortPin7				LATFbits.LATF4		//RP10(used for RASP RX RS232)
		#define expPortPin8				LATFbits.LATF5		//RP17(used for RASP TX RS232)
		#define expPortPin9				LATDbits.LATD8		//RP2
		#define expPortPin10			LATBbits.LATB11		//AN11
	
		#define setExpPortPin5			LATBbits.LATB14		//RP14(also test pin)
		#define setExpPortPin6			LATBbits.LATB15		//RP29
		#define setExpPortPin7			LATFbits.LATF4		//RP10(used for RASP RX RS232)
		#define setExpPortPin8			LATFbits.LATF5		//RP17(used for RASP TX RS232)
		#define setExpPortPin9			LATDbits.LATD8		//RP2
		#define setExpPortPin10			LATBbits.LATB11		//AN11
	
		#define PULSE PORTBbits.RB11
		#define PULSEDIR TRISBbits.TRISB11
	
		#define TESTPIN1DIR	TRISBbits.TRISB14
		#define TESTPIN1	PORTBbits.RB14
		#define GETESTPIN1	PORTBbits.RB14
		#define SETTESTPIN1	LATBbits.LATB14
	#endif

#endif
//-----------------------------------------------------------


