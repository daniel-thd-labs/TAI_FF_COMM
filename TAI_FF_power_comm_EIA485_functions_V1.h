#include "TAI_FF_Versionner.h"

#define XTFREQ_EIA485		8000000        						//On-board FRC
#define PLLMODE_EIA485		2               					//On-chip PLL setting
#define FCY2_EIA485			XTFREQ_EIA485*PLLMODE_EIA485        //Instruction Cycle Frequency
#define BAUDRATE2_EIA485	1000000
#define BRGVAL_EIA485		((FCY2_EIA485/BAUDRATE2_EIA485)/4)-1

//#define UART2_RTS_DIR	TRISAbits.TRISA2
//#define UART2_RTS		PORTAbits.RA2


#ifdef __PIC24FJ64

#define EI485BUFSIZE	140

#else

//#define RASP_BUFSIZE	256
#define EI485BUFSIZE	128
#endif

//#define LCD_CUE_SIZE 64
//#define PULSE PORTAbits.RA3

#define RX_BUFFER 0


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

extern void Delais(unsigned int tic_ms);
extern unsigned char Ack_flag_response;
extern unsigned char TX485_Heart_Beat_cnt;

extern unsigned char SavedRecalMode;
extern unsigned char Recall_mode;
extern unsigned short Tx485_Data;


void EIA485_Init(void);
void StartEIA485Uart(void);
unsigned char Push_Tx485_Data(unsigned short Data);
unsigned short Pull_Tx485_Data(void);
unsigned short Get_Tx485_Data_Quantity(void);
unsigned char Push_Rx485_Data(unsigned short Data);
unsigned short Get_Rx485_Data_Quantity(void);
unsigned short Pull_Rx485_Data(void);
unsigned char Send_One_485(unsigned int TxData);
unsigned char EIA485MsgParser(void);

unsigned char Get485MsgCnt(void);
