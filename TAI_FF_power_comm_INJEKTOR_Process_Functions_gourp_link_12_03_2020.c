// V2_0_3h 
//MUTE FROM GROUP REMOVED 24/01/2018

#include "TAI_FF_Versionner.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_global_functions.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_Group_Fonctions.h"
#include "TAI_FF_power_comm_INJEKTOR_Process_Functions.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
#include "TAI_FF_power_comm_switch_manager_V1_0.h"
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
#define TRANSPORT_STATE Config_registers[1]
#define STOP 0
#define PLAY 2
#define SNAP 0x80
#define WRITEONSTOP 0x20

const char UdpBitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char INJEKTOR_WasTouchedState[3][4];

//#include "./usb_function_midi.h"
//-----------------------------------------------------------------------------------------------
// Function:        void Main_Process_Loop_TAI(unsigned char slaveum,unsigned char Core_Number)
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

void Main_Process_Loop_UDP(unsigned char master,unsigned char slave)
{
unsigned char chan;
//-----------------------Input from DAW--->Output deo SSL------------------
if(UDP_Ping_Flag==1)
	UDP_Ping();
configured=ProcessOutput(slave);																						//Gestion master1_data et Tx USB

while(Get_Rx_Midi_Quant(RX_BUFFER))
	INJEKTOR_Midi_Parser();			//Gestion data from UDP
//----------------------Input from SSL--->Output to DAW---------------------
if(configCnt)
	midiSendGlobalConfig();
UDP_Auto_Mode_Transmit(master,slave,chan);
while(Get_Rx_Midi_Quant(RX_BUFFER))
	INJEKTOR_Midi_Parser();			//Gestion data from UDP
UDP_FF_FaderAndMuteDataMng(master,slave,chan);
FADER_UPDATE_FLAG[master][slave]=0;
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
// FUNCTIONS:void TAI_Auto_Mode_Transmit(unsigned char master, unsigned char slave, unsigned char chan)
// Notes:	 Gestion des donnés reçu du master génération des messages midi pour les messages d'automtion mode
// Force le mode d'automation a celui selectionné par la SSL
// 0=off:trim
// 1=WRITE
// 2=LATCH
// 3=READ
// 4=Touch
// added Seperate CUT WRITE MODE on bit#4 (0x10) = WRITE CUT
// if in Fader in READ or TOUCH, cuts a played back from DAW data
// However, CUT reads can be enabled or disabled from the plug-in Enable CUT switch
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void UDP_Auto_Mode_Transmit( unsigned char master, unsigned char slave, unsigned char chan)
{
int fader_number;
unsigned char localAutoMode;
unsigned char bank=(master*4)+slave;
unsigned char localWriteCutChange=FF_MuteRec_State[bank]^last_FF_MuteRec_State[bank];
unsigned char cutWriteMaskBit=0;
for(chan=0;chan<8;chan++)
	{
	fader_number=(master*32)+(slave*8)+chan;
	localAutoMode=FADER_AUTOMODE_STATE[master][slave][chan]&0x0f;
	if((rxFromSekaFaderData[fader_number]!=0xffff)&&(rxFromSekaSwtData[fader_number]!=0xff))	//Fader existe in system
		{
		if((GotoOnStopFlag)&&(Config_registers[7]&0x03))
			ManageAutmodeOnStop(master,slave,chan);
		//else
		if((localAutoMode!=AUTOMODE[fader_number])				//test pour changement de mode d'automation
			||(localWriteCutChange&UdpBitIndex[chan])			//de chamgenet du bit WRITE CUTS
			||RefreshFlag)
			{
			cutWriteMaskBit=0;
			if(FF_MuteRec_State[bank]&UdpBitIndex[chan])
				cutWriteMaskBit=0x10;
			FADER_AUTOMODE_STATE[master][slave][chan]=AUTOMODE[fader_number];
			switch(AUTOMODE[fader_number])
				{
				case 0:	if(Config_registers[10]&0x01)
							{
							UDP_PrepAutModeData(master,slave,chan);
							Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,0|cutWriteMaskBit,0,0);			//Automation "OFF" press
							configured=ProcessOutput(slave);											//Gestion master1_data et Tx USB
							lastAutoModeWasTouchFlag[master][slave]=lastAutoModeWasTouchFlag[master][slave]&~UdpBitIndex[chan];
							}	
						if(Config_registers[100])
							Config_registers[100]=0;
						motorManager(fader_number);
						SPI_BUF_SENT[master][slave][chan]=2000;
						break;		//Transmet automation mode
				case 1:	if(Config_registers[10]&0x01)
							{
							UDP_PrepAutModeData(master,slave,chan);
							Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,0x01|cutWriteMaskBit,0,0);		//Automation "Write" press
							configured=ProcessOutput(slave);											//Gestion master1_data et Tx USB
							}	
						motorManager(fader_number);
						break;		//Transmet automation mode

				case 2:	if(Config_registers[10]&0x01)
							{
							UDP_PrepAutModeData(master,slave,chan);
							Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,0x04|cutWriteMaskBit,0,0);		//Automation "touch" press
							configured=ProcessOutput(slave);											//Gestion master1_data et Tx USB
							lastAutoModeWasTouchFlag[master][slave]=lastAutoModeWasTouchFlag[master][slave]|UdpBitIndex[chan];
							}
						SPI_BUF_SENT[master][slave][chan]=rxFromSekaFaderData[fader_number]&0x3fff;			//Sauve la position du fader
						motorManager(fader_number);
						break;		//Transmet automation mod

				case 3:	if(Config_registers[10]&0x01)
							{
							UDP_PrepAutModeData(master,slave,chan);
							Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,0x03|cutWriteMaskBit,0,0);		//Automation "READ" press
							configured=ProcessOutput(slave);											//Gestion master1_data et Tx USB
							lastAutoModeWasTouchFlag[master][slave]=lastAutoModeWasTouchFlag[master][slave]&~UdpBitIndex[chan];
							}	
						if(Config_registers[100])
							Config_registers[100]=0;
						SPI_BUF_SENT[master][slave][chan]=2000;
						motorManager(fader_number);
						break;				//Transmet automation mode

				case 4:	break;				//n'existe pas

				case 5:	if(Config_registers[10]&0x01)
							{
							UDP_PrepAutModeData(master,slave,chan);
							Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,5|cutWriteMaskBit,0,0);	//Automation "TRIM Write" UDP MODE 5
							configured=ProcessOutput(slave);													//Gestion master1_data et Tx USB
							}	
						motorManager(fader_number);
						break;		//Transmet automation mode

				case 6:	
						if(Config_registers[10]&0x01)
							{
							UDP_PrepAutModeData(master,slave,chan);
							Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,8|cutWriteMaskBit,0,0);			//Automation "TRIM touch" UDP MODE 8
							configured=ProcessOutput(slave);											//Gestion master1_data et Tx USB
							lastAutoModeWasTouchFlag[master][slave]=lastAutoModeWasTouchFlag[master][slave]|UdpBitIndex[chan];
							}	
						SPI_BUF_SENT[master][slave][chan]=rxFromSekaFaderData[fader_number]&0x3fff;			//Sauve la position du fader			
						motorManager(fader_number);
						break;		//Transmet automation mod

				case 7:	if(Config_registers[10]&0x01)
							{
							UDP_PrepAutModeData(master,slave,chan);
							Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,7|cutWriteMaskBit,0,0);			//Automation "READ" press
							configured=ProcessOutput(slave);											//Gestion master1_data et Tx USB
							lastAutoModeWasTouchFlag[master][slave]=lastAutoModeWasTouchFlag[master][slave]&~UdpBitIndex[chan];
							}	
						motorManager(fader_number);
						break;		//Transmet automation mode
				default:break;
				}
			FADER_UPDATE_FLAG[master][slave]=FADER_UPDATE_FLAG[master][slave]|UdpBitIndex[chan];		//mettre a jours ce fader
			MUTE_UPDATE_FLAG[master][slave]=MUTE_UPDATE_FLAG[master][slave]|UdpBitIndex[chan];
			}
		last_FF_MuteRec_State[bank]=FF_MuteRec_State[bank];	
		}
	}
if(GotoOnStopFlag)
	GotoOnStopFlag--;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//FONCTIONS:void motorManager(unsigned char chan)
//
// Force le mode d'automation a celui selectionné par la SSL
// 0=off:trim
// 1=WRITE
// 2=LATCH
// 3=READ
// 4=Touch
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void motorManager(unsigned char chan)
{
if(!faderGroupNumber[chan])
	{
	switch((FADER_AUTOMODE_STATE[chan/32][(chan/8)%4][chan%8])&0x0f)
		{
		case 0:	setFFMotor(chan,0);
				break;		//Transmet automation mode
		case 1:	if(enableWriteVolState[chan/32][(chan/8)%4]&UdpBitIndex[chan])
					setFFMotor(chan,0);//WRITE Motor OFF
				else
					setFFMotor(chan,1);	//MOTOR ON Car FADER READ est off
				break;		//Transmet automation mode
		case 2:	setFFMotor(chan,1);		//TOUCH Motor ON
				break;		//Transmet automation mod
		case 3:	if(enableReadVolState[chan/32][(chan/8)%4]&UdpBitIndex[chan])
					setFFMotor(chan,1);	//MOTOR OFF Car FADER READ est off
				else
					setFFMotor(chan,0);	//READ Motor ON
				break;				//Transmet automation mode
		case 4:	break;		//Transmet automation mode
		case 5:	setFFMotor(chan,0);		//WRITETRIM Motor OFF
				break;		//Transmet automation mode
		case 6:	setFFMotor(chan,1);		//TOUCH TRIM Motor ON
				break;		//Transmet automation mod
		case 7:	if(enableReadVolState[chan/32][(chan/8)%4]&UdpBitIndex[chan])
					setFFMotor(chan,1);	//READ Motor ON
				else
					setFFMotor(chan,0);	//READ TRIM Motor OFF
				break;		//Transmet automation mode
		default:break;
		}
	}
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//FONCTIONS:	void PrepAutModeData(unsigned char master,
//								unsigned char slave,
//								unsigned char LED_AUTOMODE,
//								unsigned char Trim_Code);
//
// Force le mode d'automation a celui selectionné par la SSL
// 0=off:trim
// 1=WRITE
// 2=LATCH
// 3=READ
// 4=Touch
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void UDP_PrepAutModeData(unsigned char master,unsigned char slave,unsigned char chan)
{
unsigned char fader_number;
fader_number=(master*32)+(slave*8)+chan+1;

Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPAUTOMODE_CODE,0,0);								//transmet message NRPN MSB et code d'automation
TX_LAST_NRPN_MSB=UDPAUTOMODE_CODE;
Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_number,0,0);									//transmet message NRPN LSB et numéro de fader(+1)
TX_LAST_NRPN_LSB=fader_number;
}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS:void UDP_UF_FaderAndMuteDataMng(unsigned char master, unsigned char slave, unsigned char chan)
//Notes:	 Gestion des donnés reçu du master génération des messages midi
//code du master
//arguments: slave
//valeur retrounées:void
//
//SSL--->REAPER
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void UDP_FF_FaderAndMuteDataMng(unsigned char master, unsigned char slave, unsigned char chan)
{
//unsigned int grpTmpVal;
//unsigned int lnkTmpVal;
int TempValue;
unsigned int TempValue2;
unsigned int fader_number;
unsigned char fader_val_hi,fader_val_lo;
unsigned char localAutoMode;
for(chan=0;chan<8;chan++)
	{
	fader_number=(master*32)+(slave*8)+chan+1;
	localAutoMode=FADER_AUTOMODE_STATE[master][slave][chan]&0x0f;
	//-------------------------
	//FADER TOUCH Management
	//-------------------------
	if(fromFaderTouchState[master][slave]&UdpBitIndex[chan])
		{
		if(!(INJEKTOR_WasTouchedState[master][slave]&UdpBitIndex[chan]))
			{
			INJEKTOR_WasTouchedState[master][slave]=(INJEKTOR_WasTouchedState[master][slave]|UdpBitIndex[chan]);
			if((localAutoMode==TOUCH)
				||(localAutoMode==TRIMTOUCH))										//Si on est em mode touch ou trim
				{
				Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPTOUCH_CODE,0,0);								//transmet message complet
				Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_number,0,0);								//transmet message complet
				Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,1,0,0);									//Partie MSB du data entry
				if(Config_registers[10]&0x01)
					{
					if(TRANSPORT_STATE==PLAY)																				//Si on est en play, Ne pas transmetre en automode READ ou...
						{
						if(localAutoMode==TOUCH)
							{
							UDP_FF_Touch_Auto_Mode_Transmit(master, slave,chan,WRITE);
							}
						else
							UDP_FF_Touch_Auto_Mode_Transmit(master, slave,chan,TRIMWRITE);
						
						}
					}
				}
			}
		}
	else
	if(INJEKTOR_WasTouchedState[master][slave]&UdpBitIndex[chan])
		{
		INJEKTOR_WasTouchedState[master][slave]=INJEKTOR_WasTouchedState[master][slave]&~UdpBitIndex[chan];
		Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPTOUCH_CODE,0,0);								//transmet message complet
		Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_number,0,0);								//transmet message complet
		Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,0,0,0);									//Partie MSB du data entry
		if(Config_registers[10]&0x01)
			{
			if(!(Config_registers[22]&SNAP))
				{
				if(localAutoMode==WRITE)
					{
					if(lastAutoModeWasTouchFlag[master][slave]&UdpBitIndex[chan])
						UDP_FF_Touch_Auto_Mode_Transmit(master, slave,chan,TOUCH);				
					}
				else
				if(localAutoMode==TRIMWRITE)
					{
					if(lastAutoModeWasTouchFlag[master][slave]&UdpBitIndex[chan])
						UDP_FF_Touch_Auto_Mode_Transmit(master, slave,chan,TRIMTOUCH);				
					}
				}
			}
		}

//--------------------------------------------------
//FADER Moves MIDI send management
//--------------------------------------------------
	//VCA_IN_BUF[master][slave][chan]
	TempValue=FF_LinkManager(master,slave,chan,FF_GroupManager(master,slave,chan));
	//TempValue=FF_GroupManager(master,slave,chan);
	//if(faderLinkNumber[fader_number-1]) 												//Si le fader est groupé
	//TempValue=rxFromSekaFaderData[fader_number]&0x03ff;
	
	txToSekaFaderData[fader_number-1]=(txToSekaFaderData[fader_number-1]&~0x03ff)|TempValue;
	TempValue2=abs(TempValue-SPI_BUF_SENT[master][slave][chan]);
	if(	TempValue2||RefreshFlag||(FADER_UPDATE_FLAG[master][slave]&UdpBitIndex[chan]))							//test pour mise a jour
		{
		switch(localAutoMode)
			{
			case WRITE:
			case TRIMWRITE:
				if((Config_registers[22]&WRITEONSTOP)
					||(TRANSPORT_STATE==PLAY))	
					{
					SPI_BUF_SENT[master][slave][chan]=TempValue;											//Sauve la nouvelle valeur
					fader_val_hi=TempValue>>7;
					fader_val_lo=TempValue & 0x7f;
					if(Config_registers[10]&0x01)
						{							
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPVOLUME_CODE,0,0);								//transmet message complet
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_number,0,0);								//transmet message complet
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,fader_val_hi,0,0);						//Partie MSB du data entry
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_LSB,fader_val_lo,0,0);						//Partie LSB du data entry
						}					
					}
			break;
			case OFF:
			case READ:
			case TRIMREAD:
			case TOUCH:
			case TRIMTOUCH:
				SPI_BUF_SENT[master][slave][chan]=TempValue;												//Sauve la nouvelle valeur
				fader_val_hi=TempValue>>7;
				fader_val_lo=TempValue & 0x7f;
				if(Config_registers[10]&0x01)
					{							
					Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPVOLUME_CODE,0,0);								//transmet message complet
					Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_number,0,0);								//transmet message complet
					Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,fader_val_hi,0,0);						//Partie MSB du data entry
					Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_LSB,fader_val_lo,0,0);						//Partie LSB du data entry
					}					
			break;
			}
		}
	}
	if(Config_registers[10]&0x01)
		configured=ProcessOutput(slave);																						//Gestion master1_data et Tx USB

	//-------------------------
	//MUTES Management
	//-------------------------
	if((fromFaderCutState[master][slave]!=taiCutState[master][slave])											//Test si il y a eu du chagment chez les cuts
		||(MUTE_UPDATE_FLAG[master][slave]))																	//Ou doit etre updaté
		{
		for(chan=0;chan<8;chan++)
			{
			fader_number=(master*32)+(slave*8)+chan+1;
			if(fromFaderCutState[master][slave]&UdpBitIndex[chan])
				{
				if((!(taiCutState[master][slave]&UdpBitIndex[chan]))
					||(MUTE_UPDATE_FLAG[master][slave]))
				//updateFFChanLeds(0x01,fader_number-1,1);
				//toFaderCutState[master][slave]=ActiveCutState[master][slave]|UdpBitIndex[chan];
					{
					taiCutState[master][slave]=taiCutState[master][slave]|UdpBitIndex[chan];				//Update le status de Mute "ON"
					MUTE_UPDATE_FLAG[master][slave]=MUTE_UPDATE_FLAG[master][slave]&~UdpBitIndex[chan];
					if(Config_registers[10]&0x01)												//Test si "on-mute déja transmis donc on transmet si le mute state est 0
						{
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPMUTE_CODE,0,0);				//transmet message complet
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_number,0,0);				//transmet message complet
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,127,0,0);					//Partie MSB du data entry a 127
						}
					}
				}
	 		else
				{
				if((taiCutState[master][slave]&UdpBitIndex[chan])
					||(MUTE_UPDATE_FLAG[master][slave]))
					{
					taiCutState[master][slave]=taiCutState[master][slave]&~UdpBitIndex[chan];				//Update le status de Mute "ON"
					MUTE_UPDATE_FLAG[master][slave]=MUTE_UPDATE_FLAG[master][slave]&~UdpBitIndex[chan];
					//updateFFChanLeds(0x01,fader_number-1,0);
					if(Config_registers[10]&0x01)												//Test si "on-mute déja transmis donc on transmet si le mute state est 0
						{
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPMUTE_CODE,0,0);				//transmet message complet
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_number,0,0);				//transmet message complet
						Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,0,0,0);					//Partie MSB du data entry a 127
						}
					}
				}
			if(Config_registers[10]&0x01)
				configured=ProcessOutput(slave);																//Gestion master1_data et Tx USB

			}
		}
	toFaderCutState[master][slave]=ActiveCutState[master][slave];
	for(chan=0;chan<8;chan++)
		{
		fader_number=(master*32)+(slave*8)+chan+1;
		if(toFaderCutState[master][slave]&UdpBitIndex[chan])
			updateFFChanLeds(0x01,fader_number-1,1);
		else
			updateFFChanLeds(0x01,fader_number-1,0);
		}
}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS: void UDP_FF_Touch_Auto_Mode_Transmit(unsigned char master,unsigned char slave, unsigned char channel,unsigned char state)
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
void UDP_FF_Touch_Auto_Mode_Transmit(unsigned char master,unsigned char slave, unsigned char chan,unsigned char state)
{
AUTOMODE[(master*32)+(slave*8)+chan]=state;
ledAutoModeMng((master*32)+(slave*8)+chan);
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void TAI_Latch_Auto_Mode_Transmit(unsigned char master,unsigned char slave)
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
void UDP_Latch_Auto_Mode_Transmit(unsigned char master,unsigned char slave, unsigned char chan)
{
unsigned char fader_n;
fader_n=chan+1+(slave*8)+(master*32);
if(Config_registers[10]&0x01)
	{
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPAUTOMODE_CODE,0,0);						//transmet message complet d'automation
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,fader_n,0,0);								//transmetno de fader complet
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,
			FADER_AUTOMODE_STATE[master][slave][chan],0,0);						//Automation "Write" 
	}
Push_Config_data_In_Queue(2,fader_n,0);														//transmet update du master avec push du canal
Push_Config_data_In_Queue(3,FADER_AUTOMODE_STATE[master][slave][chan],0);													//et push du mode WRITE
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void UDP_MasterAutoModeTransmit(unsigned char Automode)
|
| Force le mode d'automation a celui selectionné par la SSL
| 0=off:trim
| 1=WRITE
| 2=LATCH
| 3=READ
| 4=Touch
		
*****************************************************************
****************************************************************/
void UDP_MasterAutoModeTransmit(unsigned char Automode)
{
unsigned char master,slave,chan;
unsigned char fader_number;
unsigned char AdjustedMasterFader;

AdjustedMasterFader=Config_registers[21]-8;

if(Config_registers[10]&0x01)
	{
	if(Config_registers[21])
		{
		master=(AdjustedMasterFader-1)/32;
		slave=((AdjustedMasterFader-1)/8)%4;
		chan=((AdjustedMasterFader-1)%8);
		fader_number=(master*32)+(slave*8)+chan+1;

		Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPAUTOMODE_CODE,0,0);									//transmet message NRPN MSB et code d'automation
		TX_LAST_NRPN_MSB=UDPAUTOMODE_CODE;
		Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,Config_registers[21],0,0);								//transmet message NRPN LSB et numéro de fader(+1)
		TX_LAST_NRPN_LSB=0;
		switch (Automode)
			{
			case 0:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Automode,0,0);							//Automation "OFF" press
					break;		//Transmet automation mode
			case 1:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Automode,0,0);							//Automation "Write" press
					break;		//Transmet automation mode
			case 2:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Automode,0,0);							//Automation "Latch" press
					SPI_BUF_SENT[master][slave][chan]=rxFromSekaFaderData[fader_number]&0x3fff;						//Sauve la position du fader pour trigger le latch sur un fader move						
					break;		//Transmet automation mod
			case 3:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Automode,0,0);							//Automation "READ" press
					break;		//Transmet automation mode
			case 4:	break;		//Transmet automation mode
			case 5:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Automode,0,0);							//Automation "Write" press
					break;		//Transmet automation mode
			case 6:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Automode,0,0);							//Automation "Latch" press
					SPI_BUF_SENT[master][slave][chan]=rxFromSekaFaderData[fader_number]&0x3fff;						//Sauve la position du fader pour trigger le latch sur un fader move						
					break;		//Transmet automation mod
			case 7:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Automode,0,0);										//Automation "READ" press
					break;		//Transmet automation mode
			default:break;
			}
		FADER_UPDATE_FLAG[master][slave]=FADER_UPDATE_FLAG[master][slave]|UdpBitIndex[chan];					//mettre a jours ce fader
		MUTE_UPDATE_FLAG[master][slave]=MUTE_UPDATE_FLAG[master][slave]|UdpBitIndex[chan];
		FADER_AUTOMODE_STATE[master][slave][chan]=Automode;
		configured=ProcessOutput(slave);
		}
	}
}

//----------------------------------------------------------------------------------------------
//FUNCTION: void ManageAutmodeOnStop(unsigned char master,unsigned char slave,unsigned char chan)
//
//
//
//----------------------------------------------------------------------------------------------
void ManageAutmodeOnStop(unsigned char master,unsigned char slave,unsigned char chan)
{
unsigned char newmode;
unsigned char localAutoMode	=FADER_AUTOMODE_STATE[master][slave][chan]&0x0f;
if((localAutoMode==1)		//in WRITE 
	||(localAutoMode==5))	//or in TRIMWRITE
	{
	newmode=0;
	if((Config_registers[7]&0x03)==3)//GOTO READ
		{
		if(localAutoMode==1)//IN WRITE MODE
			newmode=3;	//READ
		else
			newmode=7;	//TRIM READ
		AUTOMODE[(master*32)+(slave*8)+chan]=newmode;
		FF_MuteRec_State[(master*4)+slave]=FF_MuteRec_State[(master*4)+slave]&~UdpBitIndex[chan];
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfd;
		ledAutoModeMng((master*32)+(slave*8)+chan);
		}
	else
	if((Config_registers[7]&0x03)==2)//GOTO TOUCH
		{
		if(localAutoMode==1)
			newmode=2;//TOUCH
		else
			newmode=6;//TRIM TOUCH
		AUTOMODE[(master*32)+(slave*8)+chan]=newmode;
		FF_MuteRec_State[(master*4)+slave]=FF_MuteRec_State[(master*4)+slave]&~UdpBitIndex[chan];
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfd;
		ledAutoModeMng((master*32)+(slave*8)+chan);
		}
	}
else
	if(FF_MuteRec_State[(master*4)+slave]&UdpBitIndex[chan])
		{
		FF_MuteRec_State[(master*4)+slave]=FF_MuteRec_State[(master*4)+slave]&~UdpBitIndex[chan];
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfd;
		ledAutoModeMng((master*32)+(slave*8)+chan);
		}
}

/*
//----------------------------------------------------------------------------------------------
//FUNCTION: void Set_bank_automode(unsigned char Automode,unsigned char slave,unsigned char master)
//
//
//
//----------------------------------------------------------------------------------------------
void Set_bank_automode(unsigned char Automode,unsigned char slave,unsigned char master)
{
switch(Automode)
	{
	case 1:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,0,0,0);			//Automation "OFF"
			break;		//Transmet automation mode
	case 2:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,1,0,0);			//Automation "Write"
			break;		//Transmet automation mode
	case 3:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,2,0,0);			//Automation "Latch"
			break;		//Transmet automation mod
	case 4:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,3,0,0);			//Automation "READ"
			break;		//Transmet automation mode
	case 5:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,5,0,0);			//Automation "TOUCH"
			break;		//Transmet automation mode
	case 6:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,6,0,0);			//Automation "TRIM WRITE"
			break;		//Transmet automation mode
	case 7:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,7,0,0);			//Automation "TRIM LATCH"
			break;		//Transmet automation mode
	case 8:	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,8,0,0);			//Automation "TRIM READ"
			break;		//Transmet automation mode
	default:break;
	}
}
*/

/****************************************************************
*****************************************************************
|FONCTIONS:	void TAI_Ping(void)
|Notes:	 Gestion des donnés reçu du master génération des messages midi
|code du master
|arguments: slave
|valeur retrounées:void
|
|
*****************************************************************
****************************************************************/
void UDP_Ping(void)
{
int m,n;
Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,UDPPING_CODE,0,0);					//transmet message complet
Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Config_registers[15],0,0);					//Partie MSB du data entry a 127
UDP_Ping_Flag=0;

if((Config_registers[17]&0x0f)==0x0f) //test pour DAW actif
	SetOffLineCnt=0;
else
if(SetOffLineCnt<=4)
	SetOffLineCnt++;	
else
if(SetOffLineCnt==5)
	{
	if((!gotoMixOnFlag)&&(!gotoMixOffFlag))
		{
		gotoMixOffFlag=1;
		SetOffLineCnt=6;
		for (m=0;m<3;m++)
			for (n=0;n<4;n++)
				UDPActiveChannelFlag[m][n]=0x00;
		}
	}
DAW_on_line_flag[0]=1;	
DAW_on_line_flag[1]=1;	
DAW_on_line_flag[2]=1;	
DAW_on_line_flag[3]=1;
DAW_on_line_flag[4]=1;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void TAI_On_Line_Manager(unsigned char slaveum)
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
void UDP_On_Line_Manager(unsigned char status)
{
Config_registers[17]=(Config_registers[17]&0x0f)|status;
UDP_Ping_Flag=2;
}
