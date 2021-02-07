//#include "TAI_V2_Versionner.h"
//#include "p24FJ64GB002.h"

#define RX_BUFFER 0
#define VOLUME_CODE 0
#define MUTE_CODE 1
#define AUTOMODE_CODE 2
#define CONTROL_CODE 3
#define PING_CODE 4
#define ACTIVE_SENS 1


#define TAI_CODE 3
#define NRPN_MSB 0x63
#define NRPN_LSB 0x62
#define DATA_ENTRY_MSB 0x06
#define DATA_ENTRY_LSB 0x26
#define VMIDI_CABLE 4
extern unsigned char readflag;

extern unsigned char stop;
extern unsigned char MasterChanAssNum;
extern unsigned char blackListChannel[12];
extern unsigned char Runing_slave;
extern unsigned char SetAllWaitFlag;
extern unsigned char Slave_Present_FLAG[12];
extern unsigned char MAX_Core_Number;
extern unsigned char VCA_Bank_Location;
extern unsigned char Bank_count;
extern unsigned char TAI_Ping_Flag;
extern unsigned char Recall_it_flow_flag;
extern unsigned char TrackLockedMode;
extern unsigned char Recall_value_count;
extern unsigned char DAW_on_line_flag[5];

extern unsigned char GroupedFader[3][4];
extern unsigned char ActiveCutState[3][4];

extern unsigned char FADER_AUTOMODE_STATE[3][4][8];
extern unsigned char FADER_UPDATE_FLAG[3][4];
extern unsigned char MUTE_UPDATE_FLAG[3][4];
extern unsigned char SetAllAutomodeBuffer[3][4][3];

extern unsigned char Config_registers[128];
extern unsigned char Config_registers_index;
extern unsigned char Config_registers_data;
extern unsigned char New_Returned_Pot_n_switch_number_flag;

extern unsigned char TX_LAST_NRPN_MSB;
extern unsigned char TX_LAST_NRPN_LSB;
extern unsigned char RX_LAST_NRPN_MSB;
extern unsigned char RX_LAST_NRPN_LSB;

extern unsigned char Set_All_Auto_Flag;
extern unsigned char Set_All_Auto_Flag1;

extern unsigned char RefreshFlag;
extern unsigned int RefreshTimer;

extern unsigned char Prev_Sampled_Address;
extern unsigned short Recallit_delay;
extern unsigned char RecallActiveCore;

extern unsigned char Pull_Rx_Midi_Data(void);
extern unsigned char midi_parser_msg_count[4];
extern unsigned char swt_data1[4];
extern unsigned char swt_data2[4];
//extern unsigned char DAW_FLAG;
extern unsigned char DAW_on_line[4];
extern unsigned char configCnt;
extern unsigned char MulticoreRecallChannel;
extern unsigned char AbsRecallActiveSlave;
extern unsigned char RecallChannelIndex;

extern unsigned char UF_WasTouchedState[3][4];
extern unsigned char ToUF_MotorStatus[3][4];




extern unsigned char FaderActifs[3][4];

extern void Build_Midi_Msg(unsigned char mode, unsigned char message_type,
					unsigned char Slave_Num,unsigned char chan, 
					unsigned char data_1,
					unsigned char data_2,
					unsigned char data_3,
				 	unsigned char data_4);

extern unsigned char ProcessOutput(unsigned char Slave_num);
void REAPER_FF_Main_Process_Loop(unsigned char master, unsigned char slave);
void REAPER_FF_FaderAndMuteDataMng(unsigned char Master_Number, unsigned char slave_n, unsigned char chan);
void REAPER_FF_Touch_Auto_Mode_Transmit(unsigned char Master_Number, unsigned char slave_n, unsigned char channel,unsigned char state);
void REAPER_FF_Latch_Auto_Mode_Transmit(unsigned char Master_Number, unsigned char slave_n,unsigned char chan);
void REAPER_FF_Auto_Mode_Transmit(unsigned char Master_Number, unsigned char slave_n, unsigned char chan);
void REAPER_FF_MasterAutoModeTransmit(unsigned char Automode);
void ResetFaderGrpRef(unsigned char Master_Number,unsigned char slave_n,unsigned char chan);
void Do_TAI_Mute(unsigned char mute_value);
void TAI_Ping(void);
void TAI_On_Line_Manager(void);



extern void Delais(unsigned int tic_ms);
extern unsigned short Get_Rx_Midi_Quant(unsigned char buffer_num);
extern unsigned char Midi_Parser_TAI_Fader_n_Switch(void);
extern unsigned char Recall_mode;
extern unsigned char configured,connected;
extern unsigned char RecallActiveSlave;
extern unsigned char Pot_n_switch_number;
extern unsigned short Returned_Pot_n_switch_number;
extern void SUBCORECODE_Config_Manager(unsigned char Reg_index,unsigned char Reg_data, unsigned char source);



extern unsigned char BankAutomode[12];
extern unsigned char GlobalAutomodeCnt;
extern unsigned char GlobalAutomodeFlag;
extern unsigned char MAX_SLAVE;
extern unsigned char GotoOnStopFlag;
extern unsigned char LastMode;
extern unsigned char NewMode;

