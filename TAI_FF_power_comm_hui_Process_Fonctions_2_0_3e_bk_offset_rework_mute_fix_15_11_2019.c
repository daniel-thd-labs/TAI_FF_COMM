#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_hui_Process_Fonctions.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_global_functions.h"
#include "math.h"
#include "stdlib.h"

#define PLAYMODE 2
#define OFF 0
#define WRITE 1
#define LATCH 2
#define TOUCH 2
#define READ 3
#define TRIMLATCH 6
#define TRIMWRITE 5
#define TRIMREAD 7
#define PLAY 2

/********************************************************************
*******************************************************************
	* Function:        void Main_Process_Loop_HUI(unsigned char Core_Number, unsigned char slave_num)
	*
	* PreCondition:    None
	*
	* Input:           None
	*
	* Output:          None
	*
	* Side Effects:    None
	*	
	* Overview:        This function is a place holder for other user
	*                  routines. It is a mixture of both USB and
	*                  non-USB tasks.
	*
	* Note:            None
*******************************************************************
 *******************************************************************/
void Main_Process_Loop_HUI(unsigned char Core_Number,unsigned char slave_num)
{
//**********************Input from DAW--->Output deo SSL**********************
while(Get_Rx_Midi_Quant(RX_BUFFER))								//On vide le buffer Midi
	{
	if((Config_registers[11]&0x0f)==0x01)						//(Config_registers[11] 1 = ITB mode
		ITB_Midi_Parser_HUI_Fader_n_Switch(Core_Number);		//Gestion data from/to Master en mode In The Box
	else
		OTB_Midi_Parser_HUI_Fader_n_Switch(Core_Number);		//Gestion data from/to Master en mode Out The Box 			
	}
//******************************************************************

//if(!Refresh_auto_timer)
//	Auto_mode_refresh_manager(slave_num);
	
//**********************Input from SSL--->Output to DAW**********************
if(HUI_CORE_focus==Core_Number)
	{
	TestGlobalAutomode(slave_num,Core_Number);
	if(GotoOnStopFlag)
		AutoGotoOnStop(slave_num,Core_Number);	
	Auto_Mode_Transmit_HUI(slave_num,Core_Number);
	configured=ProcessOutput(slave_num);		//Gestion master_data et Tx USB
	HUI_UF_Fader_And_Mute_Data(slave_num,Core_Number);
	configured=ProcessOutput(slave_num);		//Gestion master_data et Tx USB
	}
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void HUI_UF_Fader_And_Mute_Data(unsigned char slave_n,unsigned char Core_Number)
|Notes:	 Gestion des données reçues du master génération des messages midi
|
|arguments: slave_n
|valeur retrounées:void
|
|
*****************************************************************
****************************************************************/
void HUI_UF_Fader_And_Mute_Data(unsigned char slave_n,unsigned char Core_Number)
{
unsigned int alt_adc,chan_bit,temp_val,fader_number;
//unsigned int local_auto_mode,
unsigned char chan,fader_val_hi,fader_val_lo,skipflag;
unsigned char newcore,newslave;

newcore=CoreMatrix[Core_Number][slave_n];
newslave=SlaveMatrix[Core_Number][slave_n];

chan_bit=0x01;
for(chan=0;chan<8;chan++)
	{
	if((rxFromSekaFaderData[fader_number]!=0xffff)	//test si Fader present
		&&(rxFromSekaSwtData[fader_number]!=0xff))
		{
		fader_number=(CoreMatrix[Core_Number][slave_n]*32)+(SlaveMatrix[Core_Number][slave_n]*8)+chan+1;
//-------------------------
// Gestion des MUTES
//-------------------------
		
		if((taiCutState[Core_Number][slave_n]&chan_bit)!=(fromFaderCutState[Core_Number][slave_n]&chan_bit))											//PAS en MUTE
			{
			taiCutState[Core_Number][slave_n]=(taiCutState[Core_Number][slave_n]&~chan_bit)|(fromFaderCutState[Core_Number][slave_n]&chan_bit);
			if(taiCutState[Core_Number][slave_n]&chan_bit)													//Test si "on-mute déja transmis
				{
				if(fromDawCutState[Core_Number][slave_n]&chan_bit)
					{
					if(Config_registers[10]&0x01)
						{
						Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x42);				//tx un-mute
						Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x02);				//tx un-mute
						}
					//fromDawCutState[Core_Number][slave_n]=fromDawCutState[Core_Number][slave_n]&~chan_bit;
					}
				}
			else
				{//MUTE OFF			
				if(!(fromDawCutState[Core_Number][slave_n]&chan_bit))
					{
					if(Config_registers[10]&0x01)
						{
						Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x42);			//tx mute
						Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x02);			//tx mute release
						}
					//dawCutState[Core_Number][slave_n]=dawCutState[Core_Number][slave_n]|chan_bit;
					}
				}
			}
	
		//--------------Gestion du TOUCH--------------------------
	
		if(rxFromSekaFaderData[fader_number]&0x8000)		//Fader is touched
			{
			if((!(FF_WasTouchedState[Core_Number][slave_n]&chan_bit))
				&&(Config_registers[1]==2))
				{
				FF_WasTouchedState[Core_Number][slave_n]=(FF_WasTouchedState[Core_Number][slave_n]|chan_bit);
				if(Config_registers[10]&0x01)
					Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x40);											//Send fader touch "ON"
				//FF_HUI_Touch_Auto_Mode_Transmit(Core_Number, slave_n,chan,WRITE);
				}
			}
		else
		if(!(rxFromSekaFaderData[fader_number]&0x8000))		//Fader is not touched
			{
			if(FF_WasTouchedState[Core_Number][slave_n]&chan_bit)
				{
				FF_WasTouchedState[Core_Number][slave_n]=FF_WasTouchedState[Core_Number][slave_n]&~chan_bit;
				if((!(Config_registers[22]&0x80))&&(Config_registers[10]&0x01))
					Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x00);											//Send fader touch "OFF"
				}
			}
	//--------------Gestion des VOLUME CHANGES--------------------------
		alt_adc=(rxFromSekaFaderData[fader_number]&0x3fff);
		temp_val=abs(alt_adc-SPI_BUF_SENT[Core_Number][slave_n][chan]);
		if((temp_val)||(FADER_UPDATE_FLAG[Core_Number][slave_n]&chan_bit))											//tout sauf "READ MODE"
			{
			skipflag=0;
			switch(FADER_AUTOMODE_STATE[Core_Number][slave_n][chan])
				{
				case WRITE:	
						if(Config_registers[22]&0x20)
							if(Config_registers[1]==PLAY)															
							skipflag=1;
				
				case OFF:	
						if(!skipflag)
							{
							fader_val_hi=alt_adc>>7;																	//decalage du MSB
							fader_val_lo=alt_adc & 0x7f;																//masquage du LSB en 3bits
							if(Config_registers[10]&0x01)
								{
								Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x40);										//Send fader touch "ON"
								Build_Midi_Msg(0,0,newslave,chan,chan,fader_val_hi,(0x20+chan),fader_val_lo);				//fait le message de fader position/move
								Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x00);										//Send fader touch "OFF"
								}
							FADER_UPDATE_FLAG[Core_Number][slave_n]=FADER_UPDATE_FLAG[Core_Number][slave_n]&~chan_bit;
							}
						SPI_BUF_SENT[Core_Number][slave_n][chan]=rxFromSekaFaderData[fader_number]&0x3fff;		//Sauve la nouvelle valeur
						break;
				}
			}
		if(FADER_AUTOMODE_STATE[Core_Number][slave_n][chan]==OFF)
			txToSekaFaderData[fader_number]=(txToSekaFaderData[fader_number]&0xc000)|alt_adc;
		else
			txToSekaFaderData[fader_number]=(txToSekaFaderData[fader_number]&0xc000)|DAW_IN_BUF[newcore][newslave][chan];
	
		chan_bit=chan_bit<<1;
		}
	toFaderCutState[Core_Number][slave_n]=fromFaderCutState[Core_Number][slave_n]|fromDawCutState[Core_Number][slave_n];
	}
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void UF_HUI_Touch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char channel,unsigned char state)
|
| 1=WRITE
| 		
*****************************************************************
****************************************************************/
void UF_HUI_Touch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char channel,unsigned char state)
{
unsigned char AutomodeOn,AutomodeOff;

switch(state)
	{
	case WRITE:AutomodeOn=0x44;AutomodeOff=0x04;break;
	case TOUCH:AutomodeOn=0x45;AutomodeOff=0x05;break;
	}

unsigned char fader_n;
fader_n=channel+1+(slave_n*8)+(Master_Number*32);
/*
Build_Midi_Msg(0,0,slave_n,channel,0x0f,channel,0x2f,0x44);		//Transmet fader select "ON"	
Build_Midi_Msg(0,0,slave_n,0x00,0x0f,0x18,0x2f,AutomodeOn);		//Automation press
Build_Midi_Msg(0,0,slave_n,0x00,0x0f,0x18,0x2f,AutomodeOff);	//Automation release
Build_Midi_Msg(0,0,slave_n,channel,0x0f,channel,0x2f,0x04);	//Transmet fader auto select "OFF"
configured=ProcessOutput(slave_n);
*/
Push_Config_data_In_Queue(2,fader_n,0);												//transmet update du master avec push du canal
Push_Config_data_In_Queue(3,state,0);													//et push du mode WRITE
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void HUI_Latch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char channel)
|
| Force le mode d'automation a celui selectionné par la SSL
| 0=off:trim
| 1=WRITE
| 2=LATCH/TOUCH
| 3=TRIM READ
| 5=TRIM WRITE
| 6=TRIM LATCH
| 7=TRIM READ
| 		
*****************************************************************
****************************************************************/
void HUI_Latch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char channel)
{
unsigned char fader_n;
unsigned char newcore,newslave;


newcore=CoreMatrix[Master_Number][newslave];
newslave=SlaveMatrix[Master_Number][newslave];

fader_n=channel+1+(slave_n*8)+(Master_Number*32);
Build_Midi_Msg(0,0,newslave,channel,0x0f,channel,0x2f,0x44);		//Transmet fader select "ON"	
Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x44);			//Automation "write" press
Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x04);			//Automation "write" release
Build_Midi_Msg(0,0,newslave,channel,0x0f,channel,0x2f,0x04);	//Transmet fader auto select "OFF"
configured=ProcessOutput(newslave);
		
Push_Config_data_In_Queue(2,fader_n,0);														//transmet update du master avec push du canal
Push_Config_data_In_Queue(3,1,0);													//et push du mode WRITE
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//FONCTIONS:	void Auto_Mode_Transmit_HUI(unsigned char slave_n,unsigned char Core_Number)
//
// Force le mode d'automation a celui selectionné par la SSL
//		
//-----------------------------------------------------------------
//-----------------------------------------------------------------
void set_ITB_mode(void)
{
unsigned char fader_number;
for(fader_number=0;fader_number<32;fader_number++)
	txToSekaFaderData[fader_number]=(txToSekaFaderData[fader_number]&0xc000)|100;		//VCA toujours a la même valeur 0db
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void Auto_Mode_Transmit_HUI(unsigned char slave_n,unsigned char Core_Number)
|
| Force le mode d'automation a celui selectionné par la SSL
|		
*****************************************************************
****************************************************************/
void Auto_Mode_Transmit_HUI(unsigned char slave_n,unsigned char Core_Number)
{
const char LocalBitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned short LED_AUTOMODE;
unsigned char chan,select_reset_flag,fader_number;
unsigned char newcore,newslave;

newcore=CoreMatrix[Core_Number][slave_n];
newslave=SlaveMatrix[Core_Number][slave_n];

if(Config_registers[10]&0x01)
	{
	if(SetAllWaitFlag)
		{
		Set_All_Automation_Mode_HUI(Config_registers[3]&0x0f);
		}
	else
		{
		select_reset_flag=0;
		for(chan=0;chan<8;chan++)
			{
			fader_number=(slave_n*8)+chan+1;
			//if((fader_number+1+(Core_Number*32))!=Config_registers[21])
				{
				LED_AUTOMODE=automodeRegisters[Core_Number][slave_n][0]&LocalBitIndex[chan];
				LED_AUTOMODE=LED_AUTOMODE|((automodeRegisters[Core_Number][slave_n][1]&LocalBitIndex[chan])<<1);				
				LED_AUTOMODE=LED_AUTOMODE|((automodeRegisters[Core_Number][slave_n][2]&LocalBitIndex[chan])<<2);
				LED_AUTOMODE=LED_AUTOMODE>>chan;
				
				if(LED_AUTOMODE!=FADER_AUTOMODE_STATE[Core_Number][slave_n][chan])
					//||(Automation_Refresh_flag))
					{
					if(!select_reset_flag)
						select_reset_flag=1;
					Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x44);	//Transmet fader select "ON"	
					switch (LED_AUTOMODE)
						{
						case 0:	Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x43);	//Automation "OFF" press
								Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x03);	//Automation "OFF" release
								configured=ProcessOutput(slave_n);
								#ifdef __ULTIMATION
								ToMF_MotorStatus[newcore][newslave]=ToMF_MotorStatus[newcore][newslave]&~LocalBitIndex[chan];
								#endif
								break;		//Transmet automation mode
						case 1:	Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x44);	//Automation "write" press
								Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x04);	//Automation "write" release
								configured=ProcessOutput(slave_n);
								#ifdef __ULTIMATION
								ToMF_MotorStatus[newcore][newslave]=ToMF_MotorStatus[newcore][newslave]&~LocalBitIndex[chan];
								#endif
								break;		//Transmet automation mode
					#ifndef __ULTIMATION
						case 2:	Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x41);	//Automation "Latch" press
								Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x01);	//Automation ""Latch" release
								configured=ProcessOutput(slave_n);
								break;		//Transmet automation mod
					#else
						case 2:	Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x45);	//Automation "TOUCH" press
								Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x05);	//Automation "TOUCH" release
								configured=ProcessOutput(slave_n);
								ToMF_MotorStatus[newcore][newslave]=ToMF_MotorStatus[newcore][newslave]|LocalBitIndex[chan];
								break;		//Transmet automation mod
					#endif
						case 3:	Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x42);	//Automation "READ"	press
								Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x02);	//Automation "READ" release
								configured=ProcessOutput(slave_n);
								#ifdef __ULTIMATION
								ToMF_MotorStatus[newcore][newslave]=ToMF_MotorStatus[newcore][newslave]|LocalBitIndex[chan];
								#endif
								break;		//Transmet automation mode
						default:break;
						}
					if((Config_registers[11]&0x0f)==0x01)
						if((LED_AUTOMODE>4)
							&&(LED_AUTOMODE!=FADER_AUTOMODE_STATE[Core_Number][slave_n][chan]))//&&
							{
							Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x40);	//Transmet TRIM "ON" Pressed
							Build_Midi_Msg(0,0,newslave,0x00,0x0f,0x18,0x2f,0x00);	//Transmet TRIM "ON" Released
							}
					Build_Midi_Msg(0,0,newslave,chan,0x0f,chan,0x2f,0x04);	//Transmet fader auto select "OFF"
					FADER_AUTOMODE_STATE[Core_Number][slave_n][chan]=LED_AUTOMODE;
					FADER_UPDATE_FLAG[Core_Number][slave_n]=FADER_UPDATE_FLAG[Core_Number][slave_n]|LocalBitIndex[chan];
					configured=ProcessOutput(newslave);
					}
				}	
			}
		}
	}	
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void Set_All_Automation_Mode_HUI(unsigned char ext_automode)
| Force le mode d'automation a celui selectionné par la SSL
|		
| Variable de sortie:void
*****************************************************************
****************************************************************/
void Set_All_Automation_Mode_HUI(unsigned char ext_automode)
{
if(Set_All_Auto_Flag)
	{
	Push_Config_data_In_Queue(2,0,0);
	Push_Config_data_In_Queue(3,ext_automode|0x80,0);
	}
SetAllWaitFlag=0;
Set_All_Auto_Flag=0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void Set_Automation_Off_HUI(void)
|
|Notes:	Établie les différents mode d'automation
|		0:off		hui:0x41	Mackie_control:	0x4c
|		1:Write			0x42					0x4b
|		2:Latch			0x43					0x4e
|		3:read			0x44					0x4a
|		
|Codes de sortie
|	1 OK
|	-1	timeout
|	-2 buffer avec plus de 1
*****************************************************************
****************************************************************/
void Set_Automation_Off_HUI(void)
{
unsigned char m,n;

Set_All_Auto_Flag=4;
Set_all_counter=6;
for(m=0;m<4;m++)
	for(n=0;n<8;n++)
		FADER_AUTOMODE_STATE[0][m][n]=OFF;
while(Set_all_counter)
	{
	Set_All_Automation_Mode_HUI(Config_registers[3]&0x0f);
	configured=ProcessOutput(0);
	}
Set_All_Auto_Flag=0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	Do_Bank_Slide_HUI(unsigned char slide_data,unsigned char source)
|
|Note:	Envoie le message de faire de banmk slide
|		
|Codes de sortie : void
|
*****************************************************************
****************************************************************/
void Do_Bank_Slide_HUI(unsigned char slide_data,unsigned char source)
{
unsigned char CORE_target,Bank_shift_count,To_Slave_bank_number;
unsigned char Bank_limit_offset;

if(source==2)
	slide_data=slide_data+32;
else
if(source==3)
	slide_data=slide_data+64;

if(slide_data<=32)
	CORE_target=0;
else
if(slide_data<=64)
	CORE_target=1;
else
if(slide_data<=96)
	CORE_target=2;

if(CORE_target!=HUI_CORE_focus)													//Si on est dans une autres zone par défaut =0
	{
	Bank_limit_offset=0;
	Bank_shift_count=0;
	To_Slave_bank_number=(CORE_target+1)*4;										//4,8 ou 12
	if(CORE_target>HUI_CORE_focus)												//On est plus haut
		{
		Bank_shift_count=((CORE_target+1)*4)-((HUI_CORE_focus+1)*4);			//nombre de shift a effecuter max sans limite par Bank_limit_offset (multiples de 4)
		if(To_Slave_bank_number>(Config_registers[15]+1))							//ex: 8-4=4
			Bank_limit_offset=To_Slave_bank_number-(Config_registers[15]+1);		//ex si 5=3, si 6=2, si 7 =1, si 8 =0
		Bank_shift_count=(Bank_shift_count-Bank_limit_offset);
		while(Bank_shift_count)
			{
			Build_Midi_Msg(0,0,0,0,0x0f,0x0a,0x2f,0x43);						//Transmet Bank up = "ON"
			Build_Midi_Msg(0,0,0,0,0x0f,0x0a,0x2f,0x03);						//Transmet Bank up = "OFF"
			Bank_shift_count--;
			}
		Bank_slide_mask=Bank_limit_offset;										//mis a jour du masque de traitement pour les fader en offset
		}
	else	
	if(CORE_target<HUI_CORE_focus)
		{
		Bank_shift_count=((HUI_CORE_focus+1)*4)-((CORE_target+1)*4);			//nombre de shift a effecuter max
		if(Bank_slide_mask)
			Bank_limit_offset=Bank_slide_mask;
		if(Bank_limit_offset)
			Bank_shift_count=Bank_shift_count-Bank_limit_offset;
		while(Bank_shift_count)
			{
			Build_Midi_Msg(0,0,0,0,0x0f,0x0a,0x2f,0x41);						//Transmet Bank up = "ON"
			Build_Midi_Msg(0,0,0,0,0x0f,0x0a,0x2f,0x01);						//Transmet Bank up = "OFF"
			Bank_shift_count--;
			}
		Bank_slide_mask=0;
		}
	HUI_CORE_focus=CORE_target;													//mettre a jour la position
	Config_registers[13]=Config_registers[13]&0b00111100;
	Config_registers[13]=Config_registers[13]|HUI_CORE_focus|(Bank_slide_mask<<2);
	Push_Config_data_In_Queue(13,Config_registers[13],0);
	}
}


