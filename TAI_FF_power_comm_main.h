#include "TAI_FF_Versionner.h"
//---- CONFIGU BYTES -----------------------------------------
//Config pour PLL a 96mhz pour USB et 16MHZ de CLK_OUT
_CONFIG1(
	WDTPS_PS1				//Watchdog Timer Postscaler:1:1
	& FWPSA_PR32			//WDT Prescaler: ratio of 1:32
	& FWDTEN_OFF			//Watchdog Timer is disabled
	& WINDIS_OFF			//Standard Watchdog Timer enabled,(Windowed-mode is disabled)
	& ICS_PGx1				//Emulator functions are shared with PGEC1/PGED1
	& COE_ON				//Set Clip On Emulation Mode:Enabled 
	& BKBUG_OFF				//Device resets into Operational mode
#ifdef __PROTECT
	& GCP_ON				//Code protection is disabled
	& GWRP_ON				//Writes to program memory are allowed
#else
	& GCP_OFF				//Code protection is disabled
	& GWRP_OFF				//Writes to program memory are allowed
#endif
	& JTAGEN_OFF )			//JTAG port is disabled
	
_CONFIG2(
	POSCMOD_NONE 			//Primary oscillator disabled 
  & DISUVREG_ON				//Internal USB 3.3V Regulator is enabled
  & IOL1WAY_OFF  			//IOLOCK One-Way Set Enable Unlimited Writes To RP Registers
  & OSCIOFNC_OFF			//clk_OUT sur RC15  
  & FCKSM_CSDCMD			//Clock Switching and Monitor:Both Clock Switching and Fail-safe Clock Monitor are disabled
  & FNOSC_FRCPLL 			//Oscillator Select:FRC+PLL  
  & PLL_96MHZ_ON			//96 MHz PLL:
  & PLLDIV_NODIV 			//USB 96 MHz PLL Prescaler Select bits:PLLDIV_NODIV  
  & IESO_ON)  				//Internal External Switch Over Mode:IESO mode (Two-speed start-up) enabled

_CONFIG3(
	WPFP_WPFP0
  & WPDIS_WPDIS
  & WPCFG_WPCFGDIS
  & WPEND_WPENDMEM)



//--- INCLUDES -----------------------------------------

#ifdef __PIC24FJ64
#define CIRCULARE_SIZE_TX 480
#define CIRCULARE_SIZE_RX 192
//#define RASP_BUFSIZE	64
//#define EI485BUFSIZE	140
#define USBBUFFERS 		8

#else

#define CIRCULARE_SIZE_TX 256
#define CIRCULARE_SIZE_RX 512
//#define RASP_BUFSIZE	256
//#define EI485BUFSIZE	256
#define USBBUFFERS 		64
#endif

#define PITCH_BEND      0xe0
#define NULL_THRESHOLD 5
#define DATA1	1
#define DATA2	2
#define DATA3	3
#define DATA4	4

//#define LCD_CUE_SIZE 64
#define MUTE_SEUIL	0x03A5
#define CHASE_STEPS 30

#define TCBUFFERSIZE 32
#define SYSEXBUFFERSIZE 64

#define RX_BUFFER 0
#define TX_BUFFER 1
#define OFF_LINE 0 
#define ON_LINE 1

#define HUI_MODE 0
#define MACKIE_CTRL_MODE 1

#define CTRL 0
#define VOLUME 1
#define OTHER 2
#define KEY_STROCK 3

#define STATUS 			0x80
#define NOTE_OFF		0x80
#define NOTE_ON			0x90
#define POLY_PRESSURE	0xa0
#define CTRL_CHNG		0xb0
#define PROG_CHNG		0xc0
#define CHAN_PRES		0xd0
#define PITCH_W			0xe0
#define SYSEX			0xf0
#define MTC 			0xf1
#define SPP 			0xf2
#define SNG_SEL 		0xf3
#define RESV1 			0xf4
#define RESV2 			0xf5
#define TUNE 			0xf6
#define EOSYSEX			0xf7

#define MUTE_ON			0x42
#define MUTE_OFF		0x02
#define SOLO_ON			0x43
#define SOLO_OFF		0x03
#define AUTO_ON			0x44
#define AUTO_OFF		0x04

#define ECHO_ON			0x01
#define ECHO_OFF		0x00

#define UART_OFF		U1MODE 	= 0x0000
#define UART_ON			U1MODE 	= 0x8000

#define RX_BUFFER_ADDRESS_TAG
#define TX_BUFFER_ADDRESS_TAG
#define MIDI_EVENT_ADDRESS_TAG

void Overflow_Process(void);
unsigned char Init_var(void);
unsigned char Init_io(void);
unsigned char Init_Timers(void);
void Init_Timer2(void);
void LoadEEPROMConfigTAI(void);
void Save_TAI_Config(void);

char nextSlave(unsigned char *Running_Core, unsigned char *slave_numb);
void MixDataProcess(unsigned char Running_Core,unsigned char slave_numb);
void RecallDataProcess(unsigned char slave_numb);
void SaveToEEpromTest(void);

void LockTrackDispatcher(unsigned char Running_Core,unsigned char slave_numb);
void Hui_On_Line_Manager(unsigned char cable_num);
void Update_fader_postition_HUI(unsigned char slave_n,unsigned char chan);
void Set_Automation_Off_HUI(void);
void Set_Automation_Off_MCU(void);
void Reset_auto(unsigned char chan, unsigned char slave_n);
void Restore_Automation(void);
void RefreshManager(void);
void Hui_On_Line_Midi_Parser(unsigned char slave_num);
unsigned char OTB_Midi_Parser_HUI_fader_n_Switch(unsigned char Core_Number);
unsigned char ITB_Midi_Parser_HUI_fader_n_Switch(unsigned char Core_Number);
unsigned char Midi_Parser_TAI_Fader_n_Switch(void);
unsigned char INJEKTOR_Midi_Parser(void);
unsigned char ProcessInput(void);
unsigned char FlushUsbBuffer(void);
unsigned char ProcessOutput(unsigned char Slave_num);

void USB_DBM_ISR( BYTE USTAT_local);
void Delais(unsigned int tic_ms);

unsigned char RS485_Lcd_Comm(unsigned char slave_numb);
unsigned char RS485_Comm_Rx_Manager(void);

void Init_Midi_Msg(unsigned char n);
void ClearMidiTXBuffer(void);
void DoStatsBits(void);
void Init_Rx_Midi_Buffer(unsigned char buffer_num);
void Init_Tx_Midi_Buffer(unsigned char buffer_num);

unsigned short Get_Rx_Midi_Quant(unsigned char buffer_num);
unsigned short Get_Tx_Midi_Quant(unsigned char buffer_num);
void Push_Usb_To_Rx_Midi_Buffer(WORD handlesize_index,unsigned char Usb_buffer_number1);
unsigned char Buffer_To_Usbmidi(unsigned char Buffer_num);
unsigned char Pull_Tx_Midi_Data(unsigned char n, unsigned char buffer_num);
void Build_Midi_Msg(unsigned char mode, unsigned char message_type,
					unsigned char Slave_Num,unsigned char chan, 
					unsigned char data_1,
					unsigned char data_2,
					unsigned char data_3,
				 	unsigned char data_4);

unsigned char Recall_It_Detect(unsigned char slave_numb);
//unsigned char Recall_It_Process(unsigned char slave_numb);

unsigned char NextRaspBank(void);
void TriggerConfigToRasp(void);
void ResetRaspfader(void);
void NextSPISlave(void);
void TcMsgParser(void);
void SysexParser(void);
void DoRaspComm(void);
void DoKbdComm(void);
void SendSingleRaspMsg(void);
void RaspMsgMaker(void);
void SysBootingMsgMaker(void);
void SetLAPS1(unsigned short time);
void SetLAPS2(unsigned short time);
/** PRIVATE PROTOTYPES *********************************************/
static void InitializeSystem(void);
void UserInit(void);
//void YourLowPriorityISRCode();
void USBCBSendResume(void);

USB_AUDIO_MIDI_EVENT_PACKET midiData MIDI_EVENT_ADDRESS_TAG;

typedef struct circ_midi_rx_buffer
	{
	unsigned char code[CIRCULARE_SIZE_RX];
	unsigned char status[CIRCULARE_SIZE_RX];
	unsigned char data1[CIRCULARE_SIZE_RX];
	unsigned char data2[CIRCULARE_SIZE_RX];
	unsigned short size;
	unsigned short top;
	unsigned short bot;
	unsigned short quantity;
	unsigned short Maxquantity;
	}circ_midi_rx_buffer;

typedef struct circ_midi_tx_buffer
	{
	unsigned char code[CIRCULARE_SIZE_TX];
	unsigned char status[CIRCULARE_SIZE_TX];
	unsigned char data1[CIRCULARE_SIZE_TX];
	unsigned char data2[CIRCULARE_SIZE_TX];
	//unsigned char cable_num[CIRCULARE_SIZE_TX];
	unsigned short size;
	unsigned short top;
	unsigned short bot;
	unsigned short quantity;
	unsigned short Maxquantity;

	}circ_midi_tx_buffer;

typedef struct midi_message
	{
	unsigned char code;
	unsigned char status;
	unsigned char data1;
	unsigned char data2;
	unsigned char cable_num;
	unsigned char msg_count;
	unsigned char data_number;
	}midi_message;

typedef struct USB_Strct
	{
	unsigned char top;
	unsigned char bot;
	unsigned char size;
	unsigned char quantity;
	unsigned char maxquantity;
	}USB_Strct;

struct circ_midi_rx_buffer RxMidiBuffer;
struct circ_midi_tx_buffer TxMidiBuffer;
struct midi_message RXmidi_msg;
struct midi_message TXmidi_msg;

 
struct USB_Strct Usb_buffer;
//unsigned char Usb_buffer_number;
unsigned char MaxUsb_buffer_number;

unsigned char Parser_usb_buffer_number;
void USB_disconnect_sequence(void);
USB_HANDLE USBTxHandle = 0;
USB_HANDLE USBRxHandle = 0;
WORD handle_count;

USB_VOLATILE BYTE msCounter;

/*VARIABLE GLOBALES*/
static BYTE hi;
static BYTE lo;
// char AbsSlaveToSlaveIndex[] ={0,1,2,3,0,1,2,3,0,1,2,3};

const char BitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/*******************************************************************************************/ 
//variables globales
/*******************************************************************************************/ 
/*******************************************************************************************/ 
/*les buffers*/
/*******************************************************************************************/ 
//WORD USB_TEMP_BUFFER[32];
unsigned char update_counter;
//unsigned char Recallit_Ctrl_bank[5][2];
unsigned char Recall_value_count;

unsigned char stateBits;
unsigned char Pulsar;
unsigned short PulsarCnt;
unsigned short ResetTRIG;
unsigned char USBSTATE;
unsigned char MAX_SLAVE;
//unsigned char Core_number;
unsigned char Real_Slave_Number;
unsigned char MAX_Core_Number;
unsigned char prev_connected_state;
unsigned char MasterChanAssNum;
//--------------------------------------------------------
//THE FLAGS
//--------------------------------------------------------
unsigned char BootStateFlag;
unsigned char LastBootStateFlag;

unsigned short Buffer_Over_Run;
unsigned short global_error_flag;
//unsigned char testflag; 
unsigned char MidiActFlag;
unsigned char MidiHeartbeatFlag;
unsigned char New_Core_Flag;
unsigned char deviceDetachFlag;
unsigned char SPI1InitFlag;
unsigned char USBStateFlag;
unsigned char DAWStateFlag;
unsigned char Save_Config_To_EEprom_Flag;
//unsigned char USB_interupt_flags;
unsigned char Master_n_Slave_Active_Flags[3];
unsigned char GOTO_FLAG;
unsigned char Recall_Skip_Flag;
//unsigned char Initialized_flag;
unsigned char debugModeFlag;
unsigned char Slave_Active_Flag[4]={0,0,0,0};
unsigned char status_flag[4] ={0,0,0,0};
unsigned short Rasp_Heartbeat_Flag;
unsigned char RaspRefreshFlag;
unsigned char FADER_UPDATE_FLAG[3][4];
unsigned char MUTE_UPDATE_FLAG[3][4];
unsigned char DAW_on_line_flag[5];
unsigned char automode_flag;
unsigned char SPI1_SYNC_FLAG;
unsigned char sysex_complete_flag;
//unsigned char WAIT_FOR_ACK_FLAG;
unsigned char Tx_tc_flag;
unsigned char Snapshot_stop_FLAG;
unsigned char Update_stop_FLAG;
unsigned char NewQFrameFlag;
unsigned char NewSysexMsgFlag;
unsigned char SRAM_write_flag;
unsigned short SnapShotCntr;
unsigned char SRAM_status_data_flag;
unsigned char Recall_it_flag;
unsigned char Recall_it_flow_flag;
unsigned char RefreshFlag;
unsigned char SetAllWaitFlag;
unsigned char Set_All_Auto_Flag;
unsigned char SetOffLineCnt;
unsigned char Grp_flag;
unsigned char Tx485_Msg_trig_flag;
unsigned char Software_auto_flag;
unsigned char ScanActiveSlaveFLAG;
unsigned char copy_automode_flag;
unsigned char Set_all_flag;
unsigned char Process_flag;
unsigned char SendHeartbeatFlag;
unsigned char New_Returned_Pot_n_switch_number_flag;
unsigned char Runing_Status_Flag;
unsigned char New_Rx485_flag;
unsigned char EXP_Config_Reg_Request_FLAG;
unsigned short SRAM_Spi_Flag;
unsigned char Ack_flag_response;
unsigned char RaspDataTxFlag;
unsigned char test_flag;
unsigned char GlobalAutomodeFlag;
unsigned char GotoOnStopFlag;
unsigned short debug_flag=0;;
unsigned char NewRaspMsgFlag;
unsigned char TAI_Ping_Flag;
unsigned char UDP_Ping_Flag;
unsigned char REAPER_FLAG;
unsigned char HUI_FLAG;
unsigned char UDP_FLAG;
unsigned char readflag;
unsigned short RecallRtnCnt;
unsigned char OverideMask;
unsigned char StopCommFlag;
unsigned char configCnt;
unsigned char gotoMixOffFlag;
unsigned char gotoMixOnFlag;
unsigned char GotoBootloadFlag;
//--------------------------------------------------------
//THE BUFFERS
//--------------------------------------------------------
unsigned int NoTtxCntr;
unsigned int NoTtxCntr;

unsigned char fromFaderTouchState[3][4];

unsigned char FADER_AUTOMODE_STATE[3][4][8];
unsigned char SavedMotorStatus[3][4];
unsigned char SavedVCAStatus[3][4];
unsigned char SavedAltStatus[3][4];
unsigned char SavedGrpIsoStatus[3][4];
unsigned char UDPActiveChannelFlag[3][4];

unsigned char ToMF_TRStatus;
unsigned char MFSetupModeReg;

unsigned char TrackLockedState[3][4];
unsigned char LastTrackLockedState[3][4];
unsigned char GostTrackLockedState[3][4];
unsigned char TrackLockedMode;

unsigned char midi_parser_msg_count[4];
unsigned char midi_parser_swt_msg_count[4];

unsigned char DAW_on_line_cntr[4];
unsigned char sysex_buffer[16];
unsigned char midi_data_count[4] ={0,0,0,0};
unsigned char cable_out_num=0;
unsigned char TC_DATA[12];
unsigned char AltReceivedDataBuffer[64];

unsigned char ReceivedDataBuffer[64] RX_BUFFER_ADDRESS_TAG;
unsigned char USBDataBuffer[USBBUFFERS][64];
unsigned char USBDataBufferQuant[USBBUFFERS];

unsigned char Rasp_fader_num;
unsigned char Rasp_fader_msb_data;
unsigned char Rasp_fader_lsb_data;
unsigned char scan_start,scan_stop;


unsigned char swt_data1[4];
unsigned char swt_data2[4];
unsigned short max_cnt;

unsigned char LCD_MSG_PASSER;

unsigned short SlaveVersion;
unsigned short MasterVersion;
unsigned short TempVersion;
unsigned short spi2_cnt;
unsigned char USb_buffer_index;
unsigned char active_slave_pass_count;

unsigned char auxDataIndex;
unsigned char auxData1;
unsigned char auxData2;
unsigned char auxData3;
unsigned char auxData4;
unsigned char kbdData;

unsigned char pref_raspdata;
unsigned char Sampled_Address;

unsigned char Slave_Flushed_Test;
unsigned char trig_rcvd[4];

unsigned char slave_Automode_index;
unsigned char tx_cnt;
unsigned char tx_cnt_max;

unsigned char msg_cnt;
unsigned char Msg_addresse;

unsigned char Tx_USB_config_index;
unsigned char Tx_USB_config_data;
unsigned char Tx_EXP_config_index;
unsigned char Tx_EXP_config_data;
unsigned char Tx_RASP_config_index;
unsigned char Tx_RASP_config_data;

unsigned char RaspRxData;

unsigned char Spi_Usb_buffer_number;
unsigned char blackListChannel[12];

unsigned short RaspRxBufQnt;
unsigned char GrpMasterNumber;
unsigned char GrpSlaveNumber;

unsigned char TX485_Heart_Beat_cnt;
unsigned int RefreshTimer;
//unsigned int Timeout;
unsigned short LAPS1;
unsigned short LAPS2;

unsigned char Int_Slave_Num;
short int DAW_ONLINE_Timer;
unsigned char Master_data;
unsigned int fader_val,tic;
unsigned char VCA_Bank_Location;
unsigned char HUI_CORE_focus;
unsigned char EXP_Config_Reg_Tx_Count;
unsigned char overflow;
unsigned short usb_overflow;
unsigned char Fader_move_init_msg=0;
unsigned char Grp_state=0;

unsigned char ctrl_mode;
char Global_null_threshold;

unsigned char RecallPassCnt;
unsigned char RecallStopPassCnt;

WORD sizeofhandle;
WORD handlesize_index;
WORD handle_count;

unsigned char Set_all_counter;

unsigned char Data_entry_cnt;
unsigned char Snd_echo_char;
unsigned char Bank_slide_mask;
unsigned char mainLoopCore;
unsigned char mainLoopSlave;

unsigned char RecallActiveCore;

unsigned char TX_LAST_NRPN_MSB;
unsigned char TX_LAST_NRPN_LSB;
unsigned char RX_LAST_NRPN_MSB;
unsigned char RX_LAST_NRPN_LSB;
unsigned char Running_channel;
unsigned char Runing_slave;
unsigned char configured,connected;
unsigned char RecallActiveSlave;
unsigned char Saved_RecallActiveSlave;


unsigned char Recall_Chanel_index;
unsigned char first_switch_cnt;

unsigned char pass_cnt;

unsigned short Data_cnt;

unsigned char Stop_fla;

unsigned char Pot_n_switch_number;
unsigned short Returned_Pot_n_switch_number;

unsigned char FullRecallScan;
unsigned char Max_Recallit_Window;
unsigned char Min_Recallit_Window;
unsigned char Last_Max_Recallit_Window;
unsigned char Last_Min_Recallit_Window;
unsigned short Last_Max_Recallit_Value;
unsigned short Last_Min_Recallit_Value;
unsigned char Last_Max_Recallit_Ctrl;
unsigned char Last_Min_Recallit_Ctrl;

//------------------------------------------------------------------------------------
unsigned char temp_config_data1,temp_config_data2;
// Cette matrice gère les registres de code de configuration pour le master et les elements externes
// la position [x][0] est un index qu'il y a un changement au regitre pointé par la valeur [x][0]
// "x" est le slave concerné
// "y" est le register
// l'index [x][0] doit est mi a une valeur de 0 apres modification
//les registres assignés sont les suivants
// [0][1] registre gérant le stop flag venant du DAW pour le [change to automation mode "X" on stop]
// [0][2] Registres gérant le Recall_it mode
//------------------------------------------------------------------------------------

unsigned char stop;
unsigned int delaycnt;
unsigned char data_count;
unsigned char uart_first_byte=0;
unsigned short RxData;
unsigned char LAST_SLAVE;
unsigned char SLAVE;
unsigned char SLAVE_INDEX;


unsigned char Usb_unparsed_buffer_cnt;
unsigned char spi_buffer_number;
unsigned char spi_data_count;
unsigned char usb_data_count;

unsigned char SRAM_Tx_Data_Cnt;
unsigned char SRAM_Rx_Data_Cnt;

unsigned char KBD_DONGLE_ONLINE;
unsigned short LCD_detect_counter;

unsigned char SysexBufferTop;
unsigned char SysexBufferBot;
unsigned char SysexBufferQuantity;
unsigned char sysex_cntr;
unsigned char SysexBufferSize;
unsigned char SysexBuffer[SYSEXBUFFERSIZE];
unsigned char SysexMsgQuantity;

unsigned char SysexIndex;

unsigned char TcBufferQuantity;
unsigned char TcBufferSize;
unsigned char TcBuffer[32];
unsigned char TcBufferTop;
unsigned char TcBufferBot;
unsigned char TcMsgQuantity;

unsigned char frame_u;
unsigned char frame_d;
unsigned char sec_u;
unsigned char sec_d;
unsigned char min_u;
unsigned char min_d;
unsigned char hour_u;
unsigned char hour_d;

unsigned char count_msg_Rx2;
unsigned short Recallit_delay;
unsigned char Recall_mode;

unsigned char ConfigIndexCntr;
unsigned short Tx485_Data;

unsigned short midi_msg_temp_data;
unsigned short Sender_Message_address;
unsigned char Selected_Chanel_Number;

unsigned short Recall_it_data;

unsigned char Play_delay_cnt;
unsigned short Total_ctrl_cnt1;
unsigned char Temp_ctrl_nmb;

unsigned short usb_buffer_cnt;
unsigned short usb_buffer_process_cnt;
unsigned char channel_cnt;
unsigned char Active_Recall_Slave;


unsigned int interrupt_counter;

unsigned char sysex_index;
unsigned char Message_Code;
//unsigned char SPI_scan_cnt;
//unsigned char master_slave_n;

//unsigned char RaspMsgType;
unsigned char RaspSlaveIndex;
unsigned char RaspCoreIndex;
unsigned char SavedTxVal;
unsigned char config_index;

unsigned char MulticoreRecallChannel;
unsigned char RaspCommInited;

unsigned char SlaveMatrix[3][4];
unsigned char CoreMatrix[3][4];
unsigned char DecodedSlaveMatrix[3][4];
unsigned char DecodedCoreMatrix[3][4];

unsigned char BankAutomode[12];
unsigned char GlobalAutomodeCnt;
unsigned char LastMode;
unsigned char NewMode;
unsigned char AtlFdrDataCntr;

unsigned char CoreChanNbr[3];
unsigned short ChannelNbrTest;
unsigned char FaderActifs[3][4];
unsigned char Tx485MsgStatus;
unsigned char ChanLinkState[3][4];
unsigned char Autochan;
