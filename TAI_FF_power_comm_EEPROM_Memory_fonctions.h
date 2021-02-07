
// pour la version modifier pour oe Raspberry pi
// Il faut libéréer des pin pour la mémoire et le raspberry pi ainsi
// que librer les pine pour le SPI comm vers le master
/*
#define CS_EEPROM_DIR	TRISAbits.TRISA2
#define	SPI_CLOCK_DIR   TRISBbits.TRISB4
#define	SPI_MOSI_DIR	TRISBbits.TRISB5
#define	SPI_MISO_DIR	TRISBbits.TRISB8

#define CS_EEPROM 		PORTAbits.RA2		//En output
#define SPI_CLOCK 		PORTBbits.RB4
#define SPI_MOSI 		PORTBbits.RB5
#define SPI_MISO 		PORTBbits.RB8
#define PAGE_SIZE 16

// Version the summer

#define CS_EEPROM_DIR			TRISAbits.TRISA4	//En output
#define CS_EEPROM 				LATAbits.LATA4

#define	EEPROM_CLOCK_DIR   		TRISAbits.TRISA3	//En output
#define EEPROM_CLOCK 			PORTAbits.RA3

#define	EEPROM_DATA_OUT_DIR		TRISBbits.TRISB2	//pin 6 En output
#define EEPROM_DATA_OUT			PORTBbits.RB2

#define	EEPROM_DATA_IN_DIR		TRISBbits.TRISB4	//En input
#define EEPROM_DATA_IN 			PORTBbits.RB4
*/

#define PAGE_SIZE 160

void SPI_EEPROM_Comm_Init(void);

void Memory_Write_Single(unsigned long address,unsigned char Data);
unsigned char Memory_Read_Single(unsigned long address);

void Memory_Bank_Save(unsigned char preset_number);
void Memory_Bank_Load(unsigned char preset_number);

void Memory_Save_Last_Preset(unsigned char preset_number);
unsigned char Memory_Load_Last_Preset(void);

unsigned char Factory_Init_Read(void);
void Factory_Init_Set(void);
void Factory_Init_Clear(void);
void Factory_Reset(void);

unsigned char Memory_Read_BitBang_Data(unsigned long adresse);
unsigned char Memory_Write_BitBang_Data(unsigned long adresse,unsigned char data);
void Memory_Write_Enable_BitBang(void);
void Memory_Write_Desable_BitBang(void);
unsigned char Memory_Write_Status_BitBang(unsigned char status);
unsigned char Memory_Read_Status_BitBang(void);

//extern unsigned char global_error_flag;
extern void Delais(unsigned int tic_ms);
extern unsigned char Led_array[120];
extern unsigned char Led_string[15];
extern unsigned char Meter_array[2];
extern unsigned char Input_array[2][4];
extern unsigned char Gain_data;
extern unsigned char Hold_delais;
extern unsigned char multi_switch_cnt;
extern unsigned char Vu_reference,new_Vu_reference;
extern unsigned char Meter_source,new_Meter_source;
extern unsigned char Preset,new_Preset;
extern unsigned char Bus_gain,new_Bus_gain;
extern unsigned char Config_registers[128];
extern unsigned char Config_registers_index;
extern unsigned char Config_registers_data;

/***********************************************************************
*************************************************************************
|FONCTIONS:	unsigned char Write_cycle_End(void)							|
|Notes:	test la fin d'un cycle d'écriture								|
|																		|
|Arguments:	void														|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
unsigned char Write_cycle_End(void);

/******************************************************************
********************************************************************
|FONCTIONS:	void Memory_Write_Page_Continue(void)					|
|Notes:	Écrit les données de supplémentaire de la page commencée	|
|																	|
|Arguments:	Msg, data_lenght										|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
void Memory_Write_Page_Continue(unsigned char data);
/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_Write_Page_Start					|
|						(unsigned short adresse,unsigned char data)	|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	Msg, data_lenght										|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Write_Page_Start(	unsigned long adresse,unsigned char data);
/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_Read_Page_Continue(void)			|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Argumen	ts:	Msg, data_lenght									|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Read_Page_Continue(void);
/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_Read_Bank_Start(unsigned char data)|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Argumen	ts:	Msg, data_lenght									|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Read_Page_Start(unsigned short adresse);

/***********************************************************************
*************************************************************************
|FONCTIONS:	void Load_Config_from_EEprom(void)
										|
|Notes:	Envoie 1 message de longueur de 8bits							|
|																		|
|Arguments:	void														|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
void Load_Config_from_EEprom(void);
/***********************************************************************
*************************************************************************
|FONCTIONS:	void Save_Config(void)										|
|Notes:	Envoie 1 message de longueur de 8bits							|
|																		|
|Arguments:	void														|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
void Save_Config_To_EEprom(void);
