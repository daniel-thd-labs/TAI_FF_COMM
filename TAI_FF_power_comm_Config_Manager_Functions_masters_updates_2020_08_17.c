#include "TAI_FF_Versionner.h"
#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_config_manager_fonctions.h"
#include "TAI_FF_power_comm_hui_Process_Fonctions.h"
#include "TAI_FF_power_comm_EIA232_functions_V1.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_global_functions.h"

//#include "TAI_V2_Versionner.h"
//const char SlaveToCoreIndex[] ={0,0,0,0,1,1,1,1,2,2,2,2};
//const char AbsSlaveToSlaveIndex[] ={0,1,2,3,0,1,2,3,0,1,2,3};
const char CfgBitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
/****************************************************************
*****************************************************************
|FONCTIONS: void ConfigTest(unsigned char Reg_index,unsigned char Reg_data, unsigned char source)
*****************************************************************
****************************************************************/
void ConfigTest(void)
{
char source;
source=Pull_Config_data_In_Queue();
if(source>=0)
	{
	if(Config_registers[0])
		ConfigDataManager(Config_registers[0],Config_registers[Config_registers[0]],source);
	}
else
	source=0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:
|void ConfigDataManager(unsigned char Reg_index,unsigned char Reg_data, unsigned char Source)
|sources:	0:Core
|			1:Master1
|			2:Master2
|			3:Master2
|			4:USB
|			5:RASP
|			6:EXT
|
|registre 0x01 = Stop flag
|Config_registers[2] = Set automation index...(0-127 individuel, 128 et plus set all 
|Config_registers[3] =automode
|25 set all off
|26 Set all write
|27 set all latch
|28 set all read
|29 set all touch
|
|registre 0x04 = Recall_it mode
|	0x01 = Enable 
|	0x02 = snap shot	
| 	0x03 = save console
| 	0x04 = load console
|
|registre 0x06 = itb/otb MODE
|	0x00 = 0TB 
|	0x01 = ITB
|
|registre 0x09 = Fader scale mode 
| 	0=pt8_9_10_scale
| 	1=pt_11_scale/2=logic_scale
|
|	 
|registre 0x0b = Csruf mode MCU ou HUI 
|
|registre 0x0c = Bank slide 
|	 
*****************************************************************
****************************************************************/
void ConfigDataManager(unsigned char Reg_index,unsigned char Reg_data, unsigned char source)
{
if(Reg_index)
	Rx_Config_Data_Manager(Reg_index,Reg_data,source);		//Core config
Config_registers[0]=0;
}

/*******************************************************************************
*********************************************************************************
|FONCTIONS:void Rx_CORE_Config_Data_Manager(unsigned char reg_index,unsigned char reg_data)
|
|
*********************************************************************************
********************************************************************************/
char Rx_Config_Data_Manager(unsigned char reg_index,unsigned char reg_data ,unsigned char source)
{
unsigned char m,n;
if(reg_index)
	{
	switch(reg_index)
		{
		case 1:
		Config_registers[1]=reg_data;								/*reset le flag mais le dit aux autres********a GÉRER... ***********************************************/
		break;
		
		//si Config_registers[2] Set all automation to si 128(0x80) et +
		case 2:	break;
		case 3: if((source>0)&&(source<4))
					{
					if(Config_registers[2])
						ExtAutoModeManager(0,Config_registers[3],Config_registers[2]-1);	//set channel
					else
						ExtAutoModeManager(2,Config_registers[3],0);						//setall
					}
				return 0;	
				break;

//#4 RECALL-IT MODES *********************************************************************************************
		case 4:	switch(Config_registers[4])
					{	
					case 0x00:
						//Recall_mode=0;					//Recall_it OFF
						//Recall_it_flow_flag=0;
					break;
					case 0x01:							//Recall_it Enable update
						//Recall_mode=1;
						//Recall_it_flow_flag=0;
					break;
					case 0x02:							//Recall_it snapshot swt:25+26
						overflow=0;
						//Recall_mode=2;
						//Recall_it_flow_flag=0;			
					break;
					case 0x03:							//Recall_it bank mode swt:28+29
						overflow=0;
						//Recall_mode=3;
						//Recall_it_flow_flag=0;					
					break;
					case 0x04:							//Recall_it save
						overflow=0;
						//Recall_mode=2;
						//Recall_it_flow_flag=0;
					break;
					default:break;
					}
		break;
//****************************************************************************************************************************************
//NOTE:il faudra gérer les messages venant des masters #2 et #3 donc 32/64 et+
//****************************************************************************************************************************************
//Gestion du channel select par le Congifg[5]*********************************************************************************************
		case 5:

		break;

		//#6 OTB/Hybride MODE select
		case 6: if(Config_registers[6]&0x01)
					set_ITB_mode();
				//else
				//	set_OTB_mode();
		break;
		//#7 after STOP got to...
		case 7:
			Config_registers[7]=reg_data;	
		break;
		
		//#8 master or chan# 32 select 
		case 8:
		break;
		//#9 Fader scale select			0x00=ptools10 / 0x01=ptools11 / 0x02=ptools12+
		case 9:
			Config_registers[9]=reg_data;	
		break;
		case 10:
		break;

		//#11 C_surf driver select
		case 11:
				for (m=0;m<3;m++)
					for (n=0;n<4;n++)
						{
						if((Config_registers[11]&0x03)==3)
							UDPActiveChannelFlag[m][n]=0x00;
						else
							UDPActiveChannelFlag[m][n]=0xff;
						}
		break;
		//#12 bank slide
		case 0x0c:
				if (Config_registers[11]==0)				//Si on est en HUI
					if (Config_registers[6]&0x01)			//Et on a un bank slide...
						Do_Bank_Slide_HUI(reg_data,source);//Do bank slide function
		break;

		case 15:break;
		case 16:break;
		case 28:if(source==1)
					{
					TempVersion=(TempVersion&0x0000)|(Config_registers[28]<<8);
					return 0;
					}
				Config_registers[28]=0;
			break;
		case 29:if(source==1)
					{
					TempVersion=(TempVersion&0xff00)|(Config_registers[29]);
					Config_registers[29]=0;
					if((TempVersion&0xf000)==0x0000)
						SlaveVersion=TempVersion;
					else
					if((TempVersion&0xf000)==0x1000)
						MasterVersion=TempVersion;
					return 0;
					}
		break;
		default:break;
		}
	}
return 1;
}
//---------------------------------------------------------------
//---------------------------------------------------------------
// FONCTIONS:		unsigned char Get_System_Size(void)
// PreCondition:    None
// Input:           None
// Output:          None : system size
// Overview:        Determine la configuration du system TAI
// Notes:   
//---------------------------------------------------------------
//---------------------------------------------------------------
unsigned char Get_System_Size(void)
{
unsigned char sizeTest=0;
unsigned char maxchan=0;
unsigned char WaitCnt=10;

if(!SEKA_ONLINE_FLAG)
	{
	while(WaitCnt)
		{
		Delais(1000);
		if(WaitCnt)
			WaitCnt--;
		}
	}

Delais(5000);

if(SEKA_ONLINE_FLAG)
	{
	for(sizeTest=0;sizeTest<SYSTEMSIZE;sizeTest++)
		{
		if(rxFromSekaSwtData[sizeTest]!= 0xff)
			maxchan=sizeTest;
		}
	if(!maxchan||!SEKA_ONLINE_FLAG)
		{
		MAX_Core_Number=0;
		MAX_SLAVE=0;
		Config_registers[15]=MAX_SLAVE;		//System_size 1 based not 0 based...
		}
	else
		{
		MAX_Core_Number=(maxchan/32)+1;//(maxchan/32)+1;
		MAX_SLAVE=(maxchan/8)+1;
		Config_registers[15]=MAX_SLAVE;		//System_size 1 based not 0 based...
		}
	}
else
	{
	MAX_Core_Number=0;
	MAX_SLAVE=0;
	Config_registers[15]=0;
	}
return SEKA_ONLINE_FLAG;
}

/*******************************************************************
*******************************************************************
* Function:        unsigned char TrackLockedManager(unsigned char mode,unsigned char value)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:           Gestion du mode de track lock
*
*******************************************************************
*******************************************************************/
unsigned char TrackLockManager(unsigned char mode)
{
unsigned char n,m;
if(mode==1)
	{
	if(TrackLockedMode==0)
		{
		TrackLockedMode=1;
		for(m=0;m<3;m++)
			{
			for(n=0;n<4;n++)
				{
				GostTrackLockedState[m][n]=TrackLockedState[m][n];
				TrackLockedState[m][n]=0x00;
				}
			}
		return TrackLockedMode;
		}
	else
	if((TrackLockedMode==1)||(TrackLockedMode==2))

		{
		TrackLockedMode=0;
		for(m=0;m<3;m++)
			{
			for(n=0;n<4;n++)
				{
				GostTrackLockedState[m][n]=TrackLockedState[m][n];
				TrackLockedState[m][n]=0x00;
				}
			}
		return TrackLockedMode;
		}

	}
else
if(mode==2)
	{
	if(TrackLockedMode==2)
		{
		for(m=0;m<3;m++)
			{
			for(n=0;n<4;n++)
				{
				GostTrackLockedState[m][n]=TrackLockedState[m][n];
				TrackLockedState[m][n]=0x00;
				}
			}
		TrackLockedMode=0;
		}
	else
		TrackLockedMode=2;
	return TrackLockedMode;
	}
else
if(mode==3)
//clear track lock
	{
	TrackLockedMode=0;
	for(m=0;m<3;m++)
		{
		for(n=0;n<4;n++)
			{
			GostTrackLockedState[m][n]=0x00;
			TrackLockedState[m][n]=GostTrackLockedState[m][n];
			}
		}
	return 3;
	}
return 0;
}

/*******************************************************************
*******************************************************************
* Function:        void SaveAutoState(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:           Gestion du mode de track lock
*
*******************************************************************
*******************************************************************/
void SaveAutoState(void)
{
unsigned char m,s;
for(m=0;m<3;m++)
	for(s=0;s<4;s++)
		{
		}
}

/*******************************************************************
*******************************************************************
* Function:        void ClearDawCutState(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:           Gestion du mode de track lock
*
*******************************************************************
*******************************************************************/
void ClearDawCutState(void)
{
int m,s;
for(m=0;m<3;m++)
	for(s=0;s<4;s++)
		fromDawCutState[m][s]=0;
}

/*******************************************************************
*******************************************************************
* Function:        void LoadAutoState(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:           Gestion du mode de track lock
*
*******************************************************************
*******************************************************************/
void LoadAutoState(void)
{
unsigned char m,s,c;
for(m=0;m<3;m++)
	for(s=0;s<4;s++)
		for(c=0;c<8;c++)
			{
			//FADER_AUTOMODE_STATE[m][s][c]=savedSpiAutomodeBuffer[m][s][0]&CfgBitIndex[c];
		  	//FADER_AUTOMODE_STATE[m][s][c]=FADER_AUTOMODE_STATE[m][s][c]|((savedSpiAutomodeBuffer[m][s][1]&CfgBitIndex[c])<<1);				
			//FADER_AUTOMODE_STATE[m][s][c]=FADER_AUTOMODE_STATE[m][s][c]|((savedSpiAutomodeBuffer[m][s][2]&CfgBitIndex[c])<<2);
			}
}
