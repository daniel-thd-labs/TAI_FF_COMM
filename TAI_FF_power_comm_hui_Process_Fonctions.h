//#include "TAI_V2_Versionner.h"

#define RX_BUFFER 0
#define VOLUME_CODE 0
#define MUTE_CODE 1
#define AUTOMODE_CODE 2
#define CONTROL_CODE 3
#define ACTIVE_SENS 1

#define TAI_CODE 3
#define NRPN_MSB 0x63
#define NRPN_LSB 0x62
#define DATA_ENTRY_MSB 0x06
#define DATA_ENTRY_LSB 0x26

//extern unsigned char global_error_flag;
extern unsigned char Slave_Present_FLAG[12];

//extern unsigned short VCA_IN_BUF[3][4][8];				//Données transmises aux slaves
//extern unsigned short DAW_IN_BUF[3][4][8];
//extern unsigned char LastFaderGroup[3][4][8];
//extern short FaderGroupRefVal[3][4][8];
//extern unsigned short SSL_FaderBuffer[3][4][8];		//Données reçues des slaves
//extern unsigned short SPI_BUF_SENT[3][4][8];
//extern unsigned short TEMP_AD_Recall_It_Buffer[8];			//Données de calcule et réference

extern unsigned char FaderCutSwitch[3][4];
extern unsigned char dawCutState[3][4];
extern unsigned char SSLMuteState[3][4];

extern unsigned char FADER_UPDATE_FLAG[3][4];
extern unsigned char MUTE_UPDATE_FLAG[3][4];
extern unsigned char FADER_AUTOMODE_STATE[3][4][8];
extern unsigned char TRIM_AUTOMODE_STATE[3][4];
//extern unsigned char spi_automode_buffer[3][4][3];
//extern unsigned char automode_buffer[3][4][3];

//extern unsigned char EXP1_ALT_AD_Flag[4][8];

extern unsigned char Config_registers[128];
extern unsigned char Config_registers_index;
extern unsigned char Config_registers_data;

extern unsigned char TX_LAST_NRPN_MSB;
extern unsigned char TX_LAST_NRPN_LSB;
extern unsigned char RX_LAST_NRPN_MSB;
extern unsigned char RX_LAST_NRPN_LSB;
extern unsigned char Runing_Status_Flag;
extern unsigned char Recall_it_flow_flag;


extern unsigned char Recall_value_count;

extern unsigned char Set_All_Auto_Flag;
extern unsigned char Set_All_Auto_Mode;	
extern unsigned char Set_all_counter;
extern unsigned char VCA_Bank_Location;
extern unsigned char HUI_CORE_focus;
extern unsigned char MasterChanAssNum;
extern unsigned char Refresh_flag;
extern unsigned char SetAllWaitFlag;
extern unsigned char Pot_n_switch_number;
extern unsigned short Returned_Pot_n_switch_number;
extern unsigned char Prev_Sampled_Address;
extern unsigned short Recallit_delay;
extern unsigned char RecallActiveCore;
extern unsigned char New_Returned_Pot_n_switch_number_flag;

extern unsigned char Pull_Rx_Midi_Data(void);
extern unsigned char midi_parser_msg_count[4];
extern unsigned char swt_data1[4];
extern unsigned char swt_data2[4];
extern unsigned char HUI_FLAG;
extern unsigned char DAW_on_line[4];

extern unsigned short Get_Rx_Midi_Quant(unsigned char buffer_num);
extern unsigned short Get_Tx_Midi_Quant(unsigned char buffer_num);

extern unsigned char OTB_Midi_Parser_HUI_Fader_n_Switch(unsigned char Core_Number);
extern unsigned char Recall_Running_Slave;
extern unsigned char Recall_mode;
extern unsigned char configured,connected;
extern unsigned char Bank_slide_mask;

extern unsigned char MulticoreRecallChannel;
extern unsigned char AbsRecallActiveSlave;
extern unsigned char MulticoreRecallChannelIndex;;

extern unsigned char UF_WasTouchedState[3][4];
extern unsigned char FromUF_TouchState[3][4];
extern unsigned char FromUF_FaderCutSwitch[3][4];
extern unsigned char FromMF_GrpAssign[3][4][8];
extern unsigned char FromMF_MotorStatus[3][4];

extern unsigned char ToMF_MotorStatus[3][4];
extern unsigned char ToMF_VCAStatus[3][4];
//extern unsigned char ToMF_AltStatus[3][4];
extern unsigned char ToMF_SoloIsoStatus[3][4];
extern unsigned char ToMF_TRStatus;

extern void Build_Midi_Msg(unsigned char mode, unsigned char message_type,
					unsigned char Slave_Num,unsigned char chan, 
					unsigned char data_1,
					unsigned char data_2,
					unsigned char data_3,
				 	unsigned char data_4);

//extern unsigned int Scale_It_TAI(unsigned int data);
extern unsigned char ProcessOutput(unsigned char Slave_num);
extern void Recall_it_Parser(void);
extern unsigned int Scale_It_Alt1(unsigned int data);


extern unsigned char Push_Tx_Midi_Data(unsigned char n,unsigned char buffer_num);

extern unsigned char SPI_Comm(unsigned char slave_n);
extern unsigned char ITB_Midi_Parser_HUI_Fader_n_Switch(unsigned char slave_num);
extern void CORE_Config_Manager(unsigned char Reg_index,unsigned char Reg_data, unsigned char source);
extern void Delais(unsigned int tic_ms);

void Main_Process_Loop_HUI(unsigned char Core_Number,unsigned char slave_num);
void UF_HUI_Touch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char channel,unsigned char state);

void Auto_Mode_Transmit_HUI(unsigned char slave_n,unsigned char Core_Number);
void Set_All_Automation_Mode_HUI(unsigned char ext_automode);
void Set_Automation_Off_HUI(void);
void Do_Bank_Slide_HUI(unsigned char slide_data,unsigned char source);

void HUI_Fader_And_Mute_Data(unsigned char slave_n,unsigned char Core_Number);
void HUI_UF_Fader_And_Mute_Data(unsigned char slave_n,unsigned char Core_Number);

void Update_fader_postition_HUI(unsigned char slave_n,unsigned char chan);
void Recall_It_HUI_Fader_And_Mute_Data(unsigned char slave_n);
unsigned char Recall_It_Process(unsigned char slave_numb);

void Refresh_Auto_Mode_HUI(unsigned char slave_num,unsigned char chnl_rfrsh_nmbr);
void Alt_Refresh_Auto_Mode_HUI(unsigned char slave_n);
void HUI_Latch_Auto_Mode_Transmit(unsigned char Master_Number,unsigned char slave_n, unsigned char channel);

void set_ITB_mode(void);
