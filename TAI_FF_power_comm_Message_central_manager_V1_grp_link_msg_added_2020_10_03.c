//Fixer le channel up on recall, pour les systeme a 1 bank

#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
//#include "TAI_FF_power_comm_EIA485_functions_V1.h"
#include "TAI_FF_power_comm_EIA485_functions_V1.h"
#include "TAI_FF_power_comm_EIA232_functions_V1.h"
#include "TAI_FF_power_comm_EEPROM_Memory_fonctions.h"
#include "TAI_FF_power_comm_config_manager_fonctions.h"
#include "TAI_FF_power_comm_REAPER_Process_Functions.h"
#include "TAI_FF_power_comm_INJEKTOR_Process_Functions.h"
#include "TAI_FF_power_comm_global_functions.h"
#include "TAI_FF_globales_externes.h"
#include "Debug.h"
#include "TAI_FF_power_comm_Group_Link_manager_23_02_2020.h"

//Local globals
#define CANCEL_FLAG 0x0001
#define DOT_FLAG 0x0002
#define ENTER_FLAG 0x0004
#define RECALLIT 4
#define SSLKEYCODE 0x17
unsigned short KbdSwtFlags;
/*****************************************************
*le message transmit par les slave et accessoires
* est formé de:
* l'adresse de destination: x0101 (= master)
* l'adresse de source ex 0x0106 (LCD + Recall_it)
* le data number
* le code de crtl du message
* le data
ex: message type
0x0101
0x0106
0x06
0x50
0x12
0xff
****************************************************/

/****************************************************************

Adressages du système Tangerine automation

0x0100 globale talk to all
0x0101 MASTER
0x0102 MASTER
0x0103 MASTER
0x0104 RECALL_IT
0x0105 LCD
0x0106 LCD(+RECALL_IT)

0x0110 a 0x0117 motor faders

RECALL_IT
0x0104 0x0000 stop, Enable= on
0x0104 0x0001 start, Enable = off
0x0104 0x0002 Set Control Adresse

Codes des fonctions
LCD pages
0x0105 0x050 page 0
0x0105 0x051 page 1
0x0105 0x052 page 2
0x0105 0x053 page 3
0x0105 0x054 page 4

LCD pages+recalit
0x0106 0x050 page 0
0x0106 0x051 page 1
0x0106 0x052 page 2
0x0106 0x053 page 3
0x0106 0x054 page 4

0x0106 0x0000 stop, Enable= on
0x0106 0x0001 start, Enable = off
0x0106 0x0002 Set Control Adresse

*/

/******************************************************************************************
********************************************************************************************
|FONCTIONS:	void SSLKbdMsgParser(unsigned char Msg_data)
|Notes:	Gère le message venamt du remote
|		
|argument:Msg_data1= le code de la switch
|argument:Msg_data2= l'état de la switch
|			
|
*******************************************************************************************
*******************************************************************************************/
void SSLKbdMsgParser(unsigned char Msg_data)
{
unsigned short temp_flags;
unsigned char KbdMsgArray[3];
PushRaspSingleData(0x0d,Msg_data);
RaspMsgType=1;
if((Config_registers[11]&0x03)==0x03)
	{
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,SSLKEYCODE,0,0);								//transmet message complet
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,Msg_data>>1,0,0);						//Partie MSB du data entry
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_LSB,Msg_data&0x01,0,0);					//Partie LSB du data entry
	}

if(KbdSwtFlags)
	{
	//Section du clavier  QUERTY
	//section qui doit fonctionnere si on est en OOF ou ON
	switch(Msg_data)
		{
		case 0x30:	Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x40);		//0
					Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x00);break;	//
		case 0x31:	Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x41);		//1
					Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x01);break;	//
		case 0x32:	Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x43);		//2
					Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x03);break;	//
		case 0x33:	Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x46);		//3
					Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x06);break;	//
		case 0x34:	Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x42);		//4
					Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x02);break;	//
		case 0x35:	Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x44);		//5
					Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x04);break;	//
		case 0x36:	Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x47);		//6
					Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x07);break;	//
		case 0x37:	Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x40);		//7
					Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x00);break;	//
		case 0x38:	Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x41);		//8
					Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x01);break;	//
		case 0x39:	Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x42);		//9
					Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x02);break;	//
		}
	}

switch(Msg_data)
	{
	case 0x2e:	temp_flags=DOT_FLAG;
				if(!(KbdSwtFlags&temp_flags))
					KbdSwtFlags=KbdSwtFlags|temp_flags;
				else	
					KbdSwtFlags=KbdSwtFlags&~temp_flags;
				Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x45);			//DOT
				Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x05);break;		//

	case 0x20:	temp_flags=ENTER_FLAG;
				if(!(KbdSwtFlags&temp_flags))
					KbdSwtFlags=KbdSwtFlags|temp_flags;
				else	
					KbdSwtFlags=KbdSwtFlags&~temp_flags;
				Build_Midi_Msg(0,0,0,0,0x0f,0x14,0x2f,0x40);			//enter
				Build_Midi_Msg(0,0,0,0,0x0f,0x14,0x2f,0x00);
			break;

	case 0x2a:	temp_flags=DOT_FLAG;
				if(KbdSwtFlags&temp_flags)
					{
					Build_Midi_Msg(0,0,0,0,0x0f,0x1b,0x2f,0x47);		//CANCEL
					Build_Midi_Msg(0,0,0,0,0x0f,0x1b,0x2f,0x07);
					KbdSwtFlags=KbdSwtFlags&~temp_flags;
					}
				else
					KbdSwtFlags=0;
			break;
	}
if(Config_registers[10]&0x01)
	{
	switch(Msg_data)
		{
		//colonne 5
		case 0xa3:KbdMsgArray[0]=0x29;KbdMsgArray[1]=1;WWWMsgParser(KbdMsgArray);break;			//TRACK LOCK ON
		case 0x9c:KbdMsgArray[0]=0x29;KbdMsgArray[2]=2;WWWMsgParser(KbdMsgArray);break;			//TRACK LOCK SET
		case 0x95:KbdMsgArray[0]=0x29;KbdMsgArray[1]=3;WWWMsgParser(KbdMsgArray);break;			//TRACK LOCK CLEAR
		case 0x8e:KbdMsgArray[0]=0x21;KbdMsgArray[1]=0;WWWMsgParser(KbdMsgArray);break;			//LEVEL MATCH

		//web app commands
		// Rangée du BAS
		case 0x9f:KbdMsgArray[0]=0x37;WWWMsgParser(KbdMsgArray);break;				//FROM START
		case 0xa0:KbdMsgArray[0]=0x3a;WWWMsgParser(KbdMsgArray);break;				//To END
		case 0xa1:KbdMsgArray[0]=0x38;WWWMsgParser(KbdMsgArray);break;				//FROM HERE
		case 0xa2:KbdMsgArray[0]=0x3b;WWWMsgParser(KbdMsgArray);break;				//JOIN
		case 0xa4:KbdMsgArray[0]=0x3f;WWWMsgParser(KbdMsgArray);break;	 			//EXECUTE
	
		//Section automation preset
		case 0x98:	
		case 0x99:	KbdMsgArray[0]=0x22;KbdMsgArray[1]=1;WWWMsgParser(KbdMsgArray);break;			//PREVIEW OFF/WRITE
	
		case 0x9a:
		case 0x9b:	KbdMsgArray[0]=0x22;KbdMsgArray[1]=2;WWWMsgParser(KbdMsgArray);break;			//REVIEW OFF/READ
	
		case 0x91:	
		case 0x92:	KbdMsgArray[0]=0x22;KbdMsgArray[1]=3;WWWMsgParser(KbdMsgArray);break;			//UPDATE ABS READ/WRITE
	
		case 0x93:	
		case 0x94:	KbdMsgArray[0]=0x22;KbdMsgArray[1]=4;WWWMsgParser(KbdMsgArray);break;			//UPDATE ABS READ/LATCH 
	
		case 0x8a:		
		case 0x8b:	KbdMsgArray[0]=0x22;KbdMsgArray[1]=5;WWWMsgParser(KbdMsgArray);break;			//TRIM READ/WRITE
	
		case 0x8c:
		case 0x8d:	KbdMsgArray[0]=0x22;KbdMsgArray[1]=6;WWWMsgParser(KbdMsgArray);break;			//TRIM READ/LATCH

//		case 0xXX:
//		case 0xXX:	KbdMsgArray[0]=0x22;KbdMsgArray[1]=6;WWWMsgParser(KbdMsgArray);break;			//SCAN ALL/MODES

		}	
	}	

switch(Msg_data)
	{
	case 0x81:	KbdMsgArray[0]=0x20;WWWMsgParser(KbdMsgArray);break;						//Mix ON/OFF
	// Rangee du haut
	case 0x84:KbdMsgArray[0]=0x21;KbdMsgArray[1]=0x01;WWWMsgParser(KbdMsgArray);break;		//
	case 0x85:KbdMsgArray[0]=0x22;KbdMsgArray[1]=0x01;WWWMsgParser(KbdMsgArray);break;		//
	case 0x86:KbdMsgArray[0]=0x23;KbdMsgArray[1]=0x01;WWWMsgParser(KbdMsgArray);break;		//
	case 0x87:KbdMsgArray[0]=0x24;KbdMsgArray[1]=0x01;WWWMsgParser(KbdMsgArray);break;		//
	case 0x88:KbdMsgArray[0]=0x25;KbdMsgArray[1]=0x01;WWWMsgParser(KbdMsgArray);break;		//
	//case 0x89:KbdMsgArray[0]=0x36;WWWMsgParser(KbdMsgArray);break;						//


	case 0x28:	Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x42);			// SSL KBD = ( in SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x02);break;

	case 0x29:	Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x43);			// SSL KBD = ) OUT SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x03);break;

	case 0x3a:	Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x43);			// SSL KBD : =  LOOP SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x03);break;

	case 0x89:	Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x44);			// SSL KBD * = CLR SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x15,0x2f,0x04);break;		//

	case 0x96:	Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x41);			//LEFT SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x01);break;		//

	case 0x97:	Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x43);			//RIGHT SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x03);break;		//

	case 0x9d:	Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x40);			//DOWN SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x00);break;		//

	case 0x77:	Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x44);			//UP SWITCH
				Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x04);break;		//
	case 0x9e:	StopCommFlag=1;break;		//
	default:break;
	}

if((Msg_data!=0x2e)&&(Msg_data!=0x8b))
	{
	temp_flags=DOT_FLAG;
	if((KbdSwtFlags&temp_flags))
		{
		Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x45);		//DOT ou GOTO
		Build_Midi_Msg(0,0,0,0,0x0f,0x13,0x2f,0x05);		//
		KbdSwtFlags=KbdSwtFlags&~temp_flags;
		}
	}
}

//--------------------------------------------------------------------------
//	function:void midiConfigMsgManager(unsigned char idx,unsigned char data)
//
//
//
//--------------------------------------------------------------------------
void midiConfigMsgManager(unsigned char idx,unsigned char data)
{
unsigned char MsgArray[3];
MsgArray[0]=idx;
MsgArray[1]=data;
WWWMsgParser(MsgArray);
}

//--------------------------------------------------------------------------
//	function:void stateRequestManager(unsigned char idx,unsigned char data)
//
//
//
//--------------------------------------------------------------------------
void stateRequestManager(unsigned char idx,unsigned char data)
{

}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	void midiSendSingleConfig(unsigned char conFidx)
//Notes:	 envoy d'un config register par messages midi
//code du master
//arguments: slave_n
//Valeur retroun�es:void
//----------------------------------------------------------------
//----------------------------------------------------------------
void midiSendSingleConfig(unsigned char idx,unsigned char data)
{
Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,CONFIG_CODE,0,0);							//transmet message complet
Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,idx,0,0);									//transmet message complet
Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,(data&0x80)>>1,0,0);					//Partie MSB du data entry a 127
Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_LSB,data&0x7f,0,0);						//Partie MSB du data entry a 127
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	void midiSendGlobalConfig(void)
//Notes:	 envoi de tout les config registers par message midi
//code du master
//arguments: slave_n
//Valeur retroun�es:void
//----------------------------------------------------------------
//----------------------------------------------------------------
void midiSendGlobalConfig(void)
{
if(configCnt<32)
	{
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,CONFIG_CODE+1,0,0);										//transmet message complet
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_LSB,configCnt,0,0);											//transmet message complet
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,(Config_registers[configCnt]&0x80)>>1,0,0);		//Partie MSB du data entry a 127
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_LSB,Config_registers[configCnt]&0x7f,0,0);				//Partie MSB du data entry a 127
	ProcessOutput(UDP_CODE);
	configCnt++;	
	}
else
	configCnt=0;
}

/********************************************************************
* Function:        void WWWMsgParser(unsigned char MsgArray)

* PreCondition:    None
*
* Input:           None
*
* Output:          None
*
* Side Effects:    None
*
* Overview:        gestion des message en RX du port RS485 EXT
*                  
*                  
*
* Note:            None
*******************************************************************/
void WWWMsgParser(unsigned char MsgArray[])
{
unsigned char msgcode;
unsigned char index;
msgcode=MsgArray[0];
switch(msgcode)
	{
//-----------------------------------------------------------------------
// messages de "file manager" switch 1er rang	  						
//-----------------------------------------------------------------------
	case 0x10:						//
		break;
	case 0x11:						//
		break;
	case 0x12:						//
		break;
	case 0x13:						//
		break;
	case 0x14:						//
		break;
	case 0x15:						//
		break;


//-----------------------------------------------------------------------
// messages d'automation et mix controle 
//-----------------------------------------------------------------------
	case 0x20:															//MIX ON OFF
			if((Config_registers[10]&0x01)
				&&!gotoMixOnFlag)									//MIX OFF
				{
				if(!gotoMixOffFlag)
					gotoMixOffFlag=1;
				}
			else
			if(((USBStateFlag)&&((REAPER_FLAG)||(HUI_FLAG)||(UDP_FLAG)))//MIX ON
				&&!gotoMixOffFlag)
				gotoMixOnFlag=1;
		break;
	case 0x21:															//LEVEL MATCH
		if(Config_registers[24]&0x01)
			Config_registers[24]=Config_registers[24]&0xfe;
		else
			Config_registers[24]=Config_registers[24]|0x01;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x21,Config_registers[24]);
		PushRaspSingleData(0x21,Config_registers[24]);
		RaspMsgType=1;
		break;
//-------Les mode d'automation TOGGLE ------------------------------------------------------
	case 0x22:
		switch(MsgArray[1])
			{
			case 0:Config_registers[14]=(Config_registers[14]&0xf8)|0;break;//SCAN ALL
			case 1:Config_registers[14]=(Config_registers[14]&0xf8)|1;break;//PREVIEW OFF/WRITE
			case 2:Config_registers[14]=(Config_registers[14]&0xf8)|2;break;//REVIEW OFF/READ
			case 3:Config_registers[14]=(Config_registers[14]&0xf8)|3;break;//ABS READ/WRITE
			case 4:Config_registers[14]=(Config_registers[14]&0xf8)|4;break;//ABS READ/LATCH or TOUCH
			case 5:Config_registers[14]=(Config_registers[14]&0xf8)|5;break;//TRIM READ/WRITE
			case 6:Config_registers[14]=(Config_registers[14]&0xf8)|6;break;//TRIM READ/LATCH or TOUCH
			case 7:Config_registers[14]=(Config_registers[14]&0xf8)|7;break;//OFF
			default:break;
			}
														//PREVIEW OFF/WRITE
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x22,Config_registers[14]);
		PushRaspSingleData(MsgArray[0],Config_registers[14]);
		RaspMsgType=1;
		Push_Config_data_In_Queue(14,Config_registers[14],SRC_RASP);
		break;
	case 0x23:	
		if(!MsgArray[1])
			Config_registers[14]=(Config_registers[14]&0x0f)|0x00;
		else
		if(MsgArray[1]==1)
			Config_registers[14]=(Config_registers[14]&0x0f)|0x10;
		else
		if(MsgArray[1]==2)
			Config_registers[14]=(Config_registers[14]&0x0f)|0x20;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x22,Config_registers[14]);
		PushRaspSingleData(0x22,Config_registers[14]);
		RaspMsgType=1;
		Push_Config_data_In_Queue(14,Config_registers[14],SRC_RASP);
	break;

	case 0x24:
		switch(MsgArray[1])
			{
			case 1: Config_registers[6]^=0b00000001;break;
			case 2: Config_registers[6]^=0b00000010;break;
			case 3: Config_registers[6]^=0b00000100;break;
			case 4: Config_registers[6]^=0b00001000;break;
			}
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x24,Config_registers[6]);
		PushRaspSingleData(0x24,Config_registers[6]);
		RaspMsgType=1;
		Push_Config_data_In_Queue(6,Config_registers[6],SRC_RASP);	
	break;

	case 0x25:SendReaperCmd(0x10);
	break;
	case 0x26://change group create mode send return group creation state	
			MCA_groupSwtManager(MsgArray[1]&0x01);

	break;
	case 0x27://change link create mode send return group creation state
			MCA_linkSwtManager(MsgArray[1]&0x01);
	break;
	case 0x28:break;
//------------------------------------------------------------------------------------------
	case 0x29:															//TRACKLOCK
		Config_registers[20]=TrackLockManager(MsgArray[1]);
		Push_Config_data_In_Queue(20,Config_registers[20],SRC_RASP);	//Tx aux MASTERS
		if(Config_registers[20]==3)
			Config_registers[20]=0;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x29,Config_registers[20]);
		PushRaspSingleData(0x29,Config_registers[20]);
		RaspMsgType=1;
		// 0 = bypass lock track mode
		// 1 = Enable lock track mode
		// 2 = set track mode
		// 3 = Clear tracks
		break;

	case 0x2a:															//Auto SNAP mode
		Config_registers[22]^=0x80;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x2a,Config_registers[22]);
		PushRaspSingleData(0x2a,Config_registers[22]);
		RaspMsgType=1;
		Push_Config_data_In_Queue(22,Config_registers[22],SRC_RASP);
		break;

	case 0x2b:															//Rehearse
		Config_registers[22]^=0x40;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x2b,Config_registers[22]);
		PushRaspSingleData(0x2b,Config_registers[22]);
		RaspMsgType=1;
		Push_Config_data_In_Queue(22,Config_registers[22],SRC_RASP);
		break;

	case 0x2c:															//Write on stop
		Config_registers[22]^=0x20;
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x2c,Config_registers[22]);
		PushRaspSingleData(0x2c,Config_registers[22]);
		RaspMsgType=1;
		Push_Config_data_In_Queue(22,Config_registers[22],SRC_RASP);
		break;
	case 0x2f:break;
//--------------------------------------------------------------------------------------------
// messages d'automation et mix controle en mode Reaper 
//--------------------------------------------------------------------------------------------
	case 0x30:						//
		SendReaperCmd(0x10);
	break;
	case 0x31:						//
		SendReaperCmd(0x11);
		break;
	case 0x32:						//
		SendReaperCmd(0x12);
		break;
	case 0x33:						//
		SendReaperCmd(0x13);
		break;
	case 0x34:						//
		SendReaperCmd(0x34);
		break;
	case 0x35:						//
		SendReaperCmd(0x35);
		break;
	case 0x36:						//
		SendReaperCmd(0x36);
		break;
	case 0x37:						//Automation write from START
		SendReaperCmd(0x37);
		break;
	case 0x38:						//Automation write from HERE
		SendReaperCmd(0x38);
		break;
	case 0x39:						//Automation write To HERE
		SendReaperCmd(0x39);
		break;
	case 0x3a:						//Automation write to END
		SendReaperCmd(0x3a);		 
		break;
	case 0x3b:						//JOIN
		SendReaperCmd(0x3b);
		break;
	case 0x3c:						//Snap code
		break;
	case 0x3d:						//Rehears code
		break;
	case 0x3e:						//
		SendReaperCmd(0x3e);
		break;
	case 0x3f:						//Execute Code
		SendReaperCmd(0x3f);
		break;

//--------------------------------------------------------------------------------------------
//	0x40 a 0x4f Messages 
//--------------------------------------------------------------------------------------------
	case 0x40:
		break;
	case 0x41:
		break;
	case 0x42:
		break;
	case 0x43:						//
		break;
	case 0x44:						//
		break;
	case 0x45:						//
		break;
	case 0x46:						//
		break;
	case 0x47:						//
		break;
	case 0x48:						//
		break;
	case 0x49:						//
		break;
		
//--------------------------------------------------------------------------------------------
//	0x50 a 0x5f Messages de CONFIG 
//--------------------------------------------------------------------------------------------
	case 0x50:						//Set protocole 0= HUI 1/ HUI Hybrid 2=REAPER 3=UDP PLUGIN
		Config_registers[11]=(Config_registers[11]&0xfc)|MsgArray[1];
		Push_Config_data_In_Queue(11,Config_registers[11],SRC_RASP);//SSL mode
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x50,Config_registers[11]);
		PushRaspSingleData(0x50,MsgArray[1]);
		RaspMsgType=1;
		break;
	case 0x51:						//SET FADER SCALE = config_register[9]
		Push_Config_data_In_Queue(9,MsgArray[1]&0x03,SRC_RASP);
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x51,Config_registers[9]);
		PushRaspSingleData(0x51,MsgArray[1]);
		RaspMsgType=1;
		break;
	case 0x52:						//Libre
		break;
	case 0x53:						//SET ON WRITE-AFTER STOP = config_register[7]
		Push_Config_data_In_Queue(7,MsgArray[1],SRC_RASP);
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x53,Config_registers[7]);
		PushRaspSingleData(0x53,MsgArray[1]);
		RaspMsgType=1;
		break;
	case 0x54:
		break;
	case 0x55:
		break;
	case 0x56:
		break;
	case 0x57:						//Set faders and mutes on/off
		//if(!(Config_registers[10]&0x80))
			{
			if(MsgArray[1])
				Config_registers[8]=Config_registers[8]|0x02;
			else
				Config_registers[8]=Config_registers[8]&0xfd;
			Push_Config_data_In_Queue(8,Config_registers[8],SRC_RASP);
			if((Config_registers[11]&0x03)==3)
				midiSendSingleConfig(0x57,Config_registers[8]);
			PushRaspSingleData(0x57,MsgArray[1]);
			RaspMsgType=1;
			}
		break;
	case 0x58:break;
	case 0x59:break;
	case 0x5a:											//get Firmware
		index =MsgArray[1];								//Set requested single config register
		PushRaspSingleData(0x5a,40);					//request Power comm firmware version
		PushRaspSingleData(0x5b,Config_registers[40]);
		PushRaspSingleData(0x5c,Config_registers[41]);
		PushRaspSingleData(0x5a,41);					//request SEKA firmware version
		PushRaspSingleData(0x5b,Config_registers[42]);
		PushRaspSingleData(0x5c,Config_registers[43]);
		RaspMsgType=1;
		break;
	case 0x5b:
		break;
	case 0x5c:	
		break;
	case 0x5d:								//Refresh fader and state
		SessionChangeManager();
		break;
	case 0x5e:								//Config registers request
		if((Config_registers[11]&0x03)==3)
			configCnt=1;		
		break;

	case 0x5f:							//Config registers Save to eeprom
		//Save_Config_To_EEprom();
		Save_Config_To_EEprom_Flag=1;
		PushRaspSingleData(0x5f,MsgArray[1]);
		RaspMsgType=1;
		break;

//--------------------------------------------------------------------------------------------
//	0x60 a 0x6f Messages de Controle du TAI
//--------------------------------------------------------------------------------------------
	case 0x60:						//Global Automation Change mode from RASP
			Push_Config_data_In_Queue(2,0,0);
			Push_Config_data_In_Queue(3,MsgArray[1],SRC_RASP);
		break;
	case 0x61:						//Master fader Automation mode change from RASP
			if(Config_registers[21])
				{
				Push_Config_data_In_Queue(2,Config_registers[21],0);
				Push_Config_data_In_Queue(3,MsgArray[1],SRC_RASP);
				}
		break;
	case 0x62:						//Part 1 of Individual Automation mode change from RASP (set autochan)
			Autochan=MsgArray[1];
		break;
	case 0x63:						//Prat 2 of Individual Automation mode change from RASP (set automode)
			if(Autochan)
				{
				Push_Config_data_In_Queue(2,Autochan,0); 			//le canal
				Push_Config_data_In_Queue(3,MsgArray[1],SRC_RASP); 	//le mode
				}
		break;

	case 0x6c:	
		break;
	case 0x6d:													//SET GLOBAL: MOTOR OFF MODE "VIRTUAL..."
		if(MsgArray[1])					
			Config_registers[23]=Config_registers[23]|0x02;
		else
			Config_registers[23]=Config_registers[23]&0xfd;
		Push_Config_data_In_Queue(23,Config_registers[23],SRC_RASP);
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x6d,Config_registers[23]);
		PushRaspSingleData(0x6d,MsgArray[1]);
		RaspMsgType=1;
		break;
	case 0x6e:													//SET GROUP ACTIVE MODE 
		if(MsgArray[1])						
			Config_registers[23]=Config_registers[23]|0x04;
		else
			Config_registers[23]=Config_registers[23]&0xfb;
		Push_Config_data_In_Queue(23,Config_registers[23],SRC_RASP);
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x6e,Config_registers[23]);
		PushRaspSingleData(0x6e,MsgArray[1]);
		RaspMsgType=1;
		break;
	case 0x6f:													//SET REMOTE FROM GROUP MODE 
		if(MsgArray[1])						
			Config_registers[23]=Config_registers[23]|0x08;
		else
			Config_registers[23]=Config_registers[23]&0xf7;
		Push_Config_data_In_Queue(23,Config_registers[23],SRC_RASP);
		if((Config_registers[11]&0x03)==3)
			midiSendSingleConfig(0x6f,Config_registers[23]);
		PushRaspSingleData(0x6f,MsgArray[1]);
		RaspMsgType=1;
		break;

//--------------------------------------------------------------------------------------------
//	0x70 a 0x7f Messages de setup et Controle du TAI
//--------------------------------------------------------------------------------------------

	case 0x70:if(MFSetupModeReg)									//Trigger MF calibrate mode 
				MFSetupModeReg=0x00;
			else
				MFSetupModeReg=0x01;								//exit setup mode 
			Config_registers[25]=MFSetupModeReg;
			if((Config_registers[11]&0x03)==3)
				midiSendSingleConfig(0x70,Config_registers[25]);
			PushRaspSingleData(0x70,Config_registers[25]);
			RaspMsgType=1;
			Push_Config_data_In_Queue(25,Config_registers[25],SRC_RASP);
	
		break;
	case 0x71:
		break;
	case 0x72:
		break;
	case 0x73://lag buffer adjust)may not be implemented if Flying faders
			if(MsgArray[1]<96)
				{
				Config_registers[32]=MsgArray[1];								//Changer la valeur du lag buffer 
				Push_Config_data_In_Queue(32,Config_registers[32],SRC_RASP);
				PushRaspSingleData(0x73,Config_registers[32]);
				RaspMsgType=1;
				}
		break;
	case 0x74:																	//Trigger Fader pattern: Square 
			if((MsgArray[1]==1)||(!MsgArray[1]))
				{
				Config_registers[33]=MsgArray[1];								//Changer la resolution de sortie de DA 
				Push_Config_data_In_Queue(33,Config_registers[33],SRC_RASP);	//0 = 10bits<<2 ou full 12bits
				PushRaspSingleData(0x74,Config_registers[33]);
				RaspMsgType=1;
				}
		break;
	case 0x75:Config_registers[35]=MsgArray[1];									//Do fader offest scan sequence
			Push_Config_data_In_Queue(35,Config_registers[35],SRC_RASP);
		break;
	case 0x76:Config_registers[36]=MsgArray[1];									//enable disable fader motor offest
			Push_Config_data_In_Queue(36,Config_registers[36],SRC_RASP);
		break;
	case 0x77:Config_registers[37]=MsgArray[1];									//kill fader 
			Push_Config_data_In_Queue(37,Config_registers[37],SRC_RASP);
		break;
	case 0x7a:
		switch(MsgArray[1])
			{
			case 0:	
				passThroughFlag=1;
				passThroughMode=0;
				PushRaspSingleData(0x7a,0);
				RaspMsgType=1;
			break;
			case 1:
				passThroughFlag=1;
				passThroughMode=1;
				PushRaspSingleData(0x7a,1);
				RaspMsgType=1;
			break;
			}
	break;
	case 0x7c:	GotoBootloadFlag=0;
				PushRaspSingleData(0x7c,GotoBootloadFlag);
				RaspMsgType=1;
		break;
	case 0x7d:													//Bootloader section
		switch(MsgArray[1])
			{
			case 1:
				if(!GotoBootloadFlag)
					{
					gotoMixOffFlag=1;
					GotoBootloadFlag=1;							//Armer pour goto bootloader
					}
				else
					GotoBootloadFlag=0;
				PushRaspSingleData(0x7c,GotoBootloadFlag);
				RaspMsgType=1;
			break;
			case 0x10:
				if(!GotoBootloadFlag)							//Armer pour goto bootloader
					{
					gotoMixOffFlag=1;
					}
				else
					GotoBootloadFlag=0;
				PushRaspSingleData(0x7c,GotoBootloadFlag);
				RaspMsgType=1;
			break;
			}
	break;
	case 0x7e:	if(GotoBootloadFlag==1)
					GotoBootloadFlag=2;
				else
					GotoBootloadFlag=0;
				PushRaspSingleData(0x7c,GotoBootloadFlag);
				RaspMsgType=1;								
		break;
	case 0x7f:																	//Trigger pour un RESET
			Config_registers[63]=0x80;
			ResetTRIG=1;
			Push_Config_data_In_Queue(63,Config_registers[63],SRC_RASP);
		break;
//--------------------------------------------------------------------------------------------
//	0x80 a 0x8f section messages
//--------------------------------------------------------------------------------------------
	case 0x80:			//Trigger pour un stop RecalliT
	break;
	case 0x81:			//Trigger pour Strip RecalliT update 
	break;
	case 0x82:			//Trigger pour un BANK RecalliT Update
	break;
	case 0x83:			//Trigger pour un snapshot
	break;
	default:break;
	}
}

/******************************************************************************************
********************************************************************************************
|FONCTIONS:	void Manage_Automation_Msg(unsigned char Msg_data)
|Notes:	Gère le message venamt du remote
|		
|argument:Msg_data1= le code de la switch
|argument:Msg_data2= l'état de la switch
|			
|
*******************************************************************************************
*******************************************************************************************/
void Manage_Automation_Msg(unsigned char Msg_data)
{
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void HUIMsgParser(unsigned char data)
|Notes:	Gère le message venamt du remote
|		
|argument	:le data
|			
|
*****************************************************************
****************************************************************/
void HUIMsgParser(unsigned char data)
{
if((Config_registers[11]&0x03)==0x03)
	{
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,NRPN_MSB,SSLKEYCODE,0,0);								//transmet message complet
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_MSB,data>>1,0,0);							//Partie MSB du data entry
	Build_Midi_Msg(UDP_CODE,0,UDPMIDI_CABLE,1,DATA_ENTRY_LSB,data&0x01,0,0);						//Partie LSB du data entry
	}
switch(data)
	{
/*
	case 12:Build_Midi_Msg(0,0,0,0,0x0f,0x0F,0x2f,0x40);		//Send RTZ midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0F,0x2f,0x00);break;	//
	case 13:Build_Midi_Msg(0,0,0,0,0x0f,0x09,0x2f,0x43);		//Send LOCATE midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x09,0x2f,0x03);break;	//
	case 14:Build_Midi_Msg(0,0,0,0,0x0f,0x09,0x2f,0x43);		//Send  	midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x09,0x2f,0x03);break;	//
*/
	case 28:Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x42);		//IN
		Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x02);break;	//
	case 29:Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x43);		//OUT
		Build_Midi_Msg(0,0,0,0,0x0f,0x10,0x2f,0x03);break;	//

	case 0xe1:Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x41);		//Send REW midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x01);break;
	case 0xe2:Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x42);		//Send FFW midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x01);break;	//
	case 0xe3:Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x43);		//Send STOP midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x03);break;	//
	case 0xe5:Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x44);		//Send PLAY midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x04);break;	//
	case 0xe4:Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x45);		//Send PLAY+REC midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0E,0x2f,0x05);break;	//
	case 0xe6:Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x40);		//RTZ
		Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x00);break;	//
	case 0xe7:Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x40);		//to END
		Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x00);break;	//
//	case 0xe8:Build_Midi_Msg(0,0,0,0,0x0f,0x09,0x2f,0x43);		//Locate
//		Build_Midi_Msg(0,0,0,0,0x0f,0x09,0x2f,0x03);break;	//
	case 0xe9:Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x43);		//Loop
		Build_Midi_Msg(0,0,0,0,0x0f,0x0f,0x2f,0x03);break;	//
/*
	case 20:Build_Midi_Msg(0,0,0,0,0x0f,0x08,0x2f,0x48);		//Send save as midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x08,0x2f,0x08);break;	//
	case 21:Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x46);		//Send SHUTTLE midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x06);break;	//
	case 22:Build_Midi_Msg(0,0,0,0,0x0f,0x0e,0x2f,0x43);		//Send STOP midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0e,0x2f,0x03);break;	//
	case 23:Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x44);		//Send 2X midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x04);break;	//
	case 24:Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x43);		//Send 5X midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x03);break;	//
	case 25:Build_Midi_Msg(0,0,0,0,0x0f,0x0e,0x2f,0x44);		//Send PLAY midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0e,0x2f,0x04);break;	//
	case 27:Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x45);		//Send SHUTTLE WHEEL midi msg
		Build_Midi_Msg(0,0,0,0,0x0f,0x0d,0x2f,0x05);break;	//
*/
	default:break;
	}
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void SendReaperCmd(unsigned char cmd)
|Notes:	 Envoie une commde a REAPER
|
*****************************************************************
****************************************************************/
void SendReaperCmd(unsigned char cmd)
{
if((Config_registers[11]&0x03)==2)
	{
	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_MSB,CONTROL_CODE,0,0);				//transmet Code de Custom control codes
	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,NRPN_LSB,0,0,0);						//transmet le LSB du NRPN =0
	Build_Midi_Msg(TAI_CODE,0,VMIDI_CABLE,0,DATA_ENTRY_MSB,cmd,0,0);				//Automation "OFF" press
	}
}
//---------------------------------------------------------------------------------
//Fonction:	void SendTally(char message);
//
//
//
//---------------------------------------------------------------------------------
void SendTally(char code, char message)
{
PushRaspSingleData(0x80+code,message);
RaspMsgType=1;
}
