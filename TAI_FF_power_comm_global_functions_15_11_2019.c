#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_power_comm_EIA232_functions_V1.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_global_functions.h"
#include "TAI_FF_power_comm_REAPER_Process_Functions.h"
#include "TAI_FF_power_comm_INJEKTOR_Process_Functions.h"
#include "TAI_FF_power_comm_config_manager_fonctions.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_switch_manager_V1_0.h"


//--------------------------------------------------------------
//void killAllInt()
//
//
//--------------------------------------------------------------
void killAllInt()
{
T1CON = 0;
IFS0bits.T1IF = 0;		// Clear interrupt flag
IEC0bits.T1IE = 0;		// Set ou clear interrupt enable bit

T2CON = 0;
IFS0bits.T2IF = 0;
IEC0bits.T2IE = 0;		// Set ou clear interrupt enable bit

T3CON = 0;
IFS0bits.T3IF = 0;
IEC0bits.T3IE = 0;

T4CON = 0;
IFS1bits.T4IF = 0;		// Clear interrupt flag
IEC1bits.T4IE = 0;		// Set ou clear interrupt enable bit

U1MODE=0;
IFS0bits.U1RXIF=0;		//Rx interrupt flag clear
IFS0bits.U1TXIF=0;		//Tx interrupt flag clear
IEC0bits.U1TXIE=0;		//clear Tx interrupt enable bit
IEC0bits.U1RXIE=0;		//Set Rx interrupt enable bit

U2MODE=0;
IFS1bits.U2RXIF = 0;	//Rx interrupt flag clear
IEC1bits.U2RXIE = 0;	//Set Rx interrupt enable bit
IFS1bits.U2TXIF = 0;	//Tx interrupt flag clear
IEC1bits.U2TXIE = 0;	//clear Tx interrupt enable bit

SPI1STATbits.SPIEN=0;	//SPI1 disable
IFS0bits.T3IF = 0;		// Clear interrupt flag
IEC0bits.T3IE = 0;		// Set ou clear interrupt enable bit

U1CON=0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void TestGlobalAutomode(unsigned char slave_n, unsigned char Master_Number)
| cherche l'état globale d'automation pour les moduels de la SSL
|		
| Variable de sortie:void
*****************************************************************
****************************************************************/
void TestGlobalAutomode(unsigned char slave_n, unsigned char Master_Number)
{
BankAutomode[slave_n+(Master_Number*4)]=FindBankAutomode(slave_n,Master_Number);
if(BankAutomode[slave_n+(Master_Number*4)])							//Test is tout les modules ua meme mode
	{
	if(GlobalAutomodeCnt==0)										//si oui on test si c'est la premiere bank
		LastMode=BankAutomode[slave_n+(Master_Number*4)];			//Sauve l'état d'automation de la 1er bank
	else
		{
		if(LastMode==BankAutomode[slave_n+(Master_Number*4)])		//si ou on test la prochaine bank egal a la précédente
			LastMode=BankAutomode[slave_n+(Master_Number*4)];
		else
			{	
			GlobalAutomodeFlag=0;
			LastMode=0;												//une bank n'est pas e tout au mdeme mode d'automation
			}
		}	
	GlobalAutomodeCnt++;
	if(GlobalAutomodeCnt>=MAX_SLAVE)
		{
		GlobalAutomodeFlag=LastMode;								//On a fait le tour et on sait si tout les bank sont au meme mode ou non
		GlobalAutomodeCnt=0;
		}		
	}
else
	{
	GlobalAutomodeFlag=0;											//La bank testée n'avait pas tout les modules au meme mode
	GlobalAutomodeCnt=0;											//On recommence a tester
	}
if(SetAllWaitFlag)
	if(GlobalAutomodeFlag==NewMode)
		SetAllWaitFlag=0;
}


/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char FindBankAutomode(void)
| retour l'état de la bank testée
|		
| Variable de sortie:void
*****************************************************************
****************************************************************/
unsigned char FindBankAutomode(unsigned char slave_n, unsigned char Master_Number)
{
unsigned char autodata0,autodata1,autodata2,global_automode;
if(!automodeRegisters[Master_Number][slave_n][0])
	autodata0=0;
else
if(automodeRegisters[Master_Number][slave_n][0]==255)
	autodata0=1;
else
	autodata0=10;

if(!automodeRegisters[Master_Number][slave_n][1])
	autodata1=0;
else
if(automodeRegisters[Master_Number][slave_n][1]==255)
	autodata1=2;
else
	autodata1=10;

if(!automodeRegisters[Master_Number][slave_n][2])
	autodata2=0;
else
if(automodeRegisters[Master_Number][slave_n][2]==255)
	autodata2=4;
else
	autodata2=10;

global_automode=autodata0+autodata1+autodata2;
switch(global_automode)
	{
	case 0:return 1;break;	//les bank est auto off
	case 1:return 2;break;	//auto write
	case 2:return 3;break;	//auto latch
	case 3:return 4;break;	//auto read
	case 4:return 5;break;	//auto trim off/live
	case 5:return 6;break;	//auto trim write
	case 6:return 7;break;	//auto trim latch
	case 7:return 8;break;	//auto trim read
	default:break;
	}
return 0;
}

/****************************************************************************************
*****************************************************************************************
|FONCTIONS:void TransportModeChangeManager(unsigned char midi message)
|Notes:	retir une donnée dans un buffer circulaire
|		sans test d'overflow	
|arguments 	:pointeur vers structure de buffer
|			:pointeur de donnée a retourner
|
|
*****************************************************************************************
****************************************************************************************/
void TransportModeChangeManager(unsigned char midi_message)
{
switch(midi_message)
	{
	case 0xfc:	Push_Config_data_In_Queue(1,1,4);
				GotoOnStopFlag=MAX_SLAVE;
				if((Config_registers[11]&0x03)>1)
					SessionChangeManager();
				break;					//STOP	
	case 0xfa:Push_Config_data_In_Queue(1,2,4);break;			//PLAY
	case 0xfb:Push_Config_data_In_Queue(1,2,4);break;			//pause?... mais je suis pas certain
	default:break;
	}
}

/****************************************************************
*****************************************************************
FONCTIONS:void AutoGotoOnStop(unsigned char slave_number,unsigned char Core_Number)

Notes:	Sur un stop de DAW on gere les changemetn de mode d'automation
	GlobalAutomodeFlag =2: pour un bank change
	GlobalAutomodeFlag =2: pour un bank change
	GlobalAutomodeFlag =2: pour un bank change
	
Codes de sortie
	1 OK
	-1	timeout
	-2 buffer avec plus de 1
*****************************************************************
****************************************************************/
void AutoGotoOnStop(unsigned char slave_number,unsigned char Core_Number)
{
unsigned char chan,bit_shift,Trim_Code,fader_n1;
unsigned char PatchedCore,PatchedSlave;
if(Config_registers[7])																//Automode goto est soit READ ou latch
	{
	PatchedCore=Core_Number;														//OK verifié
	PatchedSlave=slave_number;														//OK vérifié
	switch(GlobalAutomodeFlag)														//Gère le set-all automode
		{
		case 2:	NewMode=Config_registers[7]+1;
				//Push_Config_data_In_Queue(2,0,0);									//push la bank, 128+ =bank 0+ et plus
				Push_Config_data_In_Queue(3,Config_registers[7]|0x80,0);	
				GotoOnStopFlag=0;
				break;																//

		case 6:	NewMode=Config_registers[7]+4+1;
				//Push_Config_data_In_Queue(2,0,0);									//push la bank, 128+ =bank 0+ et plus
				Push_Config_data_In_Queue(3,(Config_registers[7]+4)|0x80,0);		//push le mode du registre
				GotoOnStopFlag=0;
				break;																//Stop alt...

		case 0:	if(GotoOnStopFlag)
					{
					GotoOnStopFlag--;
					switch(BankAutomode[slave_number+(Core_Number*4)])										//Gère le mode par bank
						{
						case 2:	Push_Config_data_In_Queue(2,128+(PatchedSlave+(PatchedCore*4)),0);			//push la bank, 128+ =bank 0+ et plus
								Push_Config_data_In_Queue(3,Config_registers[7],0);							//push le mode du registre
						break;
			
						case 6:	Push_Config_data_In_Queue(2,128+(PatchedSlave+(PatchedCore*4)),0);			//push la bank, 128+ =bank 0+ et plus
								Push_Config_data_In_Queue(3,Config_registers[7]+4,0);						//push le mode du registre
						break;
						case 0:	bit_shift=0x01;
								for(chan=0;chan<8;chan++)
									{
									if(((FADER_AUTOMODE_STATE[Core_Number][slave_number][chan]&0x0f)==1)			//On est en automode WRITE
										||((FADER_AUTOMODE_STATE[Core_Number][slave_number][chan]&0x0f)==5))		//On est en automode WRITE
										{
										if((FADER_AUTOMODE_STATE[Core_Number][slave_number][chan]&0x0f)==5)
											Trim_Code=0x04;
										else
											Trim_Code=0x00;
										fader_n1=chan+1+(PatchedSlave*8)+(PatchedCore*32);
										Push_Config_data_In_Queue(2,fader_n1,0);							//push le canal, 0 c'est aucun canal
										Push_Config_data_In_Queue(3,Config_registers[7]+Trim_Code,0);		//push le mode du registre
										}
									bit_shift=bit_shift<<1;
									}
						break;
						default:break;
						}
					}
				break;
		default:break;
		}
	}
else
	GotoOnStopFlag=0;	
}

//-----------------------------------------------------------------------------
//FUNCTION:void ExtAutoModeManager(unsigned char type,unsigned char autoMode,unsigned char idx)
//
//
//
//-----------------------------------------------------------------------------
void ExtAutoModeManager(unsigned char type,unsigned char autoMode,unsigned char idx)
{
unsigned char n;
		
switch(type)
	{
	case 0://set that channel 
		AUTOMODE[idx]=autoMode;
		ledAutoModeMng(idx);
	break;	
	case 1://Set all
		for(n=0;n<96;n++)
			{
			AUTOMODE[n]=autoMode;
			ledAutoModeMng(n);
			}
	break;	
	case 2:	//Set the channel of that bank
		for(n=0;n<8;n++)
			{
			AUTOMODE[idx+n]=autoMode;
			ledAutoModeMng(idx+n);
			}

}

}

//-----------------------------------------------------------------------------
//void SaveSysState(void)
//
//
//
//-----------------------------------------------------------------------------
void SaveSysState(void)
{
unsigned char m,n;
for(m=0;m<3;m++)
	{
	for(n=0;n<4;n++)
		{
		//savedSpiAutomodeBuffer[m][n][0]=automodeRegisters[m][n][0];
		//savedSpiAutomodeBuffer[m][n][1]=automodeRegisters[m][n][1];
		//savedSpiAutomodeBuffer[m][n][2]=automodeRegisters[m][n][2];
		//SavedMotorStatus[m][n]=ToUF_MotorStatus[m][n];
//		SavedVCAStatus[m][n]=ToMF_VCAStatus[m][n];
		//SavedAltStatus[m][n]=ToMF_AltStatus[m][n];
		//SavedGrpIsoStatus[m][n]=ToUF_GrpIsoStatus[m][n];
		}
	}
}

//-----------------------------------------------------------------------------
//void SetSysState(unsigned char state)
//
//
//
//-----------------------------------------------------------------------------
void SetSysState(unsigned char state)
{
unsigned char fader_number;

for(fader_number=0;fader_number<SYSTEMSIZE;fader_number++)
	txToSekaFaderData[fader_number]=(txToSekaFaderData[fader_number]&0xc000)|1000;
}

//-----------------------------------------------------------------------------
//void RestoreSysState(void)
//
//
//
//-----------------------------------------------------------------------------
void RestoreSysState(void)
{
unsigned char m,n;
for(m=0;m<3;m++)
	{
	for(n=0;n<4;n++)
		{
		//ToUF_MotorStatus[m][n]=SavedMotorStatus[m][n];
//		ToMF_VCAStatus[m][n]=SavedVCAStatus[m][n];
		//ToMF_AltStatus[m][n]=SavedAltStatus[m][n];
	//	ToMF_GrpIsoStatus[m][n]=SavedGrpIsoStatus[m][n];
		//for(chan=0;chan<4;chan++)
		//	VCA_IN_BUF[m][n][chan]=1023;
		}
	}
}

//-----------------------------------------------------------------------------
//void RestoreSysState(void)
//
//
//
//-----------------------------------------------------------------------------
void ResetSysState(void)
{
unsigned char fader_number;
	
for(fader_number=0;fader_number<SYSTEMSIZE;fader_number++)
	txToSekaFaderData[fader_number]=(txToSekaFaderData[fader_number]&0xc000)|1000;	
}

//---------------------------------------------------------------------------------------------
//FONCTIONS:void TAI_Session_Change_Manager(void)
//
//Notes:	recoit un message de changement de project REAPER
//		
//Codes de sortie:aucuns
//---------------------------------------------------------------------------------------------
void SessionChangeManager(void)
{
if(!RefreshFlag)
	RefreshFlag=Config_registers[15]+1;

}

//---------------------------------------------------------------------------------------------
//FONCTIONS:void refresh(void)
//
//Notes:
//		
//Codes de sortie:aucuns
//---------------------------------------------------------------------------------------------
void refresh(void)
{
RefreshFlag=Config_registers[15]+1;
}

//---------------------------------------------------------------------------------------------
//
//FONCTIONS:void MixOffManager(void)
//
//---------------------------------------------------------------------------------------------
void MixOffManager(void)
{
switch(gotoMixOffFlag)
	{
	case 1:
		SaveSysState();
		//SetAllAutoOff();
		ClearDawCutState();
		Push_Config_data_In_Queue(2,0,0);
		Push_Config_data_In_Queue(3,0,0);
		gotoMixOffFlag=2;
	break;
	case 5:
		Config_registers[10]=Config_registers[10]&0xfe;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(10,Config_registers[10]);
		PushRaspSingleData(0x20,Config_registers[10]);
		RaspMsgType=1;
		gotoMixOffFlag=0;
		MCMALedState[1]=MCMALedState[1]&0xef;

	break;
	default:break;
	}
}

//---------------------------------------------------------------------------------------------
//
//FONCTIONS:void MixOnManager(void)
//
//---------------------------------------------------------------------------------------------
void MixOnManager(void)
{
switch(gotoMixOnFlag)
	{
	case 1:gotoMixOnFlag=2;break;
	case 4:
		Config_registers[10]=Config_registers[10]|0x01;
		MCMALedState[1]=MCMALedState[1]|0x10;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(10,Config_registers[10]);
		PushRaspSingleData(0x20,Config_registers[10]);
		RaspMsgType=1;
		gotoMixOnFlag=0;
		SessionChangeManager();

	break;
	default:break;
	}
/*
LoadAutoState();
Config_registers[10]=Config_registers[10]|0x01;
SessionChangeManager();
if((Config_registers[11]&0x03)==3)
	configCnt=1;
Push_Config_data_In_Queue(10,Config_registers[10],SRC_RASP);
*/
}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS:void setFFMotor(unsigned char fader_number,unsigned char motorState)
//Note:
//
//
//
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void setFFMotor(unsigned char fader_number,unsigned char motorState)
{
if(!motorState)
	txToSekaFaderData[fader_number]=txToSekaFaderData[fader_number]&0xbfff;//WRITE Motor OFF
else
	txToSekaFaderData[fader_number]=txToSekaFaderData[fader_number]|0x4000;	//MOTOR ON Car FADER READ est off
}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS:void findServoOffset(int channel)
//Note:
//
//
//
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void findServoOffset(int channel)
{
int step,dbval;
int moyenne;
setFFMotor(channel,1);
for (step=0;step<16;step++)
	{
	switch(step){
		case 0:dbval=0; break;
		case 1:dbval=32; break;
		case 2:dbval=64; break;
		case 3:dbval=3*32; break;
		case 4:dbval=4*32; break;
		case 5:dbval=5*32; break;
		case 6:dbval=6*32; break;
		case 7:dbval=7*32; break;
		case 8:dbval=8*32; break;
		case 9:dbval=9*32; break;
		case 10:dbval=10*32; break;
		case 11:dbval=11*32; break;
		case 12:dbval=12*32; break;
		case 13:dbval=13*32; break;
		case 14:dbval=14*32; break;
		case 15:dbval=15*32; break;
		}
	txToSekaFaderData[channel]=(txToSekaFaderData[channel]&0xfc00)|dbval;//WRITE Motor OFF
	Delais(2500);
	moyenne=((txToSekaFaderData[channel]&0x03ff)-(rxFromSekaFaderData[channel]&0x03ff));
	offset[channel][step]=moyenne;
	}
/*
for (n=0;n<768;n++)
	{		
	TxSekaFaderValue[channel]=(txToSekaFaderData[channel]&0xfc00)|n;//WRITE Motor OFF
	Delais(100);
	moyenne=moyenne+((txToSekaFaderData[channel]&0x03ff)-(rxFromSekaFaderData[channel]&0x03ff));
	m++;
	if(m>=32)
		{
		moyenne=moyenne/32;
		offset[channel][step++]=moyenne;
		m=0;
		}
	}
*/
setFFMotor(channel,0);
step=0;
}
