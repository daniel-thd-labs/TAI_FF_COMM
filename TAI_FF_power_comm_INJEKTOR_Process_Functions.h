#define RX_BUFFER 0
#define UDPVOLUME_CODE 0x10
#define UDPMUTE_CODE 0x11
#define UDPAUTOMODE_CODE 0x12
#define UDPCONTROL_CODE 0x13
#define UDPTOUCH_CODE 0x14
#define UDPLOCK_CODE 0x15
#define UDPVCA_CODE 0x16
#define ACTIVE_SENS 1
#define UDPPING_CODE 0x7f

#define UDP_CODE 3
#define NRPN_MSB 0x63
#define NRPN_LSB 0x62
#define DATA_ENTRY_MSB 0x06
#define DATA_ENTRY_LSB 0x26
#define UDPMIDI_CABLE 4

extern unsigned char stop;
extern unsigned char MasterChanAssNum;
extern unsigned char blackListChannel[12];
extern unsigned char Runing_slave;
extern unsigned char SetAllWaitFlag;
extern unsigned char Slave_Present_FLAG[12];
extern unsigned char MAX_Core_Number;
extern unsigned char VCA_Bank_Location;
extern unsigned char Bank_count;
extern unsigned char UDP_Ping_Flag;
extern unsigned char UDP_FLAG;
//extern unsigned char Recallit_Ctrl_bank[5][2];
extern unsigned char Recall_it_flow_flag;
extern unsigned char TrackLockedMode;
extern unsigned char Recall_value_count;
extern unsigned char DAW_on_line_flag[5];

extern unsigned short DAW_IN_BUF[3][4][8];
extern unsigned short SPI_BUF_SENT[3][4][8];
extern unsigned short TEMP_AD_Recall_It_Buffer[8];

extern unsigned char FaderIsGrouped[3][4];

extern unsigned char FADER_AUTOMODE_STATE[3][4][8];
extern unsigned char FADER_UPDATE_FLAG[3][4];
extern unsigned char MUTE_UPDATE_FLAG[3][4];
extern unsigned char TRIM_AUTOMODE_STATE[3][4];
extern unsigned char SetAllAutomodeBuffer[3][4][3];

extern unsigned char Config_registers_index;
extern unsigned char Config_registers_data;
extern unsigned char New_Returned_Pot_n_switch_number_flag;

extern unsigned char TX_LAST_NRPN_MSB;
extern unsigned char TX_LAST_NRPN_LSB;
extern unsigned char RX_LAST_NRPN_MSB;
extern unsigned char RX_LAST_NRPN_LSB;

extern unsigned char Set_All_Auto_Flag;
extern unsigned char SetOffLineCnt;

extern unsigned char RefreshFlag;
extern unsigned int RefreshTimer;

extern unsigned char Prev_Sampled_Address;
extern unsigned short Recallit_delay;
extern unsigned char RecallActiveCore;

extern unsigned char Pull_Rx_Midi_Data(void);
extern unsigned char midi_parser_msg_count[4];
extern unsigned char swt_data1[4];
extern unsigned char swt_data2[4];

extern unsigned char DAW_on_line[4];
extern unsigned char configCnt;
extern unsigned char MulticoreRecallChannel;
extern unsigned char AbsRecallActiveSlave;
extern unsigned char RecallChannelIndex;

extern unsigned char FromMF_GrpAssign[3][4][8];
extern unsigned char FromMF_MotorStatus[3][4];

extern unsigned char ToUF_MotorStatus[3][4];
extern unsigned char ToMF_VCAStatus[3][4];
extern unsigned char ToMF_SoloIsoStatus[3][4];
extern unsigned char ToMF_TRStatus;
extern unsigned char FaderActifs[3][4];

extern void Build_Midi_Msg(unsigned char mode, unsigned char message_type,
					unsigned char Slave_Num,unsigned char chan, 
					unsigned char data_1,
					unsigned char data_2,
					unsigned char data_3,
				 	unsigned char data_4);

extern unsigned char ProcessOutput(unsigned char Slave_num);

void Main_Process_Loop_UDP(unsigned char master,unsigned char slave);

void UDP_FF_FaderAndMuteDataMng(unsigned char master,unsigned char slave, unsigned char chan);
void UDP_FF_Touch_Auto_Mode_Transmit(unsigned char master,unsigned char slave, unsigned char chan,unsigned char state);
void UDP_Latch_Auto_Mode_Transmit(unsigned char master,unsigned char slave, unsigned char chan);

//void Do_TAI_Mute(unsigned char mute_value);
void UDP_Auto_Mode_Transmit(unsigned char master,unsigned char slave,unsigned char chan);
void UDPVcaModeSelect(unsigned char master,unsigned char slave,unsigned char chan,unsigned char automode);
void UDP_PrepAutModeData(unsigned char master,unsigned char slave,unsigned char chan);
void UDP_MasterAutoModeTransmit(unsigned char Automode);
void UDP_MF_TrimModeManager(unsigned char master,unsigned char slave,unsigned char ChanBit,unsigned char mode);
void UDP_Latch_Auto_Mode_Transmit(unsigned char master, unsigned char slave,unsigned char chan);
void UDP_Ping(void);
void UDP_On_Line_Manager(unsigned char status);
void motorManager(unsigned char chan);

void ManageAutmodeOnStop(unsigned char master,unsigned char slave,unsigned char chan);

extern void Delais(unsigned int tic_ms);
extern unsigned short Get_Rx_Midi_Quant(unsigned char buffer_num);
extern unsigned char INJEKTOR_Midi_Parser(void);
extern unsigned char Midi_Parser_TAI_Fader_n_Switch(void);
extern unsigned char Recall_mode;
extern unsigned char configured,connected;
extern unsigned char RecallActiveSlave;
extern unsigned char Pot_n_switch_number;
extern unsigned short Returned_Pot_n_switch_number;
extern unsigned char UDP_Ping_Flag;

extern unsigned char BankAutomode[12];
extern unsigned char GlobalAutomodeCnt;
extern unsigned char GlobalAutomodeFlag;
extern unsigned char MAX_SLAVE;
extern unsigned char GotoOnStopFlag;
extern unsigned char LastMode;
extern unsigned char NewMode;
extern unsigned char UDPActiveChannelFlag[3][4];

//extern void SUBCORECODE_Config_Manager(unsigned char Reg_index,unsigned char Reg_data, unsigned char source);
