//#include "p24FJ64GB002.h"
#define SET_ALL_DELAY 8

//#define MASTER_RELAIS 		PORTBbits.RB15
//#define MASTER_RELAIS_DIR 	TRISBbits.TRISB15

void ConfigTest(void);
void ConfigDataManager(unsigned char Reg_index,unsigned char Reg_data, unsigned char source);
char Rx_Config_Data_Manager(unsigned char reg_index,unsigned char reg_data,unsigned char source);
unsigned char Get_System_Size(void);
unsigned char TrackLockManager(unsigned char mode);

void SaveAutoState(void);
void LoadAutoState(void);
void ClearDawCutState(void);
void SetAllAutoOff(void);
extern unsigned char GrpMasterNumber;
extern unsigned char GrpSlaveNumber;

//unsigned int Null_Adjust_Fader(	unsigned char slave_n,unsigned char chan);
extern unsigned char Set_All_Auto_Flag1;

//extern unsigned short global_error_flag;
extern unsigned char slave_Automode_index;
extern unsigned char Config_registers[128];
extern unsigned char Config_registers_index;
extern unsigned char Config_registers_data;
extern unsigned char blackListChannel[12];
extern unsigned char debugModeFlag;
extern unsigned char Tx_USB_config_index;
extern unsigned char Tx_USB_config_data;
extern unsigned char Tx_EXP_config_index;
extern unsigned char Tx_EXP_config_data;
extern unsigned char Tx_EXT_config_index;
extern unsigned char Tx_EXT_config_data;
extern unsigned char Tx_RASP_config_index;
extern unsigned char Tx_RASP_config_data;
extern unsigned char SetAllWaitFlag;
extern unsigned char MAX_Core_Number;
extern unsigned char Slave_Present_FLAG[12];
extern unsigned char Rx_USB_config_index;
extern unsigned char Rx_USB_config_data;
extern unsigned char Rx_LCD_config_index;
extern unsigned char Rx_LCD_config_data;
extern unsigned char Rx_EXT_config_index;
extern unsigned char Rx_EXT_config_data;
extern unsigned char Rx_RASP_config_index;
extern unsigned char Rx_RASP_config_data;
extern unsigned char Recall_mode;
extern unsigned char Recall_it_flow_flag;
extern unsigned char RecallActiveSlave;
extern unsigned char Recall_Chanel_index;
extern unsigned char Last_Recall_Runing_Chanel;
extern unsigned char MAX_SLAVE;
extern unsigned char MAX_Core_Number;
extern unsigned char VCA_Bank_Location;
extern unsigned char RecallActiveCore;
extern unsigned char UDPActiveChannelFlag[3][4];
extern unsigned char Modules_Section_start[7];
extern unsigned char Modules_Section_stop[7];

extern unsigned char WebRecallPassCnt;
extern unsigned char RecallPassCnt;
extern unsigned char WebRecallStopPassCnt;
extern unsigned char RecallStopPassCnt;


extern unsigned char Master_n_Slave_Active_Flags[3];
extern unsigned short Globale_Slave_Active_Flags;
extern void Set_Automation_Off_TAI(void);
extern void Set_Automation_Off_HUI(void);
extern void Set_Automation_Off_MCU(void);
extern unsigned char overflow;
extern void Do_TAI_Scale(void);
extern void Do_Pt8_9_10_Scale(void);
extern void Do_Pt11_Scale(void);

extern void Do_MCU_Scale(void);
extern void Do_TAI_Scale(void);
extern unsigned char Set_All_Auto_Flag;
extern unsigned char Set_All_Auto_Mode;
extern unsigned char Set_all_counter;

extern void Set_All_Automation_Mode_HUI(unsigned char ext_automode);
extern void Do_Bank_Slide_HUI(unsigned char slide_data,unsigned char source);
extern unsigned char pass_cnt;
extern unsigned char HUI_Group_focus;
extern unsigned char Save_Config_To_EEprom_Flag;
extern unsigned char Recall_Section_Index;

extern unsigned char MulticoreRecallChannel;
extern unsigned char AbsRecallActiveSlave;
extern unsigned char MulticoreRecallChannelIndex;

extern unsigned char SlaveMatrix[3][4];
extern unsigned char CoreMatrix[3][4];
extern unsigned char DecodedSlaveMatrix[3][4];
extern unsigned char DecodedCoreMatrix[3][4];

extern unsigned char TrackLockedState[3][4];
extern unsigned char GostTrackLockedState[3][4];
extern unsigned char TrackLockedMode;

extern unsigned char test_flag;
extern unsigned char NewMode;
extern unsigned char startmsgflag;

extern unsigned char nextRecallmode;
//extern unsigned char RaspMsgType;
extern unsigned short SlaveVersion;
extern unsigned short MasterVersion;
extern unsigned short TempVersion;
extern unsigned char MFSetupModeReg;
extern unsigned char savedSpiAutomodeBuffer[3][4][3];
