extern unsigned char debugModeFlag;
extern unsigned char SetAllWaitFlag;
extern unsigned char Set_All_Auto_Flag;
//extern unsigned char spi_automode_buffer[3][4][3];

extern unsigned char BankAutomode[12];
extern unsigned char GlobalAutomodeCnt;
extern unsigned char GlobalAutomodeFlag;
extern unsigned char MAX_SLAVE;
extern unsigned char GotoOnStopFlag;
extern unsigned char LastMode;
extern unsigned char NewMode;

extern unsigned char FADER_AUTOMODE_STATE[3][4][8];
extern unsigned char TRIM_AUTOMODE_STATE[3][4];

extern unsigned char Config_registers[128];

extern unsigned char SlaveMatrix[3][4];
extern unsigned char CoreMatrix[3][4];
extern unsigned char DecodedSlaveMatrix[3][4];
extern unsigned char DecodedCoreMatrix[3][4];
//extern unsigned short VCA_IN_BUF[3][4][8];
extern unsigned char FromMF_GrpAssign[3][4][8];
extern unsigned char ToMF_MotorStatus[3][4];
extern unsigned char ToMF_VCAStatus[3][4];
extern unsigned char ToMF_AltStatus[3][4];
extern unsigned char ToMF_GrpIsoStatus[3][4];
extern unsigned char SavedMotorStatus[3][4];
extern unsigned char SavedVCAStatus[3][4];
extern unsigned char SavedAltStatus[3][4];
extern unsigned char SavedGrpIsoStatus[3][4];
extern unsigned char gotoMixOffFlag;
extern unsigned char gotoMixOnFlag;

/****************************************************************
*****************************************************************
|FONCTIONS:void killAllInt(void)
| Force l'arret des int/rruption
|		
| Variable de sortie:void
*****************************************************************
****************************************************************/
void killAllInt(void);

/****************************************************************
*****************************************************************
|FONCTIONS:	void TestGlobalAutomode(unsigned char slave_n, unsigned char Master_Number)
| Force le mode d'automation a celui selectionné par la SSL
|		
| Variable de sortie:void
*****************************************************************
****************************************************************/
void TestGlobalAutomode(unsigned char slave_n, unsigned char Master_Number);

/****************************************************************
*****************************************************************
|FONCTIONS:	unsigned char FindBankAutomode(void)
| Force le mode d'automation a celui selectionné par la SSL
|		
| Variable de sortie:void
*****************************************************************
****************************************************************/
unsigned char FindBankAutomode(unsigned char slave_n, unsigned char Master_Number);

/****************************************************************************************
*****************************************************************************************
|FONCTIONS:void TransportModeChangeManager(unsigned char midi message)
|Notes:	retir une donnée dans un buffer circulaire
|		sans test d'overflow	
|arguments 	:pointeur vers structure de buffer
|			:pointeur de donnée a retourner
|
|
*****************************************************************************************
****************************************************************************************/
void TransportModeChangeManager(unsigned char midi_message);

/****************************************************************
*****************************************************************
FONCTIONS:void AutoGotoOnStop(unsigned char slave_number,unsigned char Core_Number)

Notes:	Établie les différents mode d'automation
		0:off		hui:0x41	Mackie_control:	0x4c
		1:Write			0x42					0x4b
		2:Latch			0x43					0x4e
		3:read			0x44					0x4a
	
Codes de sortie
	1 OK
	-1	timeout
	-2 buffer avec plus de 1
*****************************************************************
****************************************************************/
void AutoGotoOnStop(unsigned char slave_number,unsigned char Core_Number);
extern void Delais(unsigned int tic_ms);

void SaveSysState(void);
void RestoreSysState(void);
void ResetSysState(void);
void SetSysState(unsigned char state);
void SessionChangeManager(void);
void MixOffManager(void);
void MixOnManager(void);
void refresh(void);
void setFFMotor(unsigned char fader_number,unsigned char motorState);
void findServoOffset(int channel);
void ExtAutoModeManager(unsigned char type,unsigned char autoMode,unsigned char idx);
