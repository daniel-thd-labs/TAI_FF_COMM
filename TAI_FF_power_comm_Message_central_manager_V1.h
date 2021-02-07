#include "TAI_FF_Versionner.h"

//#define BAUDRATE        115200   
//#define BAUDRATE2       250000
//#define BRGVAL_RASP    ((FCY/BAUDRATE)/16)-1
//#define BRGVAL2         ((FCY2/BAUDRATE2)/16)-1

//#define UART2_RTS_DIR	TRISAbits.TRISA2
//#define UART2_RTS		PORTAbits.RA2

////#define XTFREQ          8000000        		//On-board FRC
//#define PLLMODE         2               		//On-chip PLL setting
//#define FCY             XTFREQ*PLLMODE        //Instruction Cycle Frequency
//#define FCY2            XTFREQ*PLLMODE        //Instruction Cycle Frequency


//#define LCD_CUE_SIZE 64
//#define PULSE PORTAbits.RA3

#define RX_BUFFER 0
#define VOLUME_CODE 0
#define MUTE_CODE 1
#define AUTOMODE_CODE 2
#define CONTROL_CODE 3
#define ACTIVE_SENS 1

#define TAI_CODE 3
#define UDP_CODE 3

#define NRPN_MSB 0x63
#define NRPN_LSB 0x62
#define DATA_ENTRY_MSB 0x06
#define DATA_ENTRY_LSB 0x26
#define VMIDI_CABLE 4
#define UDPMIDI_CABLE 4


#define CONFIG_CODE 0x20

//void Push_Gen_Slave_Data(unsigned char n, unsigned char buffer_num);
//unsigned char Pull_Gen_Slave_Data(unsigned char n, unsigned char buffer_num);
//void Send_Recallit_Comm_Msg(unsigned short Recall_It_Fonction_Code, unsigned short msg_lenght, unsigned short *data_ptn);
//void Send_Recallit_Midi_value(unsigned char Channel_num,unsigned char Ctrl_num,unsigned short Ctrl_value);
//void Get_Ad_Slave_Value(unsigned char Channel_number);

//extern unsigned char Push_Config_data_In_Queue(unsigned char index, unsigned short data, unsigned char source);
//extern void Set_Automation_Off_HUI(void);
//extern void Set_Automation_Off_MCU(void);
//extern unsigned char RS485CommManager(void);
//extern void Build_Midi_Msg(unsigned char mode, unsigned char message_type,
//					unsigned char Slave_Num,unsigned char chan, 
//					unsigned char data_1,
//					unsigned char data_2,
//					unsigned char data_3,
//				 	unsigned char data_4);

extern unsigned char Ack_flag_response;
extern unsigned char TX485_Heart_Beat_cnt;
extern unsigned char KBD_DONGLE_ONLINE;
extern unsigned char SavedRecalMode;
extern unsigned char Recall_mode;
extern unsigned char configCnt;

extern unsigned char gotoMixOffFlag;
extern unsigned char gotoMixOnfFlag;

extern unsigned char MasterChanAssNum;

extern unsigned char Config_registers_index;
extern unsigned char Config_registers_data;
extern unsigned char Recall_it_flow_flag;
extern unsigned char NewRaspMsgFlag;
extern unsigned char RecallActiveSlave;
extern unsigned char Recall_Section_Index;

extern unsigned char Save_Config_To_EEprom_Flag;
extern unsigned char debugModeFlag;
extern unsigned short ResetTRIG;
extern unsigned char USBStateFlag;
extern unsigned char Config_registers[128];
extern unsigned char RaspCommInited;

extern unsigned char REAPER_FLAG;
extern unsigned char HUI_FLAG;
extern unsigned char UDP_FLAG;
extern unsigned char StopCommFlag;
extern unsigned char Autochan;
extern unsigned char MFSetupModeReg;
extern unsigned char GotoBootloadFlag;

void SendTally(char code,char message);
void SSLKbdMsgParser(unsigned char Msg_data);
void HUIMsgParser(unsigned char data);
void WWWMsgParser(unsigned char MsgArray[]);
void SendReaperCmd(unsigned char cmd);
void ClearDawCutState(void);
void midiConfigMsgManager(unsigned char idx,unsigned char data);
void midiSendSingleConfig(unsigned char idx,unsigned char data);
void midiSendGlobalConfig(void);
extern void Delais(unsigned int tic_ms);
void stateRequestManager(unsigned char idx,unsigned char data);
