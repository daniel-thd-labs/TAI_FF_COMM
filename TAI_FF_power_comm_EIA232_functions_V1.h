#include "TAI_FF_Versionner.h"
#define XTFREQ          8000000        		//On-board FRC
#define PLLMODE         2               		//On-chip PLL setting
#define FCY2            XTFREQ*PLLMODE        //Instruction Cycle Frequency

#define BAUDRATE_RASP	115200   
#define BAUDRATE1MHZ    1000000
#define BRGVAL_RASP    (FCY2/(16*BAUDRATE_RASP))-1
#define BRGVAL1MHZ     (FCY2/(4*BAUDRATE1MHZ))-1

#define RASP_BUFSIZE 128
#define RX_BUFFER 0
//#define VOLUME_CODE 0
//#define MUTE_CODE 1
//#define AUTOMODE_CODE 2
//#define CONTROL_CODE 3
//#define ACTIVE_SENS 1

//#define TAI_CODE 3
//#define NRPN_MSB 99
//#define NRPN_LSB 98
//#define DATA_ENTRY_MSB 06
//#define DATA_ENTRY_LSB 38
//#define VMIDI_CABLE 4

extern void Delais(unsigned int tic_ms);
extern unsigned char RaspCommInited;

void Init_RASP_Uart1(void);
void Init_RASP_Uart2(void);
void setRaspUartBootloadMode(void);
void setRaspUartRunMode(void);
void Start_Uart1(void);
void Stop_Uart1(void);
void ResetRASPUart(void);
void Stop_RASP_Uart1(void);

unsigned char Push_Rasp_Tx_Data(unsigned char n);
short Pull_Rasp_Tx_Data(void);
unsigned char Push_Rasp_Rx_Data(unsigned char *databuffer);
short Pull_Rasp_Rx_Data(unsigned char *databuffer);
unsigned char Send_Rasp_Data(unsigned char TxData);
unsigned char PushRaspSingleData(unsigned char index,unsigned char data);
unsigned short PullRaspSingleData(void);
char RaspMsgParser(void);

unsigned char getRaspSingleDataBufferQuantity(void);
unsigned char getRaspSingleDataBufferMaxQuantity(void);
unsigned char getRaspTxBufferQuantity(void);
unsigned char getRaspTxBufferMaxQuantity(void);
unsigned char getRaspRxBufferQuantity(void);
unsigned char getRaspRxBufferMaxQuantity(void);

extern unsigned char BootStateFlag;
//extern unsigned char RaspSingleDataBuf_overrun;
extern unsigned short RaspRxBufQnt;



