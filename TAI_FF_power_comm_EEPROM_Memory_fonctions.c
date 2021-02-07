#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_power_comm_EEPROM_Memory_fonctions.h"

#define TIME 200
/****************************************************************
*****************************************************************
|FONCTIONS:	void SPI_EEPROM_Memory_Init(void)
|
|
|Notes:	gestion des switch
|Arguments d'entrée:
|variable de sortie:	
|variables local:
|variables globales:
|
*****************************************************************
****************************************************************/
void SPI_EEPROM_Comm_Init(void)
{
unsigned char data;
CS_EEPROM_DIR=0;
CS_EEPROM=1;
CS_EEPROM=0;
CS_EEPROM=1;

EEPROM_DATA_OUT_DIR =0;			//RB2/ SPI MOSI/MEMORY_DATAOUT_DIR
EEPROM_DATA_IN_DIR =1;			//RB4/ pour SPI MISO/MEMORY DATAIN_DIR
EEPROM_CLOCK_DIR =0;			//RB9/ EEPROM_CLOCK en output
EEPROM_CLOCK =1;
EEPROM_DATA_OUT=1;
Memory_Write_Status_BitBang(0x00);
data=Memory_Read_Status_BitBang();
}

/***********************************************************************
*************************************************************************
|FONCTIONS:	void Save_Config_To_EEprom(void)							|
|Notes:	Envoie 1 message de longueur de 8bits							|
|																		|
|Arguments:	void														|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
void Save_Config_To_EEprom(void)
{
unsigned char data_n;
for(data_n=1;data_n<32;data_n++)
	Memory_Write_Single(data_n,Config_registers[data_n]);

}

/***********************************************************************
*************************************************************************
|FONCTIONS:	void Load_Config_EEprom(void)										|
|Notes:	Envoie 1 message de longueur de 8bits							|
|																		|
|Arguments:	void														|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
void Load_Config_from_EEprom(void)
{
unsigned char data_n;
for(data_n=1;data_n<32;data_n++)
	Config_registers[data_n]=Memory_Read_Single(data_n);
}

/***********************************************************************
*************************************************************************
|FONCTIONS:	void Factory_Init_Clear(void)								|
|Notes:	Envoie 1 message de longueur de 8bits							|
|																		|
|Arguments:	void														|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
void Factory_Init_Clear(void)
{
Memory_Write_Enable_BitBang();
CS_EEPROM=0;
Memory_Write_BitBang_Data(0,0);
CS_EEPROM=1;
Write_cycle_End();
}

/***********************************************************************
*************************************************************************
|FONCTIONS:	void Factory_Init_Set(void)									|
|Notes:	Envoie 1 message de longueur de 8bits							|
|																		|
|Arguments:	void														|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
void Factory_Init_Set(void)
{
Memory_Write_Enable_BitBang();
CS_EEPROM=0;
Memory_Write_BitBang_Data(0,0x01);
CS_EEPROM=1;
Write_cycle_End();
}

/***********************************************************************
*************************************************************************
|FONCTIONS:	void Factory_Init_Read(unsigned char preset_number)			|
|Notes:	Envoie 1 message de longueur de 8bits							|
|																		|
|Arguments:	preset_number												|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
unsigned char Factory_Init_Read(void)
{
unsigned char data,c;
CS_EEPROM=0;
data=Memory_Read_BitBang_Data(0);
CS_EEPROM=1;
Delais(5);
c=0;
return data;
}

/******************************************************************************
*********************************************************************************
|FONCTIONS:	void Memory_Write_Single(unsigned short address,unsigned char Data)	|
|Notes:	Envoie 1 message de longueur de 8bits									|
|																				|
|Arguments:	preset_number														|
|Variables locals:Data_Lenght;													|
|																				|
|Variables globals:																|
|																				|
*********************************************************************************
*******************************************************************************/
void Memory_Write_Single(unsigned long address,unsigned char Data)
{
Memory_Write_Enable_BitBang();
CS_EEPROM=0;
Memory_Write_BitBang_Data(address,Data);
CS_EEPROM=1;
Delais(1);
Write_cycle_End();
}

/***********************************************************************
*************************************************************************
|FONCTIONS:	unsigned char Memory_Read_Single(unsigned short Address);	|
|Notes:	Recupere 1 donnéede EEPROM a l'address x						|
|																		|
|Arguments:	preset_number												|
|Variables locals:Data_Lenght;											|
|																		|
|Variables globals:														|
|																		|
*************************************************************************
***********************************************************************/
unsigned char Memory_Read_Single(unsigned long address)
{
unsigned char data;
Delais(1);
CS_EEPROM=0;
data=Memory_Read_BitBang_Data(address);
CS_EEPROM=1;
Delais(2);
return data;
}

/*******************************************************************************
*********************************************************************************
|FONCTIONS:	void Memory_Save_Last_Preset_Number(unsigned char preset_number)	|
|Notes:	Envoie 1 message de longueur de 8bits									|
|																				|
|Arguments:	preset_number														|	
|Variables locals:Data_Lenght;													|
|																				|
|Variables globals:																|
|																				|
********************************************************************************
*******************************************************************************/
void Memory_Save_Last_Preset_Number(unsigned char preset_number)
{
Memory_Write_Enable_BitBang();
CS_EEPROM=0;
Memory_Write_BitBang_Data(1,preset_number);
CS_EEPROM=1;
Write_cycle_End();	//Attend 6 ms pour le temps d'écriture du eeprom
}
/*******************************************************************************
*********************************************************************************
|FONCTIONS:	void Memory_Load_Last_Preset_Number(unsigned char preset_number)	|
|Notes:	Envoie 1 message de longueur de 8bits									|
|																				|
|Arguments:	preset_number														|
|Variables locals:Data_Lenght;													|
|																				|
|Variables globals:																|
|																				|
*********************************************************************************
*******************************************************************************/
unsigned char Memory_Load_Last_Preset_Number(void)
{
unsigned char Data;
Delais(1);
CS_EEPROM=0;
Data=Memory_Read_BitBang_Data(1);
CS_EEPROM=1;
Delais(5);
return Data;
}

/******************************************************************
********************************************************************
|FONCTIONS:	void Memory_Save_Bank(unsigned char preset_number)		|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	preset_number											|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
void Memory_Save_Bank(unsigned char preset_number)
{
/*
unsigned char n;
n=0;
Memory_Write_Single(((preset_number*PAGE_SIZE))+n++,new_Vu_reference);
*/
}

/******************************************************************
********************************************************************
|FONCTIONS:	void Memory_Load_Bank(unsigned char preset_number)		|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	preset_number											|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
void Memory_Load_Bank(unsigned char preset_number)
{
/*
unsigned char m,n;

for(m=0;m<120;m++)
	{	
	Led_array[m]=Memory_Read_Single(((preset_number)*PAGE_SIZE)+n++);
	}
*/
}

/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_BitBang_Data(unsigned char data)	|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	Msg, data_lenght										|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
void Memory_Write_Enable_BitBang(void)
{	
unsigned char shift_8bit;
unsigned char Data_Lenght;
#define ENABLE 	0b00000110
Delais(1);
CS_EEPROM=0;
shift_8bit =0b10000000;
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (ENABLE&shift_8bit)		//Ajuste SPI_data
		EEPROM_DATA_OUT	 = 1;
	else
		EEPROM_DATA_OUT	 = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(1);
	Data_Lenght--;
	}
CS_EEPROM=1;
Delais(1);
}

/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_BitBang_Data(unsigned char data)	|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	Msg, data_lenght										|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
void Memory_Write_Desable_BitBang(void)
{	
#define DESABLE 	0b00000100

unsigned char shift_8bit;
unsigned char Data_Lenght;

Delais(1);
CS_EEPROM=0;
shift_8bit =0b10000000;
//transmetre et lire le premier octet TX-RX
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (DESABLE&shift_8bit)		//Ajuste SPI_data
		EEPROM_DATA_OUT	 = 1;
	else
		EEPROM_DATA_OUT	 = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
CS_EEPROM=1;
Delais(1);
}

/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_Write_Status_BitBang(unsigned char status)			|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	 														|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Write_Status_BitBang(unsigned char status)
{	
#define WRITESTATUS 	0b00000001
unsigned char Data_val;
unsigned char Data_Lenght,shift_8bit;

Delais(1);
CS_EEPROM=0;
Data_val = 0;
shift_8bit =0b10000000;
//transmetre et lire le premier octet TX-RX
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (WRITESTATUS&shift_8bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	Delais(1);
	shift_8bit = shift_8bit>>1;
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
/**********************************************************/
//Lire les data de RX
/**********************************************************/
Data_Lenght = 8;

while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (status&shift_8bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	Delais(1);
	shift_8bit = shift_8bit>>1;
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
CS_EEPROM=1;
Delais(1);
return Data_val;
}

/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_Read_Status_BitBang(void)			|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	 														|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Read_Status_BitBang(void)
{	
#define READSTATUS 	0b00000101
unsigned char Data_val;
unsigned char Data_Lenght,shift_8bit;

Delais(1);
CS_EEPROM=0;
Data_val = 0;
shift_8bit =0b10000000;
//transmetre et lire le premier octet TX-RX
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (READSTATUS&shift_8bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	Delais(1);
	shift_8bit = shift_8bit>>1;
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
/**********************************************************/
//Lire les data de RX
/**********************************************************/
Data_Lenght = 8;

while(Data_Lenght)
	{	
	EEPROM_CLOCK= 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if(EEPROM_DATA_IN)
		Data_val=Data_val|0x01;
	else
		Data_val=Data_val&0xFE; 
	EEPROM_CLOCK = 1;				//sample le data (SPI_CLK = 1)
	if(Data_Lenght>1)
		Data_val=Data_val<<1;	
	Delais(2);
	Data_Lenght--;
	}
CS_EEPROM=1;
Delais(1);
return Data_val;
}

/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_BitBang_Data(unsigned char data)	|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Arguments:	Msg, data_lenght										|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Write_BitBang_Data(unsigned long adresse,unsigned char data)
{	
#define WRITE 	0b00000010
unsigned long shift_16bit;
unsigned char Data_Lenght,shift_8bit,cnt;

Delais(1);
shift_8bit =0b10000000;
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (WRITE&shift_8bit)		//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
cnt=0;
shift_16bit =0b1000000000000000;
Data_Lenght = 16;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (adresse&shift_16bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_16bit = shift_16bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Data_Lenght--;
	Delais(2);
	cnt++;
	}
shift_8bit=0b10000000;
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (data&shift_8bit)		//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
return 1;
}

/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_BitBang_Data(unsigned char data)	|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Argumen	ts:	Msg, data_lenght										|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Read_BitBang_Data(unsigned long adresse)
{	
#define WRITE 	0b00000010
#define READ 	0b00000011
unsigned char Data_val;
unsigned long shift_16bit;
unsigned char Data_Lenght,shift_8bit;

Data_val = 0;
shift_8bit =0b10000000;
//transmetre et lire le premier octet TX-RX
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (READ&shift_8bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
shift_16bit =0b1000000000000000;
Data_Lenght = 16;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;							//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (adresse&shift_16bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_16bit = shift_16bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;							// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
/**********************************************************/
//Lire les data de RX
/**********************************************************/
Data_Lenght = 8;
while(Data_Lenght)
	{	
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if(EEPROM_DATA_IN)
		Data_val=Data_val|0x01;
	else
		Data_val=Data_val&0xFE; 
	EEPROM_CLOCK = 1;				//sample le data (SPI_CLK = 1)
	Delais(1);
	if(Data_Lenght>1)
		Data_val=Data_val<<1;	
	Data_Lenght--;
	}
return Data_val;
}

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
unsigned char Memory_Write_Page_Start(unsigned long adresse,unsigned char data)
{	
#define WRITE 	0b00000010
unsigned long shift_16bit;
unsigned char Data_Lenght,shift_8bit;

shift_8bit =0b10000000;
//EEPROM_CLOCK =0;
//transmetre et lire le premier octet TX-RX
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (WRITE&shift_8bit)		//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}

shift_16bit =0b1000000000000000;
Data_Lenght = 16;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (adresse&shift_16bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_16bit = shift_16bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}

shift_8bit=0b10000000;
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (data&shift_8bit)		//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
//EEPROM_CLOCK =0;
return 1;
}

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
void Memory_Write_Page_Continue(unsigned char data)
{	
#define WRITE 	0b00000010
unsigned char Data_Lenght,shift_8bit;

shift_8bit=0b10000000;
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (data&shift_8bit)		//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_8bit = shift_8bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
}

/******************************************************************
********************************************************************
|FONCTIONS:	unsigned char Memory_Read_Page_Start(unsigned char data)|
|Notes:	Envoie 1 message de longueur de 8bits						|
|																	|
|Argumen	ts:	Msg, data_lenght									|
|Variables locals:Data_Lenght;										|
|																	|
|Variables globals:													|
|																	|
********************************************************************
*******************************************************************/
unsigned char Memory_Read_Page_Start(unsigned short adresse)
{	
#define WRITE 	0b00000010
#define READ 	0b00000011
unsigned char Data_val;
unsigned long shift_16bit;
unsigned char Data_Lenght,shift_8bit;

Data_val = 0;
shift_8bit =0b10000000;
//transmetre et lire le premier octet TX-RX
Data_Lenght = 8;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (READ&shift_8bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	Delais(1);
	shift_8bit = shift_8bit>>1;
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}
shift_16bit =0b1000000000000000;
Data_Lenght = 16;
while (Data_Lenght)
	{
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if (adresse&shift_16bit)				//Ajuste SPI_data
		EEPROM_DATA_OUT = 1;
	else
		EEPROM_DATA_OUT = 0;
	shift_16bit = shift_16bit>>1;
	Delais(1);
	EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
	Delais(2);
	Data_Lenght--;
	}

/**********************************************************/
//Lire les data de RX
/**********************************************************/
Data_Lenght = 8;
while(Data_Lenght)
	{	
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if(EEPROM_DATA_IN)
		Data_val=Data_val|0x01;
	else
		Data_val=Data_val&0xFE; 
	EEPROM_CLOCK = 1;				//sample le data (SPI_CLK = 1)
	if(Data_Lenght>1)
		Data_val=Data_val<<1;	
	Delais(2);
	Data_Lenght--;
	}
return Data_val;
}
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
unsigned char Memory_Read_Page_Continue(void)
{	
unsigned char Data_val;
unsigned char Data_Lenght;

Data_Lenght = 8;
Data_val = 0;
/**********************************************************/
//Lire les data de RX
/**********************************************************/
EEPROM_CLOCK = 1;				// sample le data (SPI_CLK = 1)
while(Data_Lenght)
	{	
	EEPROM_CLOCK = 0;				//SPI1_CLK OUTPUT = 0
	Delais(1);
	if(EEPROM_DATA_IN)
		Data_val=Data_val|0x01;
	else
		Data_val=Data_val&0xFE; 
	Delais(1);
	EEPROM_CLOCK = 1;				//sample le data (SPI_CLK = 1)
	if(Data_Lenght>1)
		Data_val=Data_val<<1;	
	Delais(1);
	Data_Lenght--;
	}
return Data_val;
}

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
unsigned char Write_cycle_End(void)
{
//Delais(1000);
unsigned char Write_end_flag,test;
unsigned short cnt;
cnt=0;
Write_end_flag=1;
do	{
	test=Memory_Read_Status_BitBang();
	cnt++;
	}while(test);
return 1;
}
