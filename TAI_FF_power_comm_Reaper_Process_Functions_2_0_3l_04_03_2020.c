// V2_0_3h 
//MUTE FROM GROUP REMOVED 24/01/2018

#include "TAI_FF_Versionner.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_REAPER_Process_Functions.h"
#include "TAI_FF_power_comm_global_functions.h"
#include "TAI_FF_power_comm_Group_Fonctions.h"

#include "math.h"
#include "stdlib.h"

#define OFF 0
#define WRITE 1
#define LATCH 2
#define TOUCH 2
#define READ 3
#define TRIMTOUCH 6
#define TRIMLATCH 6
#define TRIMWRITE 5
#define TRIMREAD 7
#define STOP 0
#define PLAY 2

const char LocalBitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

//#include "./usb_function_midi.h"
//-----------------------------------------------------------------------------------------------
// Function:        void REAPER_FF_Main_Process_Loop(unsigned char master,unsigned char slave)
//
// PreCondition:    None
//
// Input:           None
//
// Output:          None
//
// Side Effects:    None
//	
// Overview:        This function is a place holder for other user
//                  routines. It is a mixture of both USB and
//                  non-USB tasks.
//
// Note:            None
//-----------------------------------------------------------------------------------------------
void REAPER_FF_Main_Process_Loop(unsigned char master,unsigned char slave)
{
unsigned char chan;
//-----------------------Input from DAW--->Output deo SSL------------------
if(TAI_Ping_Flag==1)
	TAI_Ping();
configured=ProcessOutput(slave);																						//Gestion master1_data et Tx USB
//----------------------Input from DAW--->Output to SSL---------------------
while(Get_Rx_Midi_Quant(RX_BUFFER))
	Midi_Parser_TAI_Fader_n_Switch();			//Gestion data from REAPER to Master( en mode OTB)
//----------------------Input from SSL--->Output to DAW---------------------
REAPER_FF_Auto_Mode_Transmit(master,slave,chan);
REAPER_FF_FaderAndMuteDataMng(master,slave,chan);
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//FONCTIONS:void REAPER_FF_Auto_Mode_Transmit(unsigned char slave_n, unsigned char Master_Number, unsigned char chan)
//Notes:	 Gestion des donnés reçu du master génération des messages midi pour les messages d'automtion mode
// Force le mode d'automation a celui selectionné par la SSL
// 0=off:trim
// 1=WRITE
// 2=LATCH
// 3=READ
// 4=Touch
//SSL--->REAPER
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void REAPER_FF_Auto_Mode_Transmit(unsigned char Master_Number, unsigned char slave_n, unsigned char chan)
{
int fader_number;
for(chan=0;chan<8;chan++)
	{
	fader_number=(Master_Number*32)+(slave_n*8)+chan;
	//fader_number=(Master_Number*32)+(slave_n*8)+chan;
	if((rxFromSekaFaderData[fader_number]!=0xffff)&&(rxFromSekaSwtData[fader_number]!=0xff))
		{
		if((AUTOMODE[fader_number]!=FADER_AUTOMODE_STATE[Master_Number][slave_n][chan])			//test pour changement de mode d'automation
			||RefreshFlag)
			{
			FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]=AUTOMODE[fader_number];
			switch(AUTOMODE[fader_number])
				{
				case 0:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
						TX_LAST_NRPN_MSB=AUTOMODE_CODE;
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);											//transmet message NRPN LSB et numéro de fader(+1)
						TX_LAST_NRPN_LSB=fader_number;
						if(Config_registers[10]&0x01)
							Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,0,0,0);										//Automation "OFF" press
						setFFMotor(fader_number,0);
						break;		//Transmet automation mode
				case 1:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
						TX_LAST_NRPN_MSB=AUTOMODE_CODE;
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);											//transmet message NRPN LSB et numéro de fader(+1)
						TX_LAST_NRPN_LSB=fader_number;
						if(Config_registers[10]&0x01)
							Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,1,0,0);										//Automation "Write" press
						setFFMotor(fader_number,0);
						break;		//Transmet automation mode
				case 2:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
						TX_LAST_NRPN_MSB=AUTOMODE_CODE;
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);											//transmet message NRPN LSB et numéro de fader(+1)
						TX_LAST_NRPN_LSB=fader_number;
						if(Config_registers[10]&0x01)
							Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,4,0,0);										//Automation "touch" press
						setFFMotor(fader_number,1);
						SPI_BUF_SENT[Master_Number][slave_n][chan]=rxFromSekaFaderData[fader_number]&0x3fff;				//Sauve la position du fadre pour trigger le latch sur un fader move						
						break;		//Transmet automation mod
				case 3:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
						TX_LAST_NRPN_MSB=AUTOMODE_CODE;
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);											//transmet message NRPN LSB et numéro de fader(+1)
						TX_LAST_NRPN_LSB=fader_number;
						if(Config_registers[10]&0x01)
							Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,3,0,0);										//Automation "READ" press
						setFFMotor(fader_number,1);
				break;				//Transmet automation mode
				case 4:	break;		//Transmet automation mode
				case 5:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
						TX_LAST_NRPN_MSB=AUTOMODE_CODE;
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);											//transmet message NRPN LSB et numéro de fader(+1)
						TX_LAST_NRPN_LSB=fader_number;
						if(Config_registers[10]&0x01)
							Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,5,0,0);										//Automation "Write" press
						setFFMotor(fader_number,0);
						break;		//Transmet automation mode
				case 6:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
						TX_LAST_NRPN_MSB=AUTOMODE_CODE;
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);											//transmet message NRPN LSB et numéro de fader(+1)
						TX_LAST_NRPN_LSB=fader_number;
						if(Config_registers[10]&0x01)
							Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,8,0,0);										//Automation "touch" press
						setFFMotor(fader_number,1);
						SPI_BUF_SENT[Master_Number][slave_n][chan]=rxFromSekaFaderData[fader_number]&0x3fff;				//Sauve la position du fadre pour trigger le latch sur un fader move						
						break;		//Transmet automation mod
				case 7:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
						TX_LAST_NRPN_MSB=AUTOMODE_CODE;
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);											//transmet message NRPN LSB et numéro de fader(+1)
						TX_LAST_NRPN_LSB=fader_number;
						if(Config_registers[10]&0x01)
							Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,7,0,0);										//Automation "READ" press
						setFFMotor(fader_number,1);
				break;		//Transmet automation mode
				default:break;
				}
			FADER_UPDATE_FLAG[Master_Number][slave_n]=FADER_UPDATE_FLAG[Master_Number][slave_n]|LocalBitIndex[chan];					//mettre a jours ce fader
			MUTE_UPDATE_FLAG[Master_Number][slave_n]=MUTE_UPDATE_FLAG[Master_Number][slave_n]|LocalBitIndex[chan];
			}
		if(Config_registers[10]&0x01)
			configured=ProcessOutput(slave_n);																						//Gestion master1_data et Tx USB
		}
	}
}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS:void REAPER_FF_FaderAndMuteDataMng(unsigned char Master_Number,unsigned char slave_n,unsigned char chan)
//Notes:	 Gestion des donnés reçu du master génération des messages midi
//code du master
//arguments: slave_n
//valeur retrounées:void
//
//SSL--->REAPER
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void REAPER_FF_FaderAndMuteDataMng(unsigned char Master_Number, unsigned char slave_n, unsigned char chan)
{
int TempValue;
unsigned int fader_number,TempValue2;
unsigned char fader_val_hi,fader_val_lo,RealFaderNumber;

for(chan=0;chan<8;chan++)
	{
	fader_number=(Master_Number*32)+(slave_n*8)+chan;
	//fader_number=(CoreMatrix[Master_Number][slave_n]*32)+(SlaveMatrix[Master_Number][slave_n]*8)+chan+1;
	RealFaderNumber=(Master_Number*32)+(slave_n*8)+chan+1;
	
	if(((rxFromSekaFaderData[fader_number]!=0xffff)	//test si Fader present
		&&(rxFromSekaSwtData[fader_number]!=0xff))
		||(RealFaderNumber==Config_registers[21]))	//C'est le Master fader		
		{
		//VCA_IN_BUF[Master_Number][slave_n][chan]=alt_adc;
//-------------------------
//FADER TOUCH Management
//-------------------------
		if(Config_registers[1]==PLAY)																				//Si on est en play, Ne pas transmetre en automode READ ou...
			{
			if((FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]==TOUCH)
				||(FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]==TRIMTOUCH))										//Si on est em mode touch ou trim
				{
				if(rxFromSekaFaderData[fader_number]&0x8000)		//Fader is touched
					{
					if(!(FF_WasTouchedState[Master_Number][slave_n]&LocalBitIndex[chan]))
						{
						FF_WasTouchedState[Master_Number][slave_n]=(FF_WasTouchedState[Master_Number][slave_n]|LocalBitIndex[chan]);
						if(Config_registers[10]&0x01)
							{
							if(FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]==TOUCH)
								REAPER_FF_Touch_Auto_Mode_Transmit(Master_Number, slave_n,chan,WRITE);
							else
							if(FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]==TRIMTOUCH)
								REAPER_FF_Touch_Auto_Mode_Transmit(Master_Number, slave_n,chan,TRIMWRITE);
							}
						}
					}
				}
			else
			if(!(rxFromSekaFaderData[fader_number]&0x8000))
				{
				if(FF_WasTouchedState[Master_Number][slave_n]&LocalBitIndex[chan])
					{
					FF_WasTouchedState[Master_Number][slave_n]=FF_WasTouchedState[Master_Number][slave_n]&~LocalBitIndex[chan];
					if(Config_registers[10]&0x01)
						{
						if(!(Config_registers[22]&0x80))
							{
							if(FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]==WRITE)
								REAPER_FF_Touch_Auto_Mode_Transmit(Master_Number, slave_n,chan,TOUCH);				
							else
							if(FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]==TRIMWRITE)
								REAPER_FF_Touch_Auto_Mode_Transmit(Master_Number, slave_n,chan,TRIMTOUCH);				
							}
						}
					}
				}
			}
//-------------------------
//FADER MOVES Management
//-------------------------
		TempValue=FF_LinkManager(Master_Number,slave_n,chan,FF_GroupManager(Master_Number,slave_n,chan));
		TempValue2=abs(TempValue-SPI_BUF_SENT[Master_Number][slave_n][chan]);
		if(	TempValue2||RefreshFlag||(FADER_UPDATE_FLAG[Master_Number][slave_n]&LocalBitIndex[chan]))									//test pour mise a jour
			{
			switch(FADER_AUTOMODE_STATE[Master_Number][slave_n][chan])
				{
				case 0:		SPI_BUF_SENT[Master_Number][slave_n][chan]=TempValue;											//Sauve la nouvelle valeur
							fader_val_hi=TempValue>>7;
							fader_val_lo=TempValue & 0x7f;
							if(Config_registers[10]&0x01)
								{							
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,VOLUME_CODE,0,0);								//transmet message complet
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);							//transmet message complet
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,fader_val_hi,0,0);						//Partie MSB du data entry
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_LSB,fader_val_lo,0,0);						//Partie LSB du data entry
								}					
							FADER_UPDATE_FLAG[Master_Number][slave_n]=FADER_UPDATE_FLAG[Master_Number][slave_n]&~LocalBitIndex[chan];
				break;
				case 1:	
				case 5:	//if((Config_registers[22]&0x20)||(Config_registers[1]==2))												//test if (0) Write only in PLAY 
							{																									//or (1) WRITE ALLWAYS
							SPI_BUF_SENT[Master_Number][slave_n][chan]=TempValue;												//Sauve la nouvelle valeur
							fader_val_hi=TempValue>>7;
							fader_val_lo=TempValue & 0x7f;
							if(Config_registers[10]&0x01)
								{							
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,VOLUME_CODE,0,0);								//transmet message complet
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);								//transmet message complet
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,fader_val_hi,0,0);						//Partie MSB du data entry
								Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_LSB,fader_val_lo,0,0);						//Partie LSB du data entry
								}					
							FADER_UPDATE_FLAG[Master_Number][slave_n]=FADER_UPDATE_FLAG[Master_Number][slave_n]&~LocalBitIndex[chan];
							}
				break;		
				}
			}
		}	
	if(Config_registers[10]&0x01)
		configured=ProcessOutput(slave_n);																						//Gestion master1_data et Tx USB
	}

//-------------------------
//MUTES Management
//-------------------------
if((taiCutState[Master_Number][slave_n]!=fromFaderCutState[Master_Number][slave_n])					//Test si il y a eu du chagment chez les cuts
	||MUTE_UPDATE_FLAG[Master_Number][slave_n])														//Ou doit etre updaté
	{
	//taiCutState[Master_Number][slave_n]=fromFaderCutState[Master_Number][slave_n];
	for(chan=0;chan<8;chan++)
		{
		//fader_number=(CoreMatrix[Master_Number][slave_n]*32)+(SlaveMatrix[Master_Number][slave_n]*8)+chan+1;
		fader_number=(Master_Number*32)+(slave_n*8)+chan;
		RealFaderNumber=(Master_Number*32)+(slave_n*8)+chan+1;
		
		if(((rxFromSekaFaderData[fader_number]!=0xffff)	//test si Fader present
			&&(rxFromSekaSwtData[fader_number]!=0xff))
			||(RealFaderNumber==Config_registers[21]))			
			{
			if((fromFaderCutState[Master_Number][slave_n]&LocalBitIndex[chan]))																//PAS en MUTE
				{
				if((!(taiCutState[Master_Number][slave_n]&LocalBitIndex[chan]))	//Cut est off, met a jour:CUT=ON											//Test si "on-mute déja transmis donc on transmet si le mute state est 0
				||(MUTE_UPDATE_FLAG[Master_Number][slave_n]&LocalBitIndex[chan]))												
					{	
					taiCutState[Master_Number][slave_n]=taiCutState[Master_Number][slave_n]|LocalBitIndex[chan];				//Update le status de Mute "ON"
					MUTE_UPDATE_FLAG[Master_Number][slave_n]=MUTE_UPDATE_FLAG[Master_Number][slave_n]&~LocalBitIndex[chan];
					if(Config_registers[10]&0x01)
						{
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,MUTE_CODE,0,0);										//transmet message complet
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);										//transmet message complet
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,127,0,0);										//Partie MSB du data entry a 127
						}
					}
				}	
			else		//On a recu une commande de mute de la SSL
				{
				if((taiCutState[Master_Number][slave_n]&LocalBitIndex[chan])
					||(MUTE_UPDATE_FLAG[Master_Number][slave_n]&LocalBitIndex[chan]))											//Test si mute en fonction donc etat mute a changé
					{																											//transmetre et gerer MUTE message
					taiCutState[Master_Number][slave_n]=taiCutState[Master_Number][slave_n]&~LocalBitIndex[chan];				//Update status un-mute transmis
					MUTE_UPDATE_FLAG[Master_Number][slave_n]=MUTE_UPDATE_FLAG[Master_Number][slave_n]&~LocalBitIndex[chan];
					if(Config_registers[10]&0x01)
						{							
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,MUTE_CODE,0,0);										//transmet message complet 1: le mute code
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);										//2:transmet message complet
						Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,0,0,0);											//3:Partie MSB du data entry a 0
						}
					}
				}
			}
		if(Config_registers[10]&0x01)
			configured=ProcessOutput(slave_n);																						//Gestion master1_data et Tx USB
		}
	}

}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS: void REAPER_FF_Touch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char channel,unsigned char state)
//Force le mode d'automation a celui selectionné par la SSL
// 0=off:trim
// 1=WRITE
// 2=LATCH/TOUCH
// 3=TRIM READ
// 5=TRIM WRITE
// 6=TRIM LATCH
// 7=TRIM READ
//
//SSL--->REAPER
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void REAPER_FF_Touch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char chan,unsigned char state)
{
unsigned char fader_number;
fader_number=(Master_Number*32)+(slave_n*8)+chan;
//fader_number=(CoreMatrix[Master_Number][slave_n]*32)+(SlaveMatrix[Master_Number][slave_n]*8)+chan+1;

Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);						//transmet message complet d'automation
Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_number+1,0,0);						//transmet no de fader complet
Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,state,0,0);							//Automation "Write" 
Push_Config_data_In_Queue(2,fader_number+1,0);												//transmet update du master avec push du canal
Push_Config_data_In_Queue(3,state,0);														//et push du mode WRITE
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void REAPER_FF_Latch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n)
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
void REAPER_FF_Latch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char chan)
{
unsigned char fader_n;
fader_n=chan+1+(slave_n*8)+(Master_Number*32);
Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);						//transmet message complet d'automation
Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,fader_n,0,0);								//transmetno de fader complet
Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,
			FADER_AUTOMODE_STATE[Master_Number][slave_n][chan],0,0);						//Automation "Write" 
Push_Config_data_In_Queue(2,fader_n,0);														//transmet update du master avec push du canal
Push_Config_data_In_Queue(3,FADER_AUTOMODE_STATE[Master_Number][slave_n][chan],0);													//et push du mode WRITE
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void TAI_MasterAutoModeTransmit(unsigned char Automode)
|
| Force le mode d'automation a celui selectionné par la SSL
| 0=off:trim
| 1=WRITE
| 2=LATCH
| 3=READ
| 4=Touch
		
*****************************************************************
****************************************************************/
void REAPER_FF_MasterAutoModeTransmit(unsigned char Automode)
{
unsigned char Master_Number,slave_n,chan;
unsigned char fader_number;
unsigned char AdjustedMasterFader;

AdjustedMasterFader=Config_registers[21]-8;

if(Config_registers[10]&0x01)
	{
	if(Config_registers[21])
		{
		Master_Number=(AdjustedMasterFader-1)/32;
		slave_n=((AdjustedMasterFader-1)/8)%4;
		chan=((AdjustedMasterFader-1)%8);
		fader_number=(Master_Number*32)+(slave_n*8)+chan;
	

		Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,AUTOMODE_CODE,0,0);											//transmet message NRPN MSB et code d'automation
		TX_LAST_NRPN_MSB=AUTOMODE_CODE;
		Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,Config_registers[21],0,0);										//transmet message NRPN LSB et numéro de fader(+1)
		TX_LAST_NRPN_LSB=0;
		switch (Automode)
			{
			case 0:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,Automode,0,0);								//Automation "OFF" press
					break;		//Transmet automation mode
			case 1:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,Automode,0,0);								//Automation "Write" press
					break;		//Transmet automation mode
			case 2:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,Automode,0,0);								//Automation "Latch" press
					SPI_BUF_SENT[Master_Number][slave_n][chan]=rxFromSekaFaderData[fader_number]&0x3fff;					//Sauve la position du fader pour trigger le latch sur un fader move						
					break;		//Transmet automation mod
			case 3:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,Automode,0,0);								//Automation "READ" press
					break;		//Transmet automation mode
			case 4:	break;		//Transmet automation mode
			case 5:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,Automode,0,0);								//Automation "Write" press
					break;		//Transmet automation mode
			case 6:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,Automode,0,0);								//Automation "Latch" press
					SPI_BUF_SENT[Master_Number][slave_n][chan]=rxFromSekaFaderData[fader_number]&0x3fff;					//Sauve la position du fader pour trigger le latch sur un fader move						
					break;		//Transmet automation mod
			case 7:	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,Automode,0,0);								//Automation "READ" press
					break;		//Transmet automation mode
			default:break;
			}
		FADER_UPDATE_FLAG[Master_Number][slave_n]=FADER_UPDATE_FLAG[Master_Number][slave_n]|LocalBitIndex[chan];		//mettre a jours ce fader
		MUTE_UPDATE_FLAG[Master_Number][slave_n]=MUTE_UPDATE_FLAG[Master_Number][slave_n]|LocalBitIndex[chan];
		FADER_AUTOMODE_STATE[Master_Number][slave_n][chan]=Automode;
		configured=ProcessOutput(slave_n);
		}
	}
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void TAI_Ping(void)
|Notes:	 Gestion des donnés reçu du master génération des messages midi
|code du master
|arguments: slave_n
|valeur retrounées:void
|
|
*****************************************************************
****************************************************************/
void TAI_Ping(void)
{
Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,CONTROL_CODE,0,0);					//transmet message complet
Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,0x7f,0,0);					//Partie MSB du data entry a 127
TAI_Ping_Flag=0;
//DAW_ONLINE_Timer=5000;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void TAI_On_Line_Manager(unsigned char Slave_num)
||
|Notes:	Recois le data du AD et fait la mise a la mise a l'échelle pour le Ptools
|		
|Codes de sortie
|	1 OK
|
|	-1	timeout
|	-2 buffer avec plus de 1
*****************************************************************
****************************************************************/
void TAI_On_Line_Manager(void)
{
TAI_Ping_Flag=2;
DAW_on_line_flag[0]=1;	
DAW_on_line_flag[1]=1;	
DAW_on_line_flag[2]=1;	
DAW_on_line_flag[3]=1;
DAW_on_line_flag[4]=1;
}


