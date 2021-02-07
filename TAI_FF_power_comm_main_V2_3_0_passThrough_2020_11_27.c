const short CommVersion =0x2100;

#include "TAI_FF_Versionner.h"

#include "usb.h"
//#include "./spi.h"
#include "TAI_FF_power_comm_PIC24fj256_Hardware_Mapping.h"
#include "usb_function_midi.h"
#include "math.h"
#include "TAI_FF_globales.h"
#include "TAI_FF_power_comm_main.h"
#include "TAI_FF_power_comm_buffers.h"
#include "TAI_FF_power_comm_config_manager_fonctions.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
//#include "TAI_FF_power_comm_EIA485_functions_V1.h"
#include "TAI_FF_power_comm_EIA485_functions_V1.h"
#include "TAI_FF_power_comm_EIA232_functions_V1.h"
#include "TAI_FF_power_comm_EEPROM_Memory_fonctions.h"
#include "TAI_FF_power_comm_hui_Process_Fonctions.h"
#include "TAI_FF_power_comm_REAPER_Process_Functions.h"
#include "TAI_FF_power_comm_INJEKTOR_Process_Functions.h"
#include "TAI_FF_power_comm_global_functions.h"
#include "TAI_FF_power_comm_switch_manager_V1_0.h"
#include "Debug.h"
#include "globales_structures.h"
#include "bootloader_test.h"
#include "TAI_FF_passThroughMode.h"
 	/*
	TAI_Power_main2_2_2_udp_midi 09_01_2019 
		-MIDI NOTE ON-OFF FOR CHANNEL MESSAGE
	TAI_Power_main2_2_2_udp_midi 03_01_2019
		-UDP>--->CHANNEL ENABLE FLAG (sent to master on SPI message #12 MSB) needs master Frimware revision 2_2_2

	TAI_Power_main2_2_1_udp_midi 
		-UDP>--->MIDI communication comm code extended to include RW eneable bits
	
	TAI_Power_main2_2f_udp_midi 
		-UDP>-MIDI communication
		-Toggle automode OFF message_central modifaction
		-Config_registers[11] modif pour utiliser le upper nible pour les status de comm des modes du lower nible
------------------------------------------------------------------------------------
	TAI_Power_main2_2e 
		-Ajout du SoloCuteState pour gerer les cuts de groups si
		le le solo est utilisé donc associé au Grp 0
------------------------------------------------------------------------------------
	TAI_Power_main2_2 
		-Mute from DAW fixed channel 1 to 32 only limite
 		-Fixed: Return from Recall fader going every where
		-fixed: On Web server cold start empty web mix screen
		-Fixed(I think...)Recall snapshot system freeze
		-Fixed Keyboard dongle commuication stop on Recall
		-Fixed Multi switch selectal all mode limite to core zone
------------------------------------------------------------------------------------
	TAI_master_main Rev 2_0_5 RS232 4bytes packet
------------------------------------------------------------------------------------
	TAI_master_main Rev 2_0_4h_Buffer reworked
------------------------------------------------------------------------------------
	TAI_master_main Rev 2_0_4i_MF_WORK
		*Motor fader calibrate mode routine on TAI message 0x70,
		*Motor fader calibrate mode routine on TAI message 0x71
		*Motor fader calibrate mode routine on TAI message 0x72
		*automation snap mode on Config_registers[14] bit 7
		*automation Rehears mode on Config_registers[14] bit 6
------------------------------------------------------------------------------------
	TAI_master_main Rev 2_0_4i (sphere)(REV1204 sur pic)
		-some local/global variables cleanup (Initialise them in Init_var())
------------------------------------------------------------------------------------
	TAI_master_main Rev 2_0_3h (iguana) (rev1201 sur pic)
		*new USB buffer manager
		*New keyboard dongle comm (Debugged) 
------------------------------------------------------------------------------------
	TAI_master_main Rev 2_0_3g new code for pic 256...
------------------------------------------------------------------------------------
	save automation state on mix on/off
	move track lock to master-Comm SPI message #15
	V2_0_3a_ debug du problem de CORE et des MUTE/volume mal adressée en DAW ->SSL
	V2_0 Nouvelle version pour PCB V2; combine les deux assignation de "pin" pour les versions de pcb SVC et TAI V2
		mettre TAI_V2 en commenctaire pour assignation V2	
------------------------------------------------------------------------------------
	REV V2_0_3e setall_fix
------------------------------------------------------------------------------------
	Rev 2_7 SUBMASTER Avec le protocol TAI en version 1
------------------------------------------------------------------------------------
	Rev 2_6 tai Avec le protocol TAI en version 1
------------------------------------------------------------------------------------
	Rev 1_5s (32+)
			-Avec communication intermaster avec le SPI2 version du Submaster (spi = slave)
------------------------------------------------------------------------------------
	Rev 1_5 (32+)
			-Avec communicaiton intermaster avec le /SPI2
------------------------------------------------------------------------------------
	Rev 1_4 (LCD)
			-Communication avec le LCD (au 5ms)
			BUGs quelques drop de comm avec LCD
			-Recallit fonction sur LCD
------------------------------------------------------------------------------------
	Rev 1_3 ??? a documenter...
------------------------------------------------------------------------------------
	Version avec limite d"atténation a -118 db
	et max a +10db
	Rev 1__MCU_EEprom
			-Nouveau protocole de maping midi Mackie controle Univesal sur les volume, mute et automation mode select
			 compatbile avc Reaper MAC et PC, Bug avec Logigic
			-Fonciton d'écriture sur EEProm 25AA1024 save et load byte et page de 256 byte
				implanter dans le fichier "SVC_Comm_SPI_EEPROM_Memory_fonctions_Rev1.c"
	Rev 1.1 :SVC_main_comm_usb_full_interrupt_fonctionnel_
			 spi_TX_MIDI_BUFFER_
			 read_to_stop
			 Comm_fonction
			 Recall_it avec master 1_1_8b
	
	
	Rev 1.1a 	+spi pas en enhanced mode
				+Spi sans interrupt
				+"SSL Mute" transmit par fader down sur DAW
	
	Rev 1.1a1 	+Gestion des buffer USB par Interupt
	
	Rev:1 verion initiale
	*/
	/*****************************************************
	*le message transmit par les slave et accessoires
	* est formé de:
	* l'adresse de destination: x0101 (= master)
	* l'adresse de source ex 0x0106 (LCD + Recall_it)
	* l'adresse le code de crtl du message
	* le data number
	* le data
	****************************************************/
	/****************************************************************

	[1] = LIBRE
	//Config_registers[2] = Set automation index... 0 setall 
													//1 a 127 individual channels, 
													//128 a 139 bank set, 140 all auto mode index 
													//0x80= bank0; 0x81=bank1; 0x82= bank2;0x83= bank3; 
													//0x84= bank4; 0x85=bank5; 0x86= bank6;0x87= bank7;
													//0x88= bank8; 0x89=bank9; 0x8a= bank10;0x8b= bank11; 
													//0x90= Off
													//0x91= Write
													//0x92= Latch
													//0x93= Read
													//0x94=TRIM Off/LIVE
													//0x95=TRIM Write
													//0x96=TRIM Latch
													//0x97=TRIM Read
	//Config_registers[3] = 0x80 set all...	automode

	//Config_registers[4] = Recall_it mode
	//Config_registers[5] = Recall_it switch select
	
	//Config_registers[6] = libre
	//Config_registers[7] = On stop mode (0x00=nothing,0x01=READ,0x02=LATCH)
	//Config_registers[8] = Master relay bit 0x00= channel 32=32, 0x01 channel 32=master
	//Config_registers[9] = Fader_scaling_flag 0 =pt_legacy_scale, 1=pt11_scale
	//Config_registers[10] = Mix on off = 0x01 et interface modele 0x80=MF  0x00=VCA 0xc0 = FlyingFader
	
	//Config_registers[11] = Lower nible 0xf(x) Csurf mode  0=HUI 1= HUI HYBRIDE 2=TAI 3=UDP PLUGIN
							 upper nible communication status 0:off-line, 1:on-line 
							( UDP 0=Off-line  1 = UDP-HUB ON-Line  2- Plugin Online) 
	//Config_registers[12] = fader bank slide(1/0=up/down, lsb = slave number 0b00:0b01:0b10:0b11)
	//Config_registers[13] = LSB0-1=Group_focus for bank slide LSB 2-3 =group mask
	//Config_registers[14] = Automation status swtich Toggle modes

	//Config_registers[15] = System size (bank count) (MAX SLAVE NUMBER +1)
	//Config_registers[16] = VCA BANK LOCATION
	//Config_registers[17] = systeme status/Utility flags. 0x01 = save to eeprom
								0x01 = save to eeprom flag
								0x02 = USBSENS
								0x04 = USB CONFIGURED
								0x08 = DAW FLAG ONLINE 
	//Config_registers[18] = globalSwtlocation.
	//Config_registers[19] = MasterSwtlocation.
	//Config_registers[20] = Lock track state.
	//Config_registers[21] = Master Fader interface channel number
	//Config_registers[22] = Automation modifier modes(snap,rehears, write on stop,etc)  
	//Config_registers[23] = Motor fader mode
							0x01 =SET FORCE VCA MODE;
							0x02 =SET VIRTUAL MOTOR OFF MODE;
							0x04 =SET SET GROUP ACTIVE MODE;
							0x08 =SSET REMOTE FROM GROUP MODE 
		
	//Config_registers[24] = level match
	//Config_registers[25] = moving fader setup mode register
	//Config_registers[28] = SlaveVersion.
	//Config_registers[29] = MasterVersion.
	//Config_registers[30] = CommVersion.
	//Config_registers[31] = 
	//Config_registers[63] = RESET request
	liste des Config_registers lieés au sitch et externe controlers

	if single config registers (0x30)
	Config_registers[0x00->0x0c/(0->12)] track lock bits registers
	Config_registers[0x0d/(13)] SSL keybd key codes

	Config_registers[0x20/(32)] MIX/RUN ON-OFF
	Config_registers[0x21/(33)] LEVEL MATCH (SSL)
	Config_registers[0x22/(34)] Fader status modes (SSL)


	Config_registers[0x2b] configRegisters[22]	// REHEARSE mode
	Config_registers[0x2c] configRegisters[22] 	// WRITE ON STOP mode 1 = yes

											
	Config_registers[0x29] track lock mode
	Config_registers[0x2a] SNAP mode
											// Page setup
	Config_registers[0x50] configRegisters[11]
	Config_registers[0x51] configRegisters[9]	
	Config_registers[0x52] configRegisters[6] 
	Config_registers[0x53] configRegisters[7]
	Config_registers[0x54]
	Config_registers[0x55] : configRegisters[21]ak;											
	Config_registers[0x56] : configRegisters[16] = dataOutput.data[1];break;											
	Config_registers[0x57]	configRegisters[8] = configRegisters[8]|0x02;											
	Config_registers[0x5a] version des firmware registre #90
	Config_registers[0x5b]
	Config_registers[0x5c]
								
	Config_registers[0x5f]
	Config_registers[0x70]
	Config_registers[0x71]
	Config_registers[0x72] configRegisters[25] = dataOutput.data[1];break;
	Config_registers[0x7c] configRegisters[63]=(configRegisters[63]&0xfc)|(dataOutput.data[1]&0x03);
														bootloaderMode= dataOutput.data[1];
														console.log("Bootloader mode ="+bootloaderMode);


	//
	//Dans le code du master equivalent a la liste des variables de Ctrl_Registers[]:
	0;Reserver pour indiquer quel registre a changer par ctrl exterieur
	
	1:Stop flag from DAW/Ptools 0x01= stop detected ***** a faire 0x02= play detected
	
	2:ext automode controle index
	3:ext automode controle data
	
	4:Recall_it modes (auto_play/auto_rtz/auto_stop)= bit 4,5,6,7, (off/iddle-update/save/load)=bit 0,1,2,3
	5:Recall_it switch select
	6:OTB/ITB mode TOGGLE  (0x00==OTB 0x01==ITB)
	7:On stop auto write to read:On/Off (0x00=nothing,0x01=read,0x02=read trim)
	8:Master relay bit 0x00= channel 32=32 0x01 channel 32=master
	9:Fader_scaling_flag 0=pt11_scale,  1 =pt_legacy_scale 2=custom scale
	10:Fader_bank_slide msb=direction 1/0=up/down, 2lsb = slave number 0b00:0b01:0b10:0b11
	11:Csurf mode  0=MCU 1=HUI
	15 systeme size	info/Max slave
	16 master systeme size
	17 EXP1 systeme size
	18 EXP2 systeme size
	19 total systeme size
	
	Adressages du système Tangerine automation
	
	0x0100 globale talk to all
	0x0101 MASTER
	0x0102 SSL keyboard
	0x0103 Moving fader
	0x0104 RECALL_IT
	0x0105 LCD
	0x0106 LCD+RECALL_IT
	0x0107 LCD+SSL keyboard
	0x0108 LCD+SSL keyboard+RECALL_IT


	
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

//	mettre TAI_V2 en commenctaire pour assignation V2	

	

	#define MAIN_C
	//#define __SYSEX
	#define USB_INTERRUPT


//---------------------------------------------------------------
//---------------------------------------------------------------
// FONCTIONS:	int main(void)
//
// Overview:        Main program entry point.
//
//---------------------------------------------------------------
//---------------------------------------------------------------
int main(void)
{
unsigned char usb_bypass;
#ifdef __STANDALONE
usb_bypass=1;
#else
usb_bypass=0;
#endif
DORESET:if(ResetTRIG)
			__asm__ volatile("RESET");
RESTART:

mainLoopSlave=0;
MAX_SLAVE=12;
Init_io();
Init_var();
InitCfgRegBuffers();
InitPassThroughBuffer();
BootStateFlag=0;
Init_Timers();
SPI_EEPROM_Comm_Init();

BootStateFlag=1;		//SLAVE SCAN is done
Delais(500);			//Delais pour laisser le temps du scan des slaves

EIA485_Init();
StartEIA485Uart();
Delais(500);			//Delais pour laisser le temps du scan des slaves

Init_RASP_Uart1();
Delais(500);			//Delais pour laisser le temps du scan des slaves

Init_Timer2();
Delais(500);			//Delais pour laisser le temps du scan des slaves

BootStateFlag=2;		//EIA485 as been initalized
Delais(500);			//Delais pour laisser le temps du scan des slaves

InitializeSystem();
BootStateFlag=3;		//USB as been initialized
Delais(500);			//Delais pour laisser le temps du scan des slaves

BootStateFlag=4;		//SPI as been initialized
Stop_RASP_Uart1();
Init_RASP_Uart2();
Delais(500);			//Delais pour laisser le temps du scan des slaves

/************************************************/
//Si On harrive d'un cold start ou hardware reset
/************************************************/
if((!USBSENSE)&&(!deviceDetachFlag))
	{
	configured=0;
	deviceDetachFlag=1;
	USBDeviceDetach();														//Si cable USB pas actif ou branché roule en boucle ici
	BootStateFlag=6;
	}
/************************************************/
DoStatsBits();
BootStateFlag=5;
Delais(500);			//Delais pour laisser le temps du scan des slaves
LoadEEPROMConfigTAI();
Delais(500);			//Delais pour laisser le temps du scan des slaves
while(!Get_System_Size());
overflow=0;
mainLoopSlave=0;
mainLoopCore=0;
Delais(500);
BootStateFlag=6;
TriggerConfigToRasp();

while(1)
	{
	/*********************************************************/
	//Étape d'initialisation du USB
	/*********************************************************/
	if(USBSENSE)
		{
		DoStatsBits();
		prev_connected_state=connected;
		connected =1;														//On est branché... 
		while((USBSENSE)&&(!configured))								//Donc on se configure
			{
			USBDeviceAttach();
		    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1))	//Fait la sequence de connection USB
				configured=0;
			else
				configured=0x01;												
			BootStateFlag=7;
			}		
		DoStatsBits();
		}
	else
		configured=0;

	/*********************************************************/	
	//si c'est une reconncetion du USB
	//Vide le buffer Rx usb midi
	/*********************************************************/
	if((!prev_connected_state)&&(configured))								
		{
		if(Buffer_Over_Run&0x0001)								
			FlushUsbBuffer();
		}
	/****************************************************************/	
	// la séquence d'initialisation qui se fait au power on
	/****************************************************************/	
	mainLoopCore=0;
	mainLoopSlave=0;
	#ifndef __STANDALONE
	if((configured)&&(USBSENSE))
		USBStateFlag=1;
	else
		{
		configured=0;	
		USBStateFlag=0;
		Config_registers[10]=Config_registers[10]&0xfe;
		Push_Config_data_In_Queue(10,Config_registers[10],0);
		}
	#else
	USBStateFlag=1;
	#endif
	DoStatsBits();
//---------------------- MAIN LOOP -------------------------
	if(USBStateFlag)			
		{
		while((configured)
				&&(USBSENSE)
				&&(ResetTRIG<2500)
				&&(!passThroughMode))				
			{
			Overflow_Process();
			configured=ProcessInput();								//Gestion Rx USB
			ConfigTest();
			switchTester(mainLoopSlave,mainLoopCore);
			MixDataProcess(mainLoopCore,mainLoopSlave);
			EIA485MsgParser();
			if(!nextSlave(&mainLoopCore,&mainLoopSlave))
				{
				MCMASwitchTester();
				SaveToEEpromTest();
				}
			RefreshManager();
			if(GotoBootloadFlag==2)
				{
				if(!RaspMsgType)
					{
					killAllInt();
					asm volatile ("goto 0x0400");
					}
				}
			}
		DoStatsBits();
		}
	else
		{
		while((!configured)&&(!USBSENSE)&&(ResetTRIG<2500)&&(!passThroughMode))
			{			
			ConfigTest();
			EIA485MsgParser();
			if(!nextSlave(&mainLoopCore,&mainLoopSlave))
				SaveToEEpromTest();
			}
		DoStatsBits();
		}
	DoStatsBits();
//-----------------------------------
//Pass through mode even if no USB
//-----------------------------------
	while(passThroughMode)
		{
		Delais(1000);		//wait 1s for ACK message to be sent, it should be enough...
		passThroughMode=PTMain();
		}
	if(ResetTRIG)
		goto DORESET;
	Nop();
	goto RESTART;

	}
}
/********************************************************************
*******************************************************************
* Function:        void DoStatsBits(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:
*
*
*******************************************************************
*******************************************************************/
void DoStatsBits(void)
{
unsigned char temp;
temp=0;
if(USBSENSE)
	temp=0x01;
if(configured)
	temp=temp|0x02;
if(MidiActFlag)
	temp=temp|0x04;
if((Config_registers[11]&0x0f)<2)
	{
	if(HUI_FLAG)
		temp=temp|0x08;
	}
else
if((Config_registers[11]&0x0f)==2)
	{
	if(REAPER_FLAG)
		temp=temp|0x08;
	}
else
if((Config_registers[11]&0x0f)==3)
	{
	if(UDP_FLAG)
		temp=temp|0x08;
	}
stateBits=temp;
Config_registers[17]=(Config_registers[17]&0xf0)|stateBits;
}

/********************************************************************
*******************************************************************
* Function:        void MixDataProcess(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:
*
*
*******************************************************************
*******************************************************************/
void MixDataProcess(unsigned char Running_Core,unsigned char slave_numb)
{
if(gotoMixOnFlag)
	MixOnManager();
if(gotoMixOffFlag)
	MixOffManager();

if((Config_registers[11]&0x0f)<2)						
	Main_Process_Loop_HUI(Running_Core,slave_numb);			//Version HUI
else
if((Config_registers[11]&0x0f)==2)						
	REAPER_FF_Main_Process_Loop(Running_Core,slave_numb);			//Version TAI
if((Config_registers[11]&0x0f)==3)						
	Main_Process_Loop_UDP(Running_Core,slave_numb);			//Version UDP

#ifdef __SYSEX
SysexParser();
TcMsgParser();
#endif
LockTrackDispatcher(Running_Core,slave_numb);
}

/********************************************************************
*******************************************************************
* Function:        void LockTrackDispatcher(unsigned char slave_numb,unsigned char Running_Core)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:
*
*
*******************************************************************
*******************************************************************/
void LockTrackDispatcher(unsigned char Running_Core,unsigned char slave_numb)
{
if(LastTrackLockedState[Running_Core][slave_numb]!=TrackLockedState[Running_Core][slave_numb])
	{
	LastTrackLockedState[Running_Core][slave_numb]=TrackLockedState[Running_Core][slave_numb];
	PushRaspSingleData(slave_numb+(Running_Core*4),TrackLockedState[Running_Core][slave_numb]);
	RaspMsgType=1;
	}
}

/****************************************************************************************
*****************************************************************************************
|FONCTIONS:	void SysexParser(void)
|
|Notes:	Gerstion des message sysex
|		
|arguments 	:void
|			:void
|
*****************************************************************************************
****************************************************************************************/
void SysexParser(void)
{
unsigned char SyxestartFlag,SysexData;
unsigned char MsgActFlag, MmcMsg,MmcMsgType,MmcMsgLen;
if(NewSysexMsgFlag)
	{	
	MsgActFlag=1;
	MmcMsg=0;
	MmcMsgType=0;
	while((SysexBufferQuantity)&&(MsgActFlag))
		{
		SysexData=SysexBuffer[SysexBufferBot++];
		SysexBufferQuantity--;
		if(SysexBufferBot>=64)
			SysexBufferQuantity=0;
		if(SysexData==0xf0)
			{
			SysexIndex=1;	
			SyxestartFlag=1;
			}
		else
		if(SysexData==0xf7)
			{
			NewSysexMsgFlag--;
			SysexIndex=0;	
			MsgActFlag=0;
			}
		else
		switch(SysexIndex)
			{
			case 0x00:break;						//passe a travers les codes de MTC SYSEX message
			case 0x01:SysexIndex=2;break;			//Message de type realtime OU id du manifacturier 
			case 0x02:SysexIndex=3;break;			//id de l'unité
			case 0x03:	MmcMsg=SysexData;
						SysexIndex=4;break;			//<subid #1> Type de message MMC  ex:0x06 = command
	
			case 0x04:	MmcMsgType=SysexData;
						SysexIndex=5;break;			//<subid #2> Type de commande dans le cas que subid =0x06
	
			case 0x05:	if(MmcMsg==0x06)
							{
							if(MmcMsgType==0x44)
								MmcMsgLen=SysexData;
							}
						SysexIndex=6;
						break;
	
			default:if(MmcMsgType==0x44)
						{
						switch(SysexIndex)
							{
							case 6 :break;//code 0x01
							case 7 :TC_DATA[0]=SysexData;break;//hour Bit 0-4, TC format Bit 5-6
							case 8 :TC_DATA[1]=SysexData;break;//min
							case 9 :TC_DATA[2]=SysexData;break;//sec
							case 10 :TC_DATA[3]=SysexData;break;//frame
							case 11 :break;// fractionnal frame
							}
						}
					SysexIndex++;
					break;
			}
		}
	}
MsgActFlag=0;
}
	
/********************************************************************
*******************************************************************
* Function:        void SaveToEepromTest(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:
*
*******************************************************************
*******************************************************************/
void SaveToEEpromTest(void)
{
if(Save_Config_To_EEprom_Flag)
	{
	#ifdef __EEPROM					
	Save_TAI_Config();
	#endif
	Save_Config_To_EEprom_Flag=0;
	}
}
/********************************************************************
 * Function:        void Save_TAI_Config(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Init_var initialise les variables globales
 *
 * Note:            None
 *************************************************************/
void Save_TAI_Config(void)
{
char n;
//Memory_Save_Single(0,0xa5);
Save_Config_To_EEprom();
n=0;
}

/********************************************************************
*******************************************************************
* Function:        void Overflow_Process(void)
*
* PreCondition:    None
*
* Input:           None
*
* Output:          None

* Side Effects:    None
*	
* Notes:
*
*
*
*******************************************************************
*******************************************************************/
void Overflow_Process(void)
{
if(Buffer_Over_Run)
	Overflow_Process1();
}

/********************************************************************
*******************************************************************
* Function:        unsigned char FlushUsbBuffer(void)
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
unsigned char FlushUsbBuffer(void)
{
while(usb_buffer_cnt)
	{
	usb_buffer_cnt--;
	Parser_usb_buffer_number++;
	if(Parser_usb_buffer_number>=USBBUFFERS)
		Parser_usb_buffer_number=0;
	Push_Usb_To_Rx_Midi_Buffer(64,Parser_usb_buffer_number);
	}
//DebugClear(0x0001);
//Parser_usb_buffer_number=0;
//usb_buffer_cnt=0;
return 0;
}

/********************************************************************
*******************************************************************
* Function:        unsigned char ProcessInput(void)
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
unsigned char ProcessInput(void)
{
unsigned char TempUSBtop;

if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) //test le state de connection USB
	return 0;
TempUSBtop=Usb_buffer.top;
while(TempUSBtop!=Usb_buffer.bot)
	{
	Push_Usb_To_Rx_Midi_Buffer(64,Usb_buffer.bot);
	Usb_buffer.bot++;
	if(Usb_buffer.bot>=Usb_buffer.size)
		Usb_buffer.bot=0;
	}
return 0x01;
}
	
/********************************************************************
* Function:        void ProcessOutput(unsigned char Slave_num)
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
*******************************************************************/
unsigned char ProcessOutput(unsigned char Slave_num)
{
unsigned short ExitCnt;
unsigned short usb_quant;
ExitCnt=0;
if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) 
	return 0;
usb_quant=Get_Tx_Midi_Quant(Slave_num);
if(usb_quant)
	{
	USBSTATE=USBHandleBusy(USBTxHandle);
	if(!USBSTATE)
		{	
		while((Get_Tx_Midi_Quant(0))&&(configured))
			{
			USBSTATE=USBHandleBusy(USBTxHandle);
			if(!USBSTATE)
				{
				MidiActFlag=1;
				ExitCnt=0;
				Buffer_To_Usbmidi(Slave_num);
				USBTxHandle = USBTxOnePacket(MIDI_EP1,(BYTE*)&midiData,4);			
				if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) 
					return 0;
				}
			else
				ExitCnt++;
			if(ExitCnt>500)
				{
				ClearMidiTXBuffer();			
				MidiActFlag=0;
				if(configured)
					return 1;
				return 0;
				}	
			if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) 
				return 0;
			}
		if(!configured)
			return 0x00;
		}
	if(!configured)
		return 0x00;
	}
return 0x10;
}
	
/********************************************************************
* Function:        char nextSlave(unsigned char *Running_Core,unsigned char *slave_numb)
*
* PreCondition:    None
*
* Input:           None
*
* Output:          None
*
* Side Effects:    None
*
* Overview:        
*                  
*                  
* Note:            None
 *******************************************************************/
char nextSlave(unsigned char *Running_Core,unsigned char *slave_numb)
{
Real_Slave_Number++;
if(Real_Slave_Number>=MAX_SLAVE)
	{
	Real_Slave_Number=0;
	DoStatsBits();
	if((gotoMixOffFlag>1)&&(gotoMixOffFlag<5))
		gotoMixOffFlag++;
	if((gotoMixOnFlag>1)&&(gotoMixOnFlag<5))
		gotoMixOnFlag++;
	}

*slave_numb=Real_Slave_Number%4;
*Running_Core=Real_Slave_Number/4;

return Real_Slave_Number;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void RefreshManager(void)
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
void RefreshManager(void)
{
if(RefreshFlag)
	RefreshFlag--;
}

/****************************************************************
*****************************************************************
|FONCTION:	Push_Usbmidi_To_Rx_Midi_Buffer						|
|Notes:	recoit les donnée USB et met dans le bon buffer			|
|		circulaire												|
|arguments 	:pointeur vers structure de buffer					|
|			:pointeur de donnée a retourner						|
|																|
|																|
*****************************************************************
****************************************************************/
void Push_Usb_To_Rx_Midi_Buffer(WORD handlesize_index,unsigned char Usb_buffer_number1)
{
readflag=0;
//WORD handle_count;
unsigned USBDataIndex=0;
unsigned char temp_midi_data,data1,data2,data3;
signed char exit_code;
unsigned char TempCableNo;
unsigned char local_overflow,sysex_end_flag;
exit_code=0;
sysex_end_flag=0;
local_overflow=0;

while(!exit_code)
	{
	temp_midi_data=USBDataBuffer[Usb_buffer_number1][USBDataIndex];
	if((temp_midi_data&0x0f)==0x00)
		exit_code=1;		//sort car le buffer est vide
	else
		{
		TempCableNo=temp_midi_data>>4;
		data1=USBDataBuffer[Usb_buffer_number1][USBDataIndex+1];
		data2=USBDataBuffer[Usb_buffer_number1][USBDataIndex+2];
		data3=USBDataBuffer[Usb_buffer_number1][USBDataIndex+3];
		
		USBDataBuffer[Usb_buffer_number1][USBDataIndex]=0;
		//USBDataBuffer[Usb_buffer_number1][USBDataIndex+1]=0;
		//USBDataBuffer[Usb_buffer_number1][USBDataIndex+2]=0;
		//USBDataBuffer[Usb_buffer_number1][USBDataIndex+3]=0;
		USBDataIndex =USBDataIndex+4;	//si on est ici c'est pour tester le sysex

		if((temp_midi_data&0x0f)>0x07)// test pour code plus bas que sysex et autres inutilités
			{
			//testchannlvolume(0x14,data1,data2,data3);

			RxMidiBuffer.code[RxMidiBuffer.top] = temp_midi_data;		//sauve la valeur dans le buffer
			RxMidiBuffer.status[RxMidiBuffer.top] = data1;				//sauve la valeur dans le buffer
			RxMidiBuffer.data1[RxMidiBuffer.top] = data2;				//sauve la valeur dans le buffer
			RxMidiBuffer.data2[RxMidiBuffer.top] = data3;				//sauve la valeur dans le buffer

			RxMidiBuffer.top++;
			if (RxMidiBuffer.top >= RxMidiBuffer.size)					//test si on doit rouler au début
				RxMidiBuffer.top = 0;
			RxMidiBuffer.quantity++;
			if(RxMidiBuffer.quantity>RxMidiBuffer.Maxquantity)
				{	
				RxMidiBuffer.Maxquantity=RxMidiBuffer.quantity;
				//TC_DATA[0]=RxMidiMaxCnt&0x7f;
				//TC_DATA[1]=(RxMidiMaxCnt&0x3f80)>>7;
				}
			if(RxMidiBuffer.quantity>=RxMidiBuffer.size)
				DebugSet(0x0010);
			}
/*****************************************************************/
#ifdef __SYSEX
		else
		if(TempCableNo==4)
			{
			temp_midi_data=temp_midi_data&0x0f;
			if((temp_midi_data>3)&&(temp_midi_data<8))
				{
	// test le premiere message
				switch(data1)
					{
					case 0xf0:
						SysexBuffer[SysexBufferTop++]=data1;				//Data #1
						if(SysexBufferTop>=SysexBufferSize)
							SysexBufferTop=0;	
						SysexBuffer[SysexBufferTop++]=data2;				//Data #2
						if(SysexBufferTop>=SysexBufferSize)
							SysexBufferTop=0;	
						SysexBufferQuantity=SysexBufferQuantity+2;
						if(data2==0xf7)
							{
							NewSysexMsgFlag++;
							//SysexMsgQuantity++;
							}
						else
							{
							SysexBuffer[SysexBufferTop++]=data3;			//Data #3
							if(SysexBufferTop>=SysexBufferSize)
								SysexBufferTop=0;	
							SysexBufferQuantity++;
							if(data3==0xf7)
								{
								//SysexMsgQuantity++;
								NewSysexMsgFlag++;
								}
							}
					break;
					case 0xf7:
						SysexBuffer[SysexBufferTop++]=data1;				//Data #1
						if(SysexBufferTop>=SysexBufferSize)
							SysexBufferTop=0;	
						SysexBufferQuantity++;
						NewSysexMsgFlag++;
						//SysexMsgQuantity++;
					break;
					default:
						SysexBuffer[SysexBufferTop++]=data1;				//Data #1
						if(SysexBufferTop>=SysexBufferSize)
							SysexBufferTop=0;	
						SysexBuffer[SysexBufferTop++]=data2;				//Data #1
						if(SysexBufferTop>=SysexBufferSize)
							SysexBufferTop=0;	
						SysexBufferQuantity=SysexBufferQuantity+2;
						if(data2==0xf7)
							{
							NewSysexMsgFlag++;
							//SysexMsgQuantity++;
							}
						else
							{
							SysexBuffer[SysexBufferTop++]=data3;			//Data #3
							if(SysexBufferTop>=SysexBufferSize)
								SysexBufferTop=0;	
							SysexBufferQuantity++;
							if(data3==0xf7)
								{
								NewSysexMsgFlag++;
								//SysexMsgQuantity++;
								}
							}
					break;
					}
				if(SysexBufferQuantity>=64)
					DebugSet(0x2000);
				}
			else
			if(temp_midi_data==2)
				{
				if(data1== 0xf1)
					{
					TcBuffer[TcBufferTop++]=data2;				//Sauver le message TC quarter frame
					TcMsgQuantity++;
					if(TcBufferTop>=TcBufferSize)
						TcBufferTop=0;
					TcBufferQuantity++;
					if(TcBufferQuantity>=TcBufferSize)
						Buffer_Over_Run=Buffer_Over_Run|0x1000;
					}
				}	

			}
#endif
		//on ignore les autres messages et on flush
		}	
	if(USBDataIndex>=Usb_buffer.size)
		exit_code=1;
	};
}

//ici

/****************************************************************************************
*****************************************************************************************
|FONCTIONS:	unsigned char OTB_Midi_Parser_HUI_fader_n_Switch(unsigned char Core_Number)	|
|Notes:	retir une donnée dans un buffer circulaire										|
|		sans test d'overflow															|
|arguments 	:pointeur vers structure de buffer											|
|			:pointeur de donnée a retourner												|
|																						|
|DAW--->SSL																				|
*****************************************************************************************
****************************************************************************************/
unsigned char OTB_Midi_Parser_HUI_Fader_n_Switch(unsigned char Core_Number)
{
unsigned short temp_data;
unsigned char mute_bit,stopflag;
unsigned char master,slave;

Pull_Rx_Midi_Data();
if((RXmidi_msg.status==0xfc)||(RXmidi_msg.status==0xfb)||(RXmidi_msg.status==0xfa))
	{
	TransportModeChangeManager(RXmidi_msg.status);
	midi_parser_msg_count[RXmidi_msg.cable_num]=0;
	}
else
if(((RXmidi_msg.status&0xf0)==0x90)&&(RXmidi_msg.data1==0x00)&&(RXmidi_msg.data2==0x00))
	{
	Hui_On_Line_Manager(RXmidi_msg.cable_num);
	midi_parser_msg_count[RXmidi_msg.cable_num]=0;
	}
else
if(Config_registers[10]&0x01)
	{
	if (((RXmidi_msg.status&0xf0)==0xb0)&&(RXmidi_msg.data1<0x0f)&&(!midi_parser_msg_count[RXmidi_msg.cable_num]))				//c'est la premiere partie d'un message fader Move
		{
		midi_parser_msg_count[RXmidi_msg.cable_num]=1;					//Non, donc met le compteur de status a 1
		swt_data1[RXmidi_msg.cable_num]=RXmidi_msg.data1;				//Sauve la premiere section du messages data1 et... 
		swt_data2[RXmidi_msg.cable_num]=RXmidi_msg.data2;				//... data2
		}
	else
	if(((RXmidi_msg.status&0xf0)==0xb0)
		&&(midi_parser_msg_count[RXmidi_msg.cable_num]==1)
		&&(((RXmidi_msg.data1)>=0x20)&&((RXmidi_msg.data1)<=0x27)))							// C'est le deuxieme section d'un message de fader move
		{
		midi_parser_msg_count[RXmidi_msg.cable_num]=0;										//Reset le compte à 0
		if(swt_data1[RXmidi_msg.cable_num]<0x08)
			{
			temp_data=swt_data2[RXmidi_msg.cable_num];				
			temp_data=temp_data<<7;
			temp_data=temp_data|RXmidi_msg.data2;											//construit le message de VCA "pre" unscale_it

			master=DecodedCoreMatrix[0][RXmidi_msg.cable_num];
			slave=DecodedSlaveMatrix[0][RXmidi_msg.cable_num];
			if((!master)&&(!slave)&&(!swt_data1[RXmidi_msg.cable_num]))
				stopflag=1;
			DAW_IN_BUF[master][slave][swt_data1[RXmidi_msg.cable_num]]=temp_data;
			}			
		}
	else
	if(((RXmidi_msg.status&0xf0)==0xb0)
		&&(midi_parser_msg_count[RXmidi_msg.cable_num]==1)
		&&(RXmidi_msg.data1==0x2c)
		&&(swt_data1[RXmidi_msg.cable_num]==0x0c))											// C'est le deuxieme section d'un message de switch
		{
		midi_parser_msg_count[RXmidi_msg.cable_num]=0;										//Reset le compte à 0
		switch(RXmidi_msg.data2&0x07)
			{
			case 0x02:	
						master=DecodedCoreMatrix[0][RXmidi_msg.cable_num];
						slave=DecodedSlaveMatrix[0][RXmidi_msg.cable_num];
						if((!master)&&(!slave)&&(!swt_data1[RXmidi_msg.cable_num]))
							stopflag=1;

						mute_bit=BitIndex[swt_data2[RXmidi_msg.cable_num]];
						if((RXmidi_msg.data2&0xf0)==0x40)
							fromDawCutState[master][slave]=fromDawCutState[master][slave]|mute_bit;
						else
							fromDawCutState[master][slave]=fromDawCutState[master][slave]&~mute_bit;
			break;
			}
		}
	}
return 1;
}
	
/****************************************************************************************
*****************************************************************************************
|FONCTIONS:	unsigned char ITB_Midi_Parser_HUI_fader_n_Switch(unsigned char Core_Number)	|
|Notes:	retir une donnée dans un buffer circulaire										|
|		sans test d'overflow															|
|arguments 	:pointeur vers structure de buffer											|
|			:pointeur de donnée a retourner												|
|																						|
|																						|
*****************************************************************************************
****************************************************************************************/
unsigned char ITB_Midi_Parser_HUI_Fader_n_Switch(unsigned char Core_Number)
{
unsigned int temp_data;
unsigned char mute_bit;

Pull_Rx_Midi_Data();

if(((RXmidi_msg.status&0xf0)==0x90)&&(RXmidi_msg.data1==0x00)&&(RXmidi_msg.data2==0x00))
	Hui_On_Line_Manager(RXmidi_msg.cable_num);

if((RXmidi_msg.status==0xfc)||(RXmidi_msg.status==0xfb)||(RXmidi_msg.status==0xfa))
	{
	TransportModeChangeManager(RXmidi_msg.status);
	midi_parser_msg_count[RXmidi_msg.cable_num]=0;
	}
else
if(Config_registers[10]&0x01)
	{
	if ((RXmidi_msg.data1<0x08)&&(!midi_parser_msg_count[RXmidi_msg.cable_num]))				//c'est la premiere partie d'un message fader Move
		{
		midi_parser_msg_count[RXmidi_msg.cable_num]=1;			//Non, donc met le compteur de status a 1
		swt_data1[RXmidi_msg.cable_num]=RXmidi_msg.data1;		//Sauve la premiere section du messages data1 et... 
		swt_data2[RXmidi_msg.cable_num]=RXmidi_msg.data2;		//... data2
		}
	else
	if((midi_parser_msg_count[RXmidi_msg.cable_num==1])&&(((RXmidi_msg.data1)>0x1f)&&((RXmidi_msg.data1)<0x28)))		// C'est le deuxieme section d'un message de fader move
		{
		midi_parser_msg_count[RXmidi_msg.cable_num]=0;											//Reset le compte à 0
		if (swt_data1[RXmidi_msg.cable_num]<0x08)
			{
			mute_bit=BitIndex[swt_data1[RXmidi_msg.cable_num]];
			temp_data=swt_data2[RXmidi_msg.cable_num];				
			temp_data=temp_data<<7;
			temp_data=temp_data|RXmidi_msg.data2;															//construit le message de VCAC'est ici qu'on devrait faire le unscale_it
			DAW_IN_BUF[Core_Number][RXmidi_msg.cable_num][swt_data1[RXmidi_msg.cable_num]]=0x3020;		//VCA toujours a la même valeur 0db
			}
		}
	}
return 1;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//	FONCTIONS:	unsigned char Midi_Parser_TAI_Fader_n_Switch(void)						|
//																						|
//	Notes:	Recois les message midi en format REAPER									|
//	Arguments:																			|
//																						|
//																						|
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
unsigned char Midi_Parser_TAI_Fader_n_Switch(void)
{
unsigned char master,slave,chan;


Pull_Rx_Midi_Data();
if((RXmidi_msg.status==0xfc)||(RXmidi_msg.status==0xfb)||(RXmidi_msg.status==0xfa))
	TransportModeChangeManager(RXmidi_msg.status);
else
if((RXmidi_msg.code&0xf0)==0x40)
	{
	if(((RXmidi_msg.status&0xf0)==0xB0)&&(RXmidi_msg.data1==0x63)&&(RXmidi_msg.data2==0x7f))
		TAI_On_Line_Manager();
	else
	if(Config_registers[10]&0x01)		//DAW sensing et controle de transport actif meme si auotamtion = off
		{
		if(RXmidi_msg.status==0xb0)
			{
			switch(RXmidi_msg.data1)
				{
				/***************Gestion du NRPN MSB******************************/
				case 0x63:
					if(RXmidi_msg.data2<6)
						RX_LAST_NRPN_MSB=RXmidi_msg.data2;
					else
						RX_LAST_NRPN_MSB=-1;
					break;
				/***************Gestion du NRPN LSB******************************/
				case 0x62:
					if(RX_LAST_NRPN_MSB<6)
						{
						if(RXmidi_msg.data2<96)
							Running_channel=RXmidi_msg.data2;			//Dans le cas des codes 0,1,2 c'est le numéro de canal
						else
							RX_LAST_NRPN_MSB=-1;
						}
					else
						RX_LAST_NRPN_MSB=-1;
				break;
	
				/***************Gestion du DATA_ENTRY MSB******************************/
				case 0x06:
					switch(RX_LAST_NRPN_MSB)
						{
						case 0:midi_msg_temp_data=RXmidi_msg.data2<<7;break;						//c'est le MSB d'un DATA_ENTRY de volume
						//Mute
						case 1:
							master=DecodedCoreMatrix[Running_channel/32][(Running_channel/8)%4];
							slave=DecodedSlaveMatrix[Running_channel/32][(Running_channel/8)%4];
							chan=Running_channel%8;
							if(RXmidi_msg.data2)
								fromDawCutState[master][slave]=fromDawCutState[master][slave]|BitIndex[chan];			//mute "ON"
							else
								fromDawCutState[master][slave]=fromDawCutState[master][slave]&~BitIndex[chan];		//mute "OFF"
						break;	
						case 2:RX_LAST_NRPN_MSB=-1;
						break;											//c'est le MSB d'un DATA_ENTRY de D'AUTOMODE
						case 3:if(RXmidi_msg.data2==0x43)
								SessionChangeManager();RX_LAST_NRPN_MSB=-1;break;				//c'est le MSB d'un DATA_ENTRY d'un session changed
						case 5://TC_DATA[RX_LAST_NRPN_LSB]=RXmidi_msg.data2;
								RX_LAST_NRPN_MSB=-1;break;//c'est le MSB d'un DATA_ENTRY de TIME CODE
						default:RX_LAST_NRPN_MSB=-1;break;
						}				
					break;
				/***************Gestion du DATA_ENTRY LSB******************************/
				case 0x26:
						midi_msg_temp_data=midi_msg_temp_data|RXmidi_msg.data2;						//construit le message de VCA
						master=DecodedCoreMatrix[Running_channel/32][(Running_channel/8)%4];
						slave=DecodedSlaveMatrix[Running_channel/32][(Running_channel/8)%4];
						chan=Running_channel%8;
						DAW_IN_BUF[master][slave][chan]=midi_msg_temp_data;
				break;
				default:break;
				}
			}
		}
	}
return 1;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//	FONCTIONS:	unsigned char INJEKTOR_Midi_Parser(void)										|
//																						|
//	Notes:	Recois les message midi en format HUI										|
//	Arguments:																			|
//																						|
//																						|
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
unsigned char INJEKTOR_Midi_Parser(void)
{
unsigned char master,slave,chan,n;

Pull_Rx_Midi_Data();
if((RXmidi_msg.status==0xfc)||(RXmidi_msg.status==0xfb)||(RXmidi_msg.status==0xfa))
	TransportModeChangeManager(RXmidi_msg.status);
else
if((RXmidi_msg.code&0xf0)==0x40)
	{
	if((RXmidi_msg.status==0xB1)&&(RXmidi_msg.data1==0x63)&&(RXmidi_msg.data2>=0x7c))
		UDP_On_Line_Manager(1+(RXmidi_msg.data2-0x7c));
	else
		{
		switch(RXmidi_msg.status)
			{
			case 0xb1:
				switch(RXmidi_msg.data1)
					{
					/***************Gestion  du NRPN MSB******************************/
					case 0x63:
						if(RXmidi_msg.data2<0x30)
							RX_LAST_NRPN_MSB=RXmidi_msg.data2;
						else
							RX_LAST_NRPN_MSB=-1;
						break;
					/***************Gestion du NRPN LSB******************************/
					case 0x62:
						switch(RX_LAST_NRPN_MSB)
							{
							case 0x1f:
							case 0x20:
									Running_channel=RXmidi_msg.data2;
							break;
							default:
								if(RXmidi_msg.data2<96)
									Running_channel=RXmidi_msg.data2;			//Dans le cas des codes 0,1,2 c'est le numéro de canal
								else
									RX_LAST_NRPN_MSB=-1;
							break;
							}
					break;
		
					/***************Gestion du DATA_ENTRY MSB******************************/
					case 0x06:
						switch(RX_LAST_NRPN_MSB)
							{
							case 0x10:midi_msg_temp_data=RXmidi_msg.data2<<7;							//c'est le MSB d'un DATA_ENTRY de volume
							break;																		//Mute
							case 0x11:
								if(Config_registers[10]&0x01)		//DAW sensing et controle de transport actif meme si automation = off
									{
									master=Running_channel/32;
									slave=(Running_channel/8)%4;
									chan=Running_channel%8;
									if(RXmidi_msg.data2)
										fromDawCutState[master][slave]=fromDawCutState[master][slave]|BitIndex[chan];			//mute "ON"
									else
										fromDawCutState[master][slave]=fromDawCutState[master][slave]&~BitIndex[chan];		//mute "OFF"
									}
								RX_LAST_NRPN_MSB=-1;
							break;	
							case 0x12:RX_LAST_NRPN_MSB=-1;
							break;																		//c'est le MSB d'un DATA_ENTRY de D'AUTOMODE
							case 0x13:
								if(Config_registers[10]&0x01)											//DAW sensing et controle 
									{																	//de transport actif meme si auotamtion = off
									if(RXmidi_msg.data2==0x43)
										SessionChangeManager();
									}	
								RX_LAST_NRPN_MSB=-1;
							break;																		//c'est le MSB d'un DATA_ENTRY d'un session changed
							case 0x14:
									master=Running_channel/32;
									slave=(Running_channel/8)%4;
									chan=Running_channel%8;
									if(RXmidi_msg.data2)
										UDPActiveChannelFlag[master][slave]=UDPActiveChannelFlag[master][slave]|BitIndex[chan];
									else
										UDPActiveChannelFlag[master][slave]=UDPActiveChannelFlag[master][slave]&~BitIndex[chan];
									
							break;
							case 0x15:																	//TC_DATA[RX_LAST_NRPN_LSB]=RXmidi_msg.data2;
									RX_LAST_NRPN_MSB=-1;
							break;																		//c'est le MSB d'un DATA_ENTRY de TIME CODE
							case 0x18:	master=Running_channel/32;
										slave=(Running_channel/8)%4;
										chan=Running_channel%8;
										for(n=0;n<4;n++)
											{
											if(RXmidi_msg.data2&BitIndex[n])
												{		//0=READ CUTS 1= VOL READ 2= CUT WRITE 4=VOL WRITE
												switch(n)
													{
													case 0:enableReadCutState[master][slave]=enableReadCutState[master][slave]|BitIndex[chan];break;
													case 1:enableReadVolState[master][slave]=enableReadVolState[master][slave]|BitIndex[chan];break;
													case 2:enableWriteCutState[master][slave]=enableWriteCutState[master][slave]|BitIndex[chan];break;
													case 3:enableWriteVolState[master][slave]=enableWriteVolState[master][slave]|BitIndex[chan];break;
													}
												}	
											else
												{		//0=READ CUTS 1= VOL READ 2= CUT WRITE 4=VOL WRITE
												switch(n)
													{
													case 0:enableReadCutState[master][slave]=enableReadCutState[master][slave]&~BitIndex[chan];break;
													case 1:enableReadVolState[master][slave]=enableReadVolState[master][slave]&~BitIndex[chan];break;
													case 2:enableWriteCutState[master][slave]=enableWriteCutState[master][slave]&~BitIndex[chan];break;
													case 3:enableWriteVolState[master][slave]=enableWriteVolState[master][slave]&~BitIndex[chan];break;
													}
												}	
											}
										motorManager(Running_channel);
							break;
							case 0x1f:																	//TC_DATA[RX_LAST_NRPN_LSB]=RXmidi_msg.data2;
									midi_msg_temp_data=RXmidi_msg.data2<<1;
							break;
							case 0x20:																	//TC_DATA[RX_LAST_NRPN_LSB]=RXmidi_msg.data2;
									midi_msg_temp_data=RXmidi_msg.data2<<1;
							break;
							default:
									RX_LAST_NRPN_MSB=-1;
							break;
							}				
						break;
					/***************Gestion du DATA_ENTRY LSB******************************/
					case 0x26:
							switch(RX_LAST_NRPN_MSB)
							{
							case 0x10:
								midi_msg_temp_data=midi_msg_temp_data|RXmidi_msg.data2;						//construit le message de VCA
								master=Running_channel/32;
								slave=(Running_channel/8)%4;
								chan=Running_channel%8;
								if(midi_msg_temp_data&0x0400)
									ChanLinkState[master][slave]=ChanLinkState[master][slave]|BitIndex[chan];
								else    
									ChanLinkState[master][slave]=ChanLinkState[master][slave]&~BitIndex[chan];

								DAW_IN_BUF[master][slave][chan]=midi_msg_temp_data&0x03ff;
								RX_LAST_NRPN_MSB=-1;
							break;
							case 0x1f:
								if((Running_channel)&&(Running_channel<64))
									{
									Config_registers[Running_channel]=midi_msg_temp_data|RXmidi_msg.data2;
									Push_Config_data_In_Queue(Running_channel,Config_registers[Running_channel],0);
									}
								RX_LAST_NRPN_MSB=-1;
							break;
							case 0x20:
								midiConfigMsgManager(Running_channel,midi_msg_temp_data|RXmidi_msg.data2);
								RX_LAST_NRPN_MSB=-1;
							break;
							case 0x21:
								stateRequestManager(Running_channel,midi_msg_temp_data|RXmidi_msg.data2);
								RX_LAST_NRPN_MSB=-1;
							break;
							default: RX_LAST_NRPN_MSB=-1;break;
							}
					default:break;
					}
				break;
			case 0x98:
					Running_channel=RXmidi_msg.data1;
					midi_msg_temp_data=RXmidi_msg.data2<<7;
			break;
			case 0x88:
					midi_msg_temp_data=midi_msg_temp_data|RXmidi_msg.data2;						//construit le message de VCA
					master=Running_channel/32;
					slave=(Running_channel/8)%4;
					chan=Running_channel%8;	
					if(midi_msg_temp_data&0x1000)
						ChanLinkState[master][slave]=ChanLinkState[master][slave]|BitIndex[chan];
					else    
						ChanLinkState[master][slave]=ChanLinkState[master][slave]&~BitIndex[chan];
					if(midi_msg_temp_data&0x2000)
						UDPActiveChannelFlag[master][slave]=UDPActiveChannelFlag[master][slave]|BitIndex[chan];
					else
						UDPActiveChannelFlag[master][slave]=UDPActiveChannelFlag[master][slave]&~BitIndex[chan];
					if(midi_msg_temp_data&0x0400)
						fromDawCutState[master][slave]=fromDawCutState[master][slave]|BitIndex[chan];			//mute "ON"
					else
						fromDawCutState[master][slave]=fromDawCutState[master][slave]&~BitIndex[chan];		//mute "OFF"

					DAW_IN_BUF[master][slave][chan]=midi_msg_temp_data&0x03ff;
			break;
			default:	RX_LAST_NRPN_MSB=-1;
			break;
			}
		}
	}
return 1;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//	Function:        unsigned char Buffer_To_Usbmidi(unsigned char Buffer_num)
//	PreCondition:    None
//	Input:           pointeur vers structure de buffer
//	Output:          note used
//	Side Effects:    None
//	
//Overview:			Retir une donnée d'un buffer circulaire et transmet
//  				en USB
// Note:            None
//----------------------------------------------------------------
//----------------------------------------------------------------
unsigned char Buffer_To_Usbmidi(unsigned char Buffer_num)
{
Pull_Tx_Midi_Data(4,Buffer_num);	
midiData.Val = 0;
midiData.CableNumber = TXmidi_msg.cable_num;
midiData.CodeIndexNumber = TXmidi_msg.code;
midiData.DATA_0 = TXmidi_msg.status;			//Control change
midiData.DATA_1 = TXmidi_msg.data1;			//Zone selecte
midiData.DATA_2 = TXmidi_msg.data2;			//Zone number
return 1;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	void Build_Midi_Msg(unsigned char Zone, unsigned char Port,
//								unsigned char Data1, unsigned char Data2,
//								unsigned char Channel_Num ,unsigned char Slave_Num)
//
//Notes:	Recois un message et envoie une acuse reception
//		DA
//Codes de sortie
//	1 OK
//
//	-1	timeout
//	-2 buffer avec plus de 1
//----------------------------------------------------------------
//----------------------------------------------------------------
void Build_Midi_Msg(unsigned char mode, unsigned char message_type,
					unsigned char Slave_Num,unsigned char chan, 
					unsigned char data_1,
					unsigned char data_2,
					unsigned char data_3,
				 	unsigned char data_4)
{
if(configured)
	{
	if(mode==0)
		{
		TXmidi_msg.code= MIDI_CIN_CONTROL_CHANGE;		//
		TXmidi_msg.status=0xB0;							//type de message midi
		TXmidi_msg.data1=data_1;
		TXmidi_msg.data2=data_2;						//Select ZONE ex:Chanel 0x04 = 04
		TXmidi_msg.cable_num=Slave_Num;
		Push_Tx_Midi_Data(4,Slave_Num);
	
		TXmidi_msg.code= MIDI_CIN_CONTROL_CHANGE;		//sauve la valeur dans le buffer
		TXmidi_msg.status=0xB0;	
		TXmidi_msg.data1=data_3;
		TXmidi_msg.data2=data_4;						//Select ZONE ex:Chanel 0x04 = 04
		TXmidi_msg.cable_num=Slave_Num;
		Push_Tx_Midi_Data(4,Slave_Num);
		}
	else
	if(mode==1)
		{
		if (message_type==0)
			{
			TXmidi_msg.code= MIDI_CIN_NOTE_ON;
			TXmidi_msg.status=0x90+chan;						//type de message midi
			TXmidi_msg.data1=data_1;
			TXmidi_msg.data2=data_2;						//Data2	
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
			}
		else
		if (message_type==1)	
			{
			TXmidi_msg.code= MIDI_CIN_NOTE_OFF;
			TXmidi_msg.status=0x80+chan;						//type de message midi
			TXmidi_msg.data1=data_1;
			TXmidi_msg.data2=data_2;						//Data2	
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
			}
		else	
		if	(message_type==2)
			{
			TXmidi_msg.code= MIDI_CIN_PITCH_BEND_CHANGE;
			TXmidi_msg.status=0xE0+chan;						//type de message midi
			TXmidi_msg.data1=data_1;
			TXmidi_msg.data2=data_2;						//Data2	
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
			}
		else
		if	(message_type==3)
			{
			TXmidi_msg.code= MIDI_CIN_NOTE_ON;
			TXmidi_msg.status=0x90+chan;						//type de message midi
			TXmidi_msg.data1=data_1;
			TXmidi_msg.data2=data_2;							//Data2	
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
		
			TXmidi_msg.code= MIDI_CIN_NOTE_ON;
			TXmidi_msg.status=0x90+chan;						//type de message midi
			TXmidi_msg.data1=data_3;
			TXmidi_msg.data2=data_4;							//Data2	
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
				
			TXmidi_msg.code= MIDI_CIN_NOTE_ON;
			TXmidi_msg.status=0x90+chan;						//type de message midi
			TXmidi_msg.data1=data_1;
			TXmidi_msg.data2=data_4;							//Data2	
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
			}
		}
	else
	if(mode==2)
		{
		TXmidi_msg.code= MIDI_CIN_SINGLE_BYTE;
		TXmidi_msg.status=data_1;						//type de message midi
		TXmidi_msg.data1=0;
		TXmidi_msg.data2=0;							//Data2	
		TXmidi_msg.cable_num=Slave_Num;
		Push_Tx_Midi_Data(4,Slave_Num);
		}
	else
	if(mode==3)											//mode de message pour le TAI
		{
		if	(message_type==0)
			{
			TXmidi_msg.code= MIDI_CIN_CONTROL_CHANGE;		//Dit que c'est un message de controle pour le NRPN
			TXmidi_msg.status=0xB0+chan;	
			TXmidi_msg.data1=data_1;
			TXmidi_msg.data2=data_2;						//Select ZONE ex:Chanel 0x04 = 04
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
			}
		else
		if	(message_type==1)
			{
			TXmidi_msg.code= MIDI_CIN_2_BYTE_MESSAGE;		//dit que c'est une message en running status
			TXmidi_msg.status=0xB0+chan;	
			TXmidi_msg.data1=data_1;
			TXmidi_msg.data2=data_2;						//Select ZONE ex:Chanel 0x04 = 04
			TXmidi_msg.cable_num=Slave_Num;
			Push_Tx_Midi_Data(4,Slave_Num);
			}
		}
	}
}

/****************************************************************************************
*****************************************************************************************
|FONCTIONS:	void TcMsgParser(void)
|
|Notes:	Geston s mesage midi Timecode
|		
|arguments 	:void
|			:void
|
*****************************************************************************************
****************************************************************************************/
void TcMsgParser(void)
{
unsigned char TcData,TcIndex;
while((TcMsgQuantity)&&(TcBufferQuantity))
	{
	TcData=TcBuffer[TcBufferBot++];
	if(TcBufferBot>=TcBufferSize)
		TcBufferBot=0;
	TcBufferQuantity--;
	TcMsgQuantity--;
	TcIndex=TcData>>4;
	switch(TcIndex)
		{
		case 0:TC_DATA[0]=(TC_DATA[0]&0x70)|(TcData&0x0f);break;
		case 1:TC_DATA[0]=(TC_DATA[0]&0x0f)|TcData<<4;break;
		case 2:TC_DATA[1]=(TC_DATA[1]&0x70)|(TcData&0x0f);break;
		case 3:TC_DATA[1]=(TC_DATA[1]&0x0f)|TcData<<4;break;
		case 4:TC_DATA[2]=(TC_DATA[2]&0x70)|(TcData&0x0f);break;
		case 5:TC_DATA[2]=(TC_DATA[2]&0x0f)|TcData<<4;break;
		case 6:TC_DATA[3]=(TC_DATA[3]&0x70)|(TcData&0x0f);break;
		case 7:TC_DATA[3]=(TC_DATA[3]&0x0f)|((TcData&0x01)<<4);break;
		}
	}
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//FONCTIONS:	void Hui_On_Line_Manager(unsigned char Slave_num)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
//	
//Overview:        This function is a place holder for other user
//                 routines. It is a mixture of both USB and
//                 non-USB tasks.
// Note:            None
//----------------------------------------------------------------
//----------------------------------------------------------------
void Hui_On_Line_Manager(unsigned char cable_num)
{
TXmidi_msg.code= MIDI_CIN_NOTE_ON;		//sauve la valeur dans le buffer
TXmidi_msg.status= 0x90;				//sauve la valeur dans le buffer
TXmidi_msg.data1= 0;					//sauve la valeur dans le buffer
TXmidi_msg.data2 =0x7f;					//sauve la valeur dans le buffer
TXmidi_msg.cable_num =cable_num;		//sauve la valeur dans le buffer

Push_Tx_Midi_Data(4,cable_num);

DAW_on_line_cntr[cable_num]=0x04;
DAW_on_line_flag[cable_num]=0x01;

}

/****************************************************************
*****************************************************************
|FONCTIONS:	Init_Midi_Msg										|
|Notes:	retir une donnée dans un buffer circulaire				|
|		sans test d'overflow									|
|arguments 	:pointeur vers structure de buffer					|
|			:pointeur de donnée a retourner						|
|																|
|																|
*****************************************************************
****************************************************************/
void Init_Midi_Msg(unsigned char n)
{
RXmidi_msg.code=0;
RXmidi_msg.status=0;
RXmidi_msg.data1=0;
RXmidi_msg.data2=0;
RXmidi_msg.cable_num=0;
RXmidi_msg.msg_count=0;
RXmidi_msg.data_number=0;

TXmidi_msg.code=0;
TXmidi_msg.status=0;
TXmidi_msg.data1=0;
TXmidi_msg.data2=0;
TXmidi_msg.cable_num=0;
TXmidi_msg.msg_count=0;
TXmidi_msg.data_number=0;
}

//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
// SECTION du "merge" des FONCTIONS avec le NON USB
//*******************************************************************************************************************

/********************************************************************
 * Function:        Init_var(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Init_var initialise les variables globales
 *
 * Note:            None
 *******************************************************************/
/************************************************/
// Fonction init des variables
/************************************************/
unsigned char Init_var(void)
{	
unsigned short j,k,m,n;

for (j=0;j<12;j++)
	{
	ledFlashState1[j]=0x00;
	ledFlashState2[j]=0x00;
	
	}

for (j=0;j<3;j++)
	{
	for (m=0;m<4;m++)
		{
		FF_WasTouchedState[j][m]=0x00;
//		ToMF_VCAStatus[j][m]=0x00;
		//ToUF_MotorStatus[j][m]=0x00;
		FADER_UPDATE_FLAG[j][m]=0;
		MUTE_UPDATE_FLAG[j][m]=0;
		TrackLockedState[j][m]=0x00;
		SlaveMatrix[j][m]=m;
		CoreMatrix[j][m]=m;

		FaderActifs[j][m]=0;
		enableReadVolState[j][m]=0xff;
		enableReadCutState[j][m]=0xff;
		enableWriteCutState[j][m]=0xff;
		enableWriteVolState[j][m]=0xff;
		UDPActiveChannelFlag[j][m]=0xff;
		fromFaderCutState[j][m]=0x00;
		ChanLinkState[j][m]=0x00;
		for (n=0;n<3;n++)
			{
			automodeRegisters[j][m][n]=0x00;
			}
		for (k=0;k<8;k++)
			{
			FADER_AUTOMODE_STATE[j][m][k]=0x00;
			}	
		}
	}
for (n=0;n<SYSTEMSIZE;n++)
	{
	faderGroupNumber[n]=0x00;
	faderLinkNumber[n]=0x00;	
	LastFaderAssignedGroup[n]=0x00;
	rxFromSekaFaderData[n]=0x00;
	rxFromSekaSwtData[n]=0x00;
	txToSekaFaderData[n]=0x00;
	txToSekaFaderLedState[n]=0x00;
	prevSwtOnMode[n]=0x00;
	prevSwtOffMode[n]=0x00;
	multiSwtCode[n]=0x00;
	swtMode[n]=0x00;
	}

MCMASwtMode[0]=0;
MCMASwtMode[1]=0;
MCMASwtMode[2]=0;
MCMASwtMode[3]=0;

flashSpeed1=50;

masterSwtStates=0;
masterSwtNbr=7;
globalSwtNbr=6;
gotoMixOnFlag=0;
gotoMixOffFlag=0;
SPI1InitFlag=0;
deviceDetachFlag=1;
prev_connected_state=0;
connected=0;
configured=0;
Pulsar=0x00;
Parser_usb_buffer_number=0;
ScanActiveSlaveFLAG=1;
#ifdef __DEBUGBUFFER
debugModeFlag=1;
#else
debugModeFlag=0;
#endif
MasterChanAssNum=0;
USBStateFlag=0;
ResetTRIG=0;
configCnt=0;
Config_registers[15]=12;
Real_Slave_Number=0;
Init_Rx_Midi_Buffer(0);
Init_Tx_Midi_Buffer(0);
Init_Midi_Msg(0);
Init_Midi_Msg(1);
Init_Midi_Msg(2);
Init_Midi_Msg(3);
Init_Midi_Msg(4);
Global_null_threshold=NULL_THRESHOLD;
//multi_switch_cnt=0;
first_switch_cnt=0;
//first_switch_auto_state=0;
Process_flag=0;
//adr_n_code=0x01;
Grp_flag=0;
Software_auto_flag=0;
handle_count=0;
Set_all_flag=0;
copy_automode_flag=0;
//	Refresh_auto_timer=0;
VCA_Bank_Location=3;
HUI_CORE_focus=0;
Bank_slide_mask=0;
active_slave_pass_count=119;
pass_cnt=0;
sysex_cntr=0;
usb_buffer_cnt=0;
usb_data_count=0;
USBSTATE=1;
usb_buffer_process_cnt=0;
ConfigIndexCntr=1;

Usb_buffer.size=64;
Usb_buffer.top=0;
Usb_buffer.bot=0;
Usb_buffer.quantity=0;
Usb_buffer.maxquantity=0;

RefreshFlag=0;

unsigned char usbbufertest;
usbbufertest=USBBUFFERS;
sysex_index=0;

sysex_complete_flag=0;
trig_rcvd[0]=0;
trig_rcvd[1]=0;
trig_rcvd[2]=0;
trig_rcvd[3]=0;

TcBufferSize=TCBUFFERSIZE;
SysexBufferSize=SYSEXBUFFERSIZE;

USb_buffer_index=1;
KBD_DONGLE_ONLINE=0;
IEC0bits.T2IE = 0;		//Arreter le interrupt du timer#2
//count3=0;
//count4=0;
ctrl_mode=0;			//Démarre en Mode HUI par défaut
Save_Config_To_EEprom_Flag=0;
Config_registers[17]=0;		//Systeme status flags reset
Pot_n_switch_number=1;

Selected_Chanel_Number=1;
return 1;
}

/********************************************************************
 * Function:        void LoadEEPROMConfigTAI(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Init_var initialise les variables de configuration
 *
 * Note:            None
 *************************************************************/
void LoadEEPROMConfigTAI(void)
{
#define REAPER 1
#define PROTOOLS 0
#define HYBRIDE 1
#define OTB 0


unsigned char test_char;
test_char=Factory_Init_Read();
if(0xa3!=test_char)
	{								//Do factory init
	Memory_Write_Single(0,0xa3);
	Config_registers[4]=0x00;		//Recallit mode et config 0=off par defaut
	Config_registers[5]=0x01;		//Recall it channel select
	Config_registers[6]=0x08;		//Hybride mode 0=off
	Config_registers[7]=0x03;		//On stop goto
	Config_registers[8]=0x00;		//master fader/#32 relais 0=vca#8
	Config_registers[9]=0x00;		//Prtools fader scale 0=Prootls 8/9/10
	Config_registers[11]=3;			//Controle surface mode 0=HUI 1= HUI hybrid 2=REAPER 3=INJEKTOR
	Config_registers[12]=0x00;		//Group focus et goup mask
	Config_registers[14]=0x22;			//Default system size, MaxSlave=12
	Config_registers[15]=12;		//Default system size, MaxSlave=12

	Config_registers[16]=0;			//VCA bank LOCATON
	Config_registers[18]=95;		//global mode switch location
	Config_registers[19]=94;		//master swit location
	Config_registers[20]=0;			//TrackLockManager=0ff;
	Config_registers[21]=0;			//MasterFader channel number
	Config_registers[23]=0;			//MasterFader channel number

	Config_registers[24]=0;			//Level match
	Config_registers[32]=8;			//Smouthing buffer size
	Config_registers[33]=0;			//Final DAC resolution 0=10bits 1=12bits

	Save_Config_To_EEprom();
	}
else
	{			
	Config_registers[1]=0;								//TRANSPORT STATE
	Config_registers[3]=0x00;							//Recallit mode et config 0=off par defaut
	Config_registers[4]=0x00;							//Recallit mode et config 0=off par defaut
	Config_registers[5]=0x01;							//Recall it channel select
	Config_registers[6]=0x08;							//Memory_Read_Single(6);
	Config_registers[7]=Memory_Read_Single(7);			//On stop goto
	Config_registers[8]=Memory_Read_Single(8);			//master fader/#32 relais 0=vca#8
	Config_registers[9]=Memory_Read_Single(9);			//Prtools fader scale 0=Prootls 8/9/10
	Config_registers[11]=Memory_Read_Single(11);
	Config_registers[12]=0x00;							//Group focus et goup mask
	Config_registers[14]=0x02|(Memory_Read_Single(14)&0xf0);
	//Config_registers[15]=12;							//Default system size, MaxSlave=12
	Config_registers[16]=Memory_Read_Single(16);
	Config_registers[18]=Memory_Read_Single(18);
	Config_registers[19]=Memory_Read_Single(19);
	Config_registers[21]=Memory_Read_Single(21);		//Master Fader Location
	Config_registers[23]=Memory_Read_Single(23);		//SSL GLOBAL MOTOR AND VCA MODES
	Config_registers[32]=Memory_Read_Single(32);		//Multistage filter Steps
	Config_registers[33]=Memory_Read_Single(32);		//Multistage filter Steps
	}
//	Config_registers[11]=0;

Config_registers[10]=0xc0; //Systeme type (flying fader) and mix off
Config_registers[32]=10;
Config_registers[33]=0;

Config_registers[23]=0x02;
//Config_registers[21]=73;								//Temp master fader channel number
Config_registers[25]=0;
Config_registers[14]=0x22;								//READ/WRITE no snap no rehearse
//Config_registers[15]=12;								//Default system size, MaxSlave=12
#ifdef __STANDALONE
Config_registers[10]=0xc1;
#endif
Push_Config_data_In_Queue(2,0,0);		//push Automation change mode
Push_Config_data_In_Queue(3,0x00,0);	//push automation mode

Push_Config_data_In_Queue(4,Config_registers[4],0);			//push recall mode
//Push_Config_data_In_Queue(5,Config_registers[5],0);		//push recall mode
Push_Config_data_In_Queue(7,Config_registers[7],0);
Push_Config_data_In_Queue(8,Config_registers[8],0);
Push_Config_data_In_Queue(9,Config_registers[9],0);
Push_Config_data_In_Queue(11,Config_registers[11],0);
Push_Config_data_In_Queue(12,Config_registers[12],0);
Push_Config_data_In_Queue(14,Config_registers[14],0);
Push_Config_data_In_Queue(23,Config_registers[23],0);
Push_Config_data_In_Queue(24,Config_registers[24],0);
Push_Config_data_In_Queue(25,Config_registers[25],0);
Push_Config_data_In_Queue(28,1,0);
Push_Config_data_In_Queue(28,2,0);
Push_Config_data_In_Queue(32,Config_registers[32],0);
Push_Config_data_In_Queue(33,Config_registers[33],0);
Config_registers[30]=CommVersion;
Push_Config_data_In_Queue(10,Config_registers[10],0);
}

/********************************************************************
 * Function:        unsigned char Init_io(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initiailise les entrées et sorties de CPU
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                *
 *
 * Note:            None
 *******************************************************************/
unsigned char Init_io(void)
{
//Assignation des ports de communication avec les slaves	
AD1PCFGL =0xffff;

TESTPIN1_DIR=0;
TESTPIN2_DIR=0;
TESTPIN3_DIR=0;
TESTPIN4_DIR=0;

return 0;
}

/********************************************************************
 * Function:        unsigned char Init_Timers(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initiailise les entrées et sorties de CPU
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                *
 *
 * Note:            None
 *******************************************************************/
unsigned char Init_Timers(void)
{

/************************************************/
// Section init Timer1
/************************************************/
TMR1 = 0;				// Clear timer 1
PR1 = 0x00fa;			// Interrupt aux 1ms
//	PR1 = 0x1000;			// Interrupt aux 16ms
IFS0bits.T1IF = 0;		// Clear interrupt flag
IEC0bits.T1IE = 1;		// Set ou clear interrupt enable bit
IPC0bits.T1IP=4;

/************************************************/
// Section init Timer3 utilise pour le SPI1: Comm master/slave
/************************************************/
IPC2bits.T3IP=4;
TMR3 = 0;				// Clear timer 1
PR3 = 0x0200;				// Interrupt aux 1ms
T3CON = 0x8010;			// Fosc/4, 1:64 prescale, start TMR1
IFS0bits.T3IF = 0;		// Clear interrupt flag
IEC0bits.T3IE = 1;		// Set ou clear interrupt enable bit

/************************************************/
// Section init Timer4 utilise pour le SPI2:	COMM EXP
/************************************************/

TMR4 = 0;				// Clear timer 1
PR4 = 0x0020;			// Interrupt aux 4ms
IFS1bits.T4IF = 0;		// Clear interrupt flag
IEC1bits.T4IE = 0;		// Set ou clear interrupt enable bit
T4CON = 0x8030;			// Fosc/4, 1:64 prescale, start TMR1

/************************************************/
// Section init variables des compteurs
/************************************************/
//Timeout = 0;
DAW_ONLINE_Timer= 4000;
T1CON = 0x8010;			// Fosc/4, 1:64 prescale, start TMR1
return 1;
}		

/************************************************/
// Section init Timer2 utilisé pour le RS485: Ext Comm 
/************************************************/
void Init_Timer2(void)
{
TMR2 = 0;				// Clear timer 1
PR2 = 0x1000;			// Interrupt aux 4 ms
T2CON = 0x8010;			// Fosc/4, 1:64 prescale, start TMR1
IFS0bits.T2IF = 0;
IEC0bits.T2IE = 1;		// Set ou clear interrupt enable bit
IPC1bits.T2IP=4;
}

/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{
AD1PCFGL = 0xFFFF; //TOUT LES PINS ANALOG SONT OFF

	//On the PIC24FJ64GB004 Family of USB microcontrollers, the PLL will not power up and be enabled
	//by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
	//This allows the device to power up at a lower initial operating frequency, which can be
	//advantageous when powered from a source which is not gauranteed to be adequate for 32MHz
	//operation.  On these devices, user firmware needs to manually set the CLKDIV<PLLEN> bit to
	//power up the PLL.

#ifdef __PIC24FJ64
unsigned int pll_startup_counter = 600;
CLKDIVbits.PLLEN = 1;
while(pll_startup_counter--);
#endif
// si ça bug c'est ici la mode a revoir remettre en commentaire la ligne suivante


#if defined(USE_USB_BUS_SENSE_IO)
tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
#endif

#if defined(USE_SELF_POWER_SENSE_IO)
//tris_self_power = INPUT_PIN;	// See HardwareProfile.h
#endif

UserInit();
USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware variables to known states.

}//end InitializeSystem

/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:
 *
 *****************************************************************************/
void UserInit(void)
{
//Initialize all of the LED pins
hi=0x00;
lo=0x00;
//initialize the variable holding the handle for the last
// transmission
USBTxHandle = NULL;
USBRxHandle = NULL;
}

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void){}
/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *
 *					This call back is invoked when a wakeup from USB suspend
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void){}
/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.

    if(msCounter != 0)
    {
        msCounter--;
    }
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void){}
/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void){}
/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void){}// Must claim session ownership if supporting this request end
/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This
 *					callback function should initialize the endpoints
 *					for the device's usage according to the current
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
//enable the HID endpoint
USBEnableEndpoint(MIDI_EP1,USB_OUT_ENABLED|USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

//Re-arm the OUT endpoint for the next packet
USBRxHandle = USBRxOnePacket(MIDI_EP1,(BYTE*)&ReceivedDataBuffer,64);
}
/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
static WORD delay_count;
    
//First verify that the host has armed us to perform remote wakeup.
//should be checked).
if(USBGetRemoteWakeupStatus() == TRUE) 
    {
	//Verify that the USB bus is in fact suspended, before we send
    //remote wakeup signalling.
	if(USBIsBusSuspended() == TRUE)
        {
    	USBMaskInterrupts();
            
		//Clock switch to settings consistent with normal USB operation.
		USBCBWakeFromSuspend();
		USBSuspendControl = 0; 
		USBBusIsSuspended = FALSE;  //So we don't execute this code again, 
									//until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //5ms+ total delay since start of idle).
		delay_count = 3600U;        
		do{
			delay_count--;
			}while(delay_count);
            
            //Now drive the resume K-state signalling onto the USB bus.
		USBResumeControl = 1;       // Start RESUME signaling
		delay_count = 1800U;        // Set RESUME line for 1-13 ms
		do{
			delay_count--;
			}while(delay_count);
		USBResumeControl = 0;       //Finished driving resume signalling

		USBUnmaskInterrupts();
		}
	}
}

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        int event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           int event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
char test;
switch( event )
	{
    case EVENT_TRANSFER:
    	//Add application specific callback task or callback function here if desired.
        break;
	case EVENT_SOF:
    	USBCB_SOF_Handler();
		break;
	case EVENT_SUSPEND:
		USBCBSuspend();
		break;
	case EVENT_RESUME:
		USBCBWakeFromSuspend();
		break;
	case EVENT_CONFIGURED:
		USBCBInitEP();
		break;
	case EVENT_SET_DESCRIPTOR:
		USBCBStdSetDscHandler();
		break;
	case EVENT_EP0_REQUEST:
		USBCBCheckOtherReq();
		break;
	case EVENT_BUS_ERROR:
		USBCBErrorHandler();
		break;
	case EVENT_TRANSFER_TERMINATED:
		//Add application specific callback task or callback function here if desired.
		//The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
		//FEATURE (endpoint halt) request on an application endpoint which was 
		//previously armed (UOWN was = 1).  Here would be a good place to:
		//1.Determine which endpoint the transaction that just got terminated was 
	 	//	on, by checking the handle value in the *pdata.
		//2.  Re-arm the endpoint if desired (typically would be the case for OUT 
		//      endpoints).
		test=0;
		break;
		default:
		break;
	}
	return TRUE;
}


/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        int event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           int event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
void USB_disconnect_sequence(void)
{
	connected =0;
	configured=0;					//insérer ici les code de bypass relais
	USBDeviceDetach();				//Sinon détache la connection midi

}		

//*******************************************************************************************************************
//*******************************************************************************************************************

/************************************************/
// Routine d'interrupt du Timer 1
/************************************************/
void Delais(unsigned int tic_ms)
{
if(tic_ms)
	{
	delaycnt = tic_ms;
	while (delaycnt);
	}
}

//--------------------------------------------------------------
//
// LAPS DELAIS1
//
//--------------------------------------------------------------
void SetLAPS1(unsigned short time)
{
if(time!=0)
	LAPS1=time;
}

//--------------------------------------------------------------
//
// LAPS DELAIS2
//
//--------------------------------------------------------------
void SetLAPS2(unsigned short time)
{
if(time!=0)
	LAPS2=time;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	void Push_Tx_Midi_Data(unsigned char n, unsigned char buffer_num)
|Notes:	pousse une donnée dans un buffer circulaire
|		sans test d'overflow message midi qui doit etre envoyé
|arguments	:n la strucutre de midi message
|			:donnée a sauver
|
|
|
*****************************************************************
****************************************************************/
unsigned char Push_Tx_Midi_Data(unsigned char n, unsigned char buffer_num)
{
if(TxMidiBuffer.quantity>=TxMidiBuffer.size)
	return 0;
TxMidiBuffer.code[TxMidiBuffer.top] = (TXmidi_msg.code&0x0f)|(TXmidi_msg.cable_num<<4);		//sauve la valeur dans le buffer
TxMidiBuffer.status[TxMidiBuffer.top] = TXmidi_msg.status;									//sauve la valeur dans le buffer
TxMidiBuffer.data1[TxMidiBuffer.top] = TXmidi_msg.data1;									//sauve la valeur dans le buffer
TxMidiBuffer.data2[TxMidiBuffer.top] = TXmidi_msg.data2;									//sauve la valeur dans le buffer

TxMidiBuffer.top++;
if (TxMidiBuffer.top >= TxMidiBuffer.size)					//test si on doit rouler au début
	TxMidiBuffer.top = 0;									//si oui roule au début

TxMidiBuffer.quantity++;	
if(TxMidiBuffer.quantity>=TxMidiBuffer.Maxquantity)
	TxMidiBuffer.Maxquantity=TxMidiBuffer.quantity;
if(TxMidiBuffer.quantity>=TxMidiBuffer.size)
	DebugSet(0x0080);

return 1;

}
/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char Pull_Midi_Data(unsigned char n, unsigned char buffer_num)
|
|Notes:	retir une donnée dans un buffer circulaire
|		sans test d'overflow			
|arguments 	:n la strucutre de midi message
|			:Buffer_num le buffer de messag midi en Rx ou Tx
|
|
*****************************************************************
****************************************************************/
unsigned char Pull_Tx_Midi_Data(unsigned char n, unsigned char buffer_num)
{
if(TxMidiBuffer.quantity)
	{
	TXmidi_msg.code = TxMidiBuffer.code[TxMidiBuffer.bot]&0x0f;							//sauve la valeur dans le buffer
	TXmidi_msg.status = TxMidiBuffer.status[TxMidiBuffer.bot];
	TXmidi_msg.data1= TxMidiBuffer.data1[TxMidiBuffer.bot];
	TXmidi_msg.data2 = TxMidiBuffer.data2[TxMidiBuffer.bot];
	TXmidi_msg.cable_num = TxMidiBuffer.code[TxMidiBuffer.bot]>>4;
	TxMidiBuffer.bot++;	
	TxMidiBuffer.quantity--;						//
	if (TxMidiBuffer.bot >= TxMidiBuffer.size)		//test si on doit rouler au début
		TxMidiBuffer.bot = 0;							//si oui roule au début
	}
return 1;
}
/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char Pull_Midi_Data(void)
|
|Notes:	retir une donnée dans un buffer circulaire
|		sans test d'overflow			
|arguments 	:n la strucutre de midi message
|			:Buffer_num le buffer de messag midi en Rx ou Tx
|
|
*****************************************************************
****************************************************************/
unsigned char Pull_Rx_Midi_Data(void)
{
if(RxMidiBuffer.quantity)
	{
	RXmidi_msg.code = RxMidiBuffer.code[RxMidiBuffer.bot];			//sauve la valeur dans le buffer
	RXmidi_msg.status = RxMidiBuffer.status[RxMidiBuffer.bot];
	RXmidi_msg.data1= RxMidiBuffer.data1[RxMidiBuffer.bot];
	RXmidi_msg.data2 = RxMidiBuffer.data2[RxMidiBuffer.bot];
	RXmidi_msg.cable_num = RxMidiBuffer.code[RxMidiBuffer.bot]>>4;
	RxMidiBuffer.bot++;	
	RxMidiBuffer.quantity--;
	if(RxMidiBuffer.bot >= RxMidiBuffer.size)						//test si on doit rouler au début
		RxMidiBuffer.bot = 0;										//si oui roule au début
	}
return 0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	Get_Tx_Midi_Quant									|
|Notes:	retir une donnée dans un buffer circulaire				|
|		sans test d'overflow									|
|arguments 	:pointeur vers structure de buffer					|
|			:pointeur de donnée a retourner						|
|																|
|																|
*****************************************************************
****************************************************************/
unsigned short Get_Tx_Midi_Quant(unsigned char buffer_num)
{
return TxMidiBuffer.quantity;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	Get_Tx_Midi_Quant									|
|Notes:	retir une donnée dans un buffer circulaire				|
|		sans test d'overflow									|
|arguments 	:pointeur vers structure de buffer					|
|			:pointeur de donnée a retourner						|
|																|
|																|
*****************************************************************
****************************************************************/
void ClearMidiTXBuffer(void)
{
TxMidiBuffer.bot=0;	
TxMidiBuffer.top=0;	
TxMidiBuffer.quantity=0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:	Get_Rx_Midi_Quant									|
|Notes:	retir une donnée dans un buffer circulaire				|
|		sans test d'overflow									|
|arguments 	:pointeur vers structure de buffer					|
|			:pointeur de donnée a retourner						|
|																|
|																|
*****************************************************************
****************************************************************/
unsigned short Get_Rx_Midi_Quant(unsigned char buffer_num)
{
return RxMidiBuffer.quantity;
}


/****************************************************************
*****************************************************************
|FONCTIONS:	void init_midi_buffer(unsigned char buffer_num, circ_midi_buffer buffer[])
|
|Notes:	retir une donnée dans un buffer circulaire
|		sans test d'overflow
|arguments 	:pointeur vers structure de buffer
|			:pointeur de donnée a retourner	
|											
|
*****************************************************************
****************************************************************/
void Init_Rx_Midi_Buffer(unsigned char buffer_num)
{
unsigned short a;
for (a=0;a<CIRCULARE_SIZE_RX;a++)
	{
	RxMidiBuffer.code[a] = 0;		//sauve la valeur dans le buffer
	RxMidiBuffer.status[a] = 0;		//sauve la valeur dans le buffer
	RxMidiBuffer.data1[a] = 0;		//sauve la valeur dans le buffer
	RxMidiBuffer.data2[a] = 0;		//sauve la valeur dans le buffer
	}
RxMidiBuffer.size= CIRCULARE_SIZE_RX;
RxMidiBuffer.top= 0;
RxMidiBuffer.bot= 0;
RxMidiBuffer.quantity= 0;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:void Init_Tx_Midi_Buffer(unsigned char buffer_num)
//Notes:
//				
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Init_Tx_Midi_Buffer(unsigned char buffer_num)
{
unsigned short a;
for (a=0;a<CIRCULARE_SIZE_TX;a++)
	{
	TxMidiBuffer.code[a] = 0;			//sauve la valeur dans le buffer
	TxMidiBuffer.status[a] = 0;		//sauve la valeur dans le buffer
	TxMidiBuffer.data1[a] = 0;		//sauve la valeur dans le buffer
	TxMidiBuffer.data2[a] = 0;		//sauve la valeur dans le buffer
	}
TxMidiBuffer.size= CIRCULARE_SIZE_TX;
TxMidiBuffer.top= 0;
TxMidiBuffer.bot= 0;
TxMidiBuffer.quantity= 0;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:unsigned char NextRaspBank(void)
//Notes:
//				
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
unsigned char NextRaspBank(void)
{
RaspSlaveIndex++;																//increment le slave number
if(RaspSlaveIndex>3)															//si maxslave atteint
	{
	RaspCoreIndex++;															//passe au prochain core
	RaspSlaveIndex=0;															//et recommence le #slave a 0
	}
if(((RaspCoreIndex*4+RaspSlaveIndex))>(Config_registers[15]-1))				//test si on a atteint le max total de slave
	{
	RaspSlaveIndex=0;															//et recommence le #slave a 0
	RaspCoreIndex=0;															//et recommence le #core a 0
	if(Config_registers[24])
		{
		if(!AtlFdrDataCntr)
			AtlFdrDataCntr=1;
		else
			AtlFdrDataCntr=0;
		}
	else
		AtlFdrDataCntr=1;
	}

return ((RaspCoreIndex*32)+(RaspSlaveIndex*8));
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:void ResetRaspfader(void)
//Notes:
//				
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ResetRaspfader(void)
{
RaspMsgType=0;
RaspSlaveIndex=0;
RaspCoreIndex=0;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:void TriggerConfigToRasp(void)
//Notes:
//				
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void TriggerConfigToRasp(void)
{
RaspMsgType=1;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:BOOL DBM_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
//Notes:
//				
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOL DBM_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
USB_DBM_ISR( *(BYTE *)pdata );
return 1;
}
	
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:void USB_DBM_ISR( BYTE USTAT_local)
//Notes:
//				
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void USB_DBM_ISR( BYTE USTAT_local)
{
unsigned char USBDataCnt;
if(!(USTAT_local&0x08))
	{
	if(!USBHandleBusy(USBRxHandle))
   		{
		if(Usb_buffer.quantity>Usb_buffer.size)
			{
			DebugSet(0x0001);
			USBRxHandle = USBRxOnePacket(MIDI_EP1,(BYTE*)&AltReceivedDataBuffer,USB_EP0_BUFF_SIZE);
			}
		else
			{
			USBRxHandle = USBRxOnePacket(MIDI_EP1,(BYTE*)&ReceivedDataBuffer,USB_EP0_BUFF_SIZE);
			USBDataCnt=0;
			while(USBDataCnt<Usb_buffer.size)
				{
				USBDataBuffer[Usb_buffer.top][USBDataCnt]=ReceivedDataBuffer[USBDataCnt];
				ReceivedDataBuffer[USBDataCnt]=0;
				USBDataCnt++;
				}
			USBDataBufferQuant[Usb_buffer.top]=USBDataCnt;
			Usb_buffer.top++;
			if(Usb_buffer.top>=Usb_buffer.size)
				Usb_buffer.top=0;
			if(Usb_buffer.quantity>=Usb_buffer.size)
				DebugSet(0x0001);
			if(Usb_buffer.maxquantity>=Usb_buffer.quantity)
				Usb_buffer.maxquantity=Usb_buffer.quantity;
			}
		}
	}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Sous fonctions d'interuption:void DoRaspComm(void)
//Notes:
//				
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void DoRaspComm(void)
{
if(BootStateFlag>=6)
	{
	if(!Rasp_Heartbeat_Flag)
		{
		switch(RaspMsgType)
			{
			case 0:RaspMsgMaker();		Rasp_Heartbeat_Flag=1;break;					//le FLAG TX Rasp data = 0
			case 1:SendSingleRaspMsg();	Rasp_Heartbeat_Flag=1;break;					//le FLAG TX Rasp data =3 (single data msg)
			default:break;
			}
		}
	else
		Rasp_Heartbeat_Flag--;
	RaspMsgParser();
	}
else
if(BootStateFlag>0)
	{
	if(LastBootStateFlag!=BootStateFlag)
		{
		LastBootStateFlag=BootStateFlag;
		SysBootingMsgMaker();
		}
	RaspMsgParser();
	}
	
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//Fonctions void SysBootingMsgMaker(void)
//Notes:
//Arguments:
//Variables locals:
//
//Variables globals:
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void SysBootingMsgMaker(void)
{
Send_Rasp_Data(0xff);					//ferme le fichier si ecriture au Raspberry pi en cours
Send_Rasp_Data(0xe0);					//le code de controle pour un data type msg#0
Send_Rasp_Data(BootStateFlag);			//le code de controle pour un data type msg#0
Send_Rasp_Data(0xfe);					//le code de controle pour un data type msg#0
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//Fonctions void RaspMsgMaker(void)
//Notes:
//Arguments:
//Variables locals:
//
//Variables globals:
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void RaspMsgMaker(void)
{
unsigned char n;
unsigned char tempautomode;
int ChannelNumber;
Send_Rasp_Data(0xff);					//ferme le fichier si ecriture au Raspberry pi en cours
Send_Rasp_Data(0x90);					//le code de controle pour un data type msg#0

if(AtlFdrDataCntr)
	{
/*	if(debugModeFlag)																				//LE LSB du data
		{
		if((!PRaspCoreIndex)&&(!RaspSlaveIndex))
			{
			TempFaderVal[5]=usb_buffer_cnt<<8;
			TempFaderVal[6]=RxMidiBuffer.quantity<<4;
			TempFaderVal[7]=TxMidiBuffer.quantity<<5;
			}
		}						
*/
	Send_Rasp_Data(0x00|stateBits);								//le code de controle pour data fader info msg#1 
	Send_Rasp_Data((RaspCoreIndex*4)+RaspSlaveIndex);			//le numéro de block fader msg#2 
	for(n=0;n<8;n++)
		{
		ChannelNumber=(RaspCoreIndex*32)+(RaspSlaveIndex*8)+n;
		tempautomode=FADER_AUTOMODE_STATE[RaspCoreIndex][RaspSlaveIndex][n];
		if(Config_registers[24])														//pas en level match
			{
			Send_Rasp_Data(((rxFromSekaFaderData[ChannelNumber]&0x3fff)>>7)&0x007f);
			Send_Rasp_Data((rxFromSekaFaderData[ChannelNumber]&0x3fff)&0x7f);
			}
		else
		switch(tempautomode)
			{
			case 0:
				Send_Rasp_Data(((rxFromSekaFaderData[ChannelNumber]&0x3fff)>>7)&0x007f);
				Send_Rasp_Data((rxFromSekaFaderData[ChannelNumber]&0x3fff)&0x7f);
			break;
			case 1://WRITE
			case 5://TRIMWRITE
//				if(enableWriteVolState[RaspCoreIndex][RaspSlaveIndex]&BitIndex[n])
//					{
					Send_Rasp_Data(((rxFromSekaFaderData[ChannelNumber]&0x3fff)>>7)&0x007f);
					Send_Rasp_Data((rxFromSekaFaderData[ChannelNumber]&0x3fff)&0x7f);
//					}
//				else
//					{
//					Send_Rasp_Data(((rxFromSekaFaderData[ChannelNumber]&0x3fff)>>7)&0x007f);
//					Send_Rasp_Data((rxFromSekaFaderData[ChannelNumber]&0x3fff)&0x7f);
//					}
					break;					
			case 2://TOUCH ou LATCH
			case 3://READ
			case 6://TRIM TOUCH ou TRIM LATCH
			case 7://TRIMREAD
//				if(enableReadVolState[RaspCoreIndex][RaspSlaveIndex]&BitIndex[n])
//					{
//					Send_Rasp_Data(((VCA_IN_BUF[RaspCoreIndex][RaspSlaveIndex][n])>>7)&0x007f);
//					Send_Rasp_Data(VCA_IN_BUF[RaspCoreIndex][RaspSlaveIndex][n]&0x7f);
//					}
//				else
//					{
					Send_Rasp_Data(((rxFromSekaFaderData[ChannelNumber]&0x3fff)>>7)&0x007f);
					Send_Rasp_Data((rxFromSekaFaderData[ChannelNumber]&0x3fff)&0x7f);
//					}
			break;
			}
		}
//mutes, automation et touchstate bit 0 a 6
	Send_Rasp_Data(toFaderCutState[RaspCoreIndex][RaspSlaveIndex]&0x7f);						//mute 1-7			msg#19
	Send_Rasp_Data(automodeRegisters[RaspCoreIndex][RaspSlaveIndex][0]&0x7f);			//auotbit0 1-7		msg#20
	Send_Rasp_Data(automodeRegisters[RaspCoreIndex][RaspSlaveIndex][1]&0x7f);			//auotbit1 1-7		msg#21
	Send_Rasp_Data(automodeRegisters[RaspCoreIndex][RaspSlaveIndex][2]&0x7f);			//auotbit2 1-7		msg#22
	Send_Rasp_Data(fromFaderTouchState[RaspCoreIndex][RaspSlaveIndex]&0x7f);				//Touch_state 		msg#23
	Send_Rasp_Data((
			  (toFaderCutState[RaspCoreIndex][RaspSlaveIndex]&0x80)>>1)						//mute bit bit8		msg#24
			|((automodeRegisters[RaspCoreIndex][RaspSlaveIndex][0]&0x80)>>2)			//autobit0 bit8
			|((automodeRegisters[RaspCoreIndex][RaspSlaveIndex][1]&0x80)>>3)			//autobit1 bit8
			|((automodeRegisters[RaspCoreIndex][RaspSlaveIndex][2]&0x80)>>4)			//autobit2 bit8
			|((fromFaderTouchState[RaspCoreIndex][RaspSlaveIndex]&0x80)>>5));				//Touch_state bit 8	Send_Rasp_Data(auxDataIndex&0x07);	//auxDataIndexauxDataIndex				msg#25
	Send_Rasp_Data((ConfigIndexCntr&0x3f)|((Config_registers[ConfigIndexCntr]&0x80)>>1));	//le config data index et MSb	msg#25
	Send_Rasp_Data((Config_registers[ConfigIndexCntr]&0x7f));								//le config data index et MSb	msg#26
	ConfigIndexCntr++;
	if(ConfigIndexCntr>=64)
		ConfigIndexCntr=1;
	
	Send_Rasp_Data(auxDataIndex&0x07);																//auxDataIndex	msg#25
	switch(auxDataIndex)
		{	
		case 0:Send_Rasp_Data(TC_DATA[0]);break;											//msg#26
		case 1:Send_Rasp_Data(TC_DATA[1]);break;
		case 2:Send_Rasp_Data(TC_DATA[2]);break;
		case 3:Send_Rasp_Data(TC_DATA[3]);break;
		case 4:Send_Rasp_Data(auxData1);break;
		case 5:Send_Rasp_Data(auxData2);auxData1=0;break;
		default:break;
		}
	auxDataIndex++;
	if(auxDataIndex>5)
		auxDataIndex=0;
	}
else
	{
//	RaspCoreIndex=RaspCoreIndex;		//OK verifié
//	RaspSlaveIndex=RaspSlaveIndex;		//OK vérifié
	Send_Rasp_Data(0x10|stateBits);									//le code de controle pour data type DAW FADER
	Send_Rasp_Data((RaspCoreIndex*4)+RaspSlaveIndex);				//le numéro de block fader
	for(n=0;n<8;n++)
		{
		ChannelNumber=(RaspCoreIndex*32)+(RaspSlaveIndex*8)+n;
		Send_Rasp_Data(((txToSekaFaderData[ChannelNumber]&0x3fff)>>7)&0x007f);			//le MSB du data1	msg#3
		Send_Rasp_Data((txToSekaFaderData[ChannelNumber]&0x3fff)&0x7f);					//LE LSB du data1	msg#4
		}

	Send_Rasp_Data(fromFaderCutState[RaspCoreIndex][RaspSlaveIndex]&0x7f);				//mute 1-7			msg#19
	Send_Rasp_Data(automodeRegisters[RaspCoreIndex][RaspSlaveIndex][0]&0x7f);			//auotbit0 1-7		msg#20
	Send_Rasp_Data(automodeRegisters[RaspCoreIndex][RaspSlaveIndex][1]&0x7f);			//auotbit1 1-7		msg#21
	Send_Rasp_Data(automodeRegisters[RaspCoreIndex][RaspSlaveIndex][2]&0x7f);			//auotbit2 1-7		msg#22
	Send_Rasp_Data(fromFaderTouchState[RaspCoreIndex][RaspSlaveIndex]&0x7f);			//Touch_state 		msg#23
	Send_Rasp_Data((
			  (fromFaderCutState[RaspCoreIndex][RaspSlaveIndex]&0x80)>>1)				//mute bit bit8		msg#24
			|((automodeRegisters[RaspCoreIndex][RaspSlaveIndex][0]&0x80)>>2)			//autobit0 bit8
			|((automodeRegisters[RaspCoreIndex][RaspSlaveIndex][1]&0x80)>>3)			//autobit1 bit8
			|((automodeRegisters[RaspCoreIndex][RaspSlaveIndex][2]&0x80)>>4)			//autobit2 bit8
			|((fromFaderTouchState[RaspCoreIndex][RaspSlaveIndex]&0x80)>>5));			//Touch_state bit 8
	
	Send_Rasp_Data((ConfigIndexCntr&0x3f)|(((Config_registers[ConfigIndexCntr]&0x80)>>1)));		//le config data index et MSb	msg#25
	Send_Rasp_Data((Config_registers[ConfigIndexCntr]&0x7f));									//le config data index et LSb	msg#26
	ConfigIndexCntr++;
	if(ConfigIndexCntr>=64)
		ConfigIndexCntr=1;
	Send_Rasp_Data(auxDataIndex&0x07);																	//auxData	msg#27

	switch(auxDataIndex)
		{	
		case 0:Send_Rasp_Data(TC_DATA[0]);break;												//msg#28
		case 1:Send_Rasp_Data(TC_DATA[1]);break;
		case 2:Send_Rasp_Data(TC_DATA[2]);break;
		case 3:Send_Rasp_Data(TC_DATA[3]);break;
		case 4:Send_Rasp_Data(auxData3);break;
		case 5:Send_Rasp_Data(auxData4);auxData3=0;break;
		default:break;
		}
	auxDataIndex++;
	if(auxDataIndex>5)
		auxDataIndex=0;
	}
Send_Rasp_Data(0xfe);																	//msg#29
NextRaspBank();
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//Sous fonctions d'interuption:void SendSingleRaspMsg(void)
//Notes:
//Arguments:
//Variables locals:
//
//Variables globals:
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void SendSingleRaspMsg(void)
{
unsigned short data;
Send_Rasp_Data(0xff);								//Code de synchro de debut de message
Send_Rasp_Data(0x90);								//le code de controle pour un Message de heartbeat
Send_Rasp_Data(0x30|stateBits);						//le code de controle pour type de heartbeat fader data ou info
data=PullRaspSingleData();
Send_Rasp_Data((data>>8)&0x7f);						//le code d'index pour type de heartbeat fader data ou info
Send_Rasp_Data((data&0x0080)>>1);					//Config data msb
Send_Rasp_Data(data&0x7f);							//Config data lsb 0-6
Send_Rasp_Data(0xfe);
	if(!getRaspSingleDataBufferQuantity())
	RaspMsgType=0;
else
	RaspMsgType=1;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:void __attribute__((interrupt, auto_psv)) _T1Interrupt(void)
//	Timer 1 interrupt 
//Arguments:				
//Variables locals:
//		
//Variables globals:
//Note:	pulsar,delaycnt,LAPS1,LAPS2,DAW_ONLINE_TIMER	ResetTRIG					
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void __attribute__((interrupt, auto_psv)) _T1Interrupt(void)
{
	if(PulsarCnt)
		PulsarCnt--;
	else
		{
		PulsarCnt=2000;
		Pulsar=0b00000011;
		}
	if(PulsarCnt==1000)
		Pulsar=0b00000010;			//LSb0 =0;
	else	
	if(PulsarCnt==100)
		Pulsar=0b00000000;;			//LSb1=0;
		
//	if(Timeout)
//		Timeout--;
//		else
//		Timeout=0;
	if (delaycnt)
		delaycnt--;

	if(LAPS1)
		LAPS1--;
	else
		LAPS1=0;
	
	if(LAPS2)
		LAPS2--;
	else
		LAPS2=0;

	#ifndef __STANDALONE
	if(!DAW_ONLINE_Timer)
		{
		if((Config_registers[11]&0x0f)<2)			//Si en HUI MODE
			{
			if(DAW_on_line_cntr[0])
				DAW_on_line_cntr[0]--;
			else
				DAW_on_line_flag[0]=0;

			if(DAW_on_line_cntr[1])
				DAW_on_line_cntr[1]--;
			else
				DAW_on_line_flag[1]=0;

			if(DAW_on_line_cntr[2])
				DAW_on_line_cntr[2]--;
			else
				DAW_on_line_flag[2]=0;

			if(DAW_on_line_cntr[3])
				DAW_on_line_cntr[3]--;
			else
				DAW_on_line_flag[3]=0;

			if(DAW_on_line_flag[0]||DAW_on_line_flag[1]||DAW_on_line_flag[2]||DAW_on_line_flag[3])
				HUI_FLAG=1;
			else
				HUI_FLAG=0;
			DAW_ONLINE_Timer=5000;
			}
		else
		if((Config_registers[11]&0x0f)==2)			//Si en REAPER MODE
			{
			if(TAI_Ping_Flag!=2)
				REAPER_FLAG=0;	
			else
				REAPER_FLAG=1;	
			TAI_Ping_Flag=1;
			DAW_ONLINE_Timer=10000;
			}
		else
		if((Config_registers[11]&0x0f)==3)			//Si en PLUGIN MODE
			{
			if(UDP_Ping_Flag!=2)
				UDP_FLAG=0;
			else
				UDP_FLAG=1;
			UDP_Ping_Flag=1;			
			DAW_ONLINE_Timer=10000;
			}
		}
	DAW_ONLINE_Timer--;
	#else
	HUI_FLAG=1;
	REAPER_FLAG=1;	
	UDP_FLAG=1;
	DAW_on_line_flag[0]=1;
	DAW_on_line_flag[1]=1;
	DAW_on_line_flag[2]=1;
	DAW_on_line_flag[3]=1;
	#endif

	if(ResetTRIG)
		ResetTRIG++;
	IFS0bits.T1IF = 0;		// clear interrupt flag
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//Fonction:void __attribute__((interrupt, auto_psv)) _T2Interrupt(void)
//Notes:
// Routine d'interrupt du Timer 2
//Arguments:				
//Variables locals:
//		
//Variables globals:
//							
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void __attribute__((interrupt, auto_psv)) _T2Interrupt(void)
{
if(flashCounter1)
	flashCounter1--;
else
	{
	flashByte1=flashByte1^0xff;
	if(!flashByte1)
		flashByte2=flashByte2^0xff;	
	flashCounter1=flashSpeed1;
	}

if(SEKA_ONLINE_FLAG)
	{
	WAIT_FOR_ACK_FLAG++;
	if(WAIT_FOR_ACK_FLAG>=100)
		SEKA_ONLINE_FLAG=0;
	else
		{
		if((!U2STAbits.UTXBF)&&(!UART2_RTS))			//Test pour une place de libre,
			{
			UART2_RTS=1;								// en mode 'out'
			Tx_From_Message_address=0x101;				//Adresse de source
			TxChan=TxChan+8;
			if(TxChan>=SYSTEMSIZE)
				TxChan=0;
			Tx_data_count=1;
			MsgLenght=29;
			U2TXREG=Tx_From_Message_address;			//Adresse de source en destination
			IFS0bits.T2IF=0;
			IEC0bits.T2IE=1;
			IFS1bits.U2TXIF = 0;						//clear Tx interrupt flag
			IEC1bits.U2TXIE = 1;						//clear Tx interrupt enable bit
			}
		else
			{
			IFS0bits.T2IF=0;
			IEC0bits.T2IE=1;
			}
		}
	}
else
	{
	if(WAIT_FOR_ACK_FLAG)
		WAIT_FOR_ACK_FLAG--;
	else
	if((!U2STAbits.UTXBF)&&(!UART2_RTS))			//Test pour une place de libre,
		{
		SEKA_ONLINE_FLAG=0;
		WAIT_FOR_ACK_FLAG=10000;
		UART2_RTS=1;								// en mode 'out'
		Tx_From_Message_address=0x101;				//Adresse de source
		TxChan=0;
		Tx_data_count=1;
		MsgLenght=29;
		U2TXREG=Tx_From_Message_address;			//Adresse de source en destination
		IFS0bits.T2IF=0;
		IEC0bits.T2IE=1;
		IFS1bits.U2TXIF = 0;						//clear Tx interrupt flag
		IEC1bits.U2TXIE = 1;						//clear Tx interrupt enable bit
		}
	else
		UART2_RTS=0;
	}
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//FONCTIONS:	_T3Interrupt(void)		TRANSMIT COMM->MASTER
//Notes:	Routine d'interrupt du Timer 3 pour le SPI COMM->MASTERS
//Arguments:
//Variables locals:
//
//Variables globals:
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void __attribute__((interrupt, auto_psv)) _T3Interrupt(void)
{
DoRaspComm();
IFS0bits.T3IF = 0;		// clear interrupt flag
}

