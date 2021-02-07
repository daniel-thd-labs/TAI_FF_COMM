unsigned short FF_GroupManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan);
unsigned short FF_LinkManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan);
unsigned short FaderLimitCalc(int finalval);

extern unsigned char MF_WasTouchedState[3][4];
extern unsigned char FromUF_TouchState[3][4];
//extern unsigned char FromUF_FaderCutSwitch[3][4];
extern unsigned char FromMF_SoloCutState[3][4];

extern unsigned char GrpAssign[3][4][8];
extern unsigned char GrpFdrWasTouchedState[3][4];
extern unsigned char FromMF_MotorStatus[3][4];
extern short InitalFaderValue[3][4][8];



extern unsigned char MotorDefeatCnt[3][4][8];
//extern unsigned char DAW_mute_state[3][4];
//extern unsigned char FaderCutSwitch[3][4];
//extern unsigned char SSLMuteState[3][4];
//extern unsigned char FaderIsGrouped[3][4];
//extern unsigned char FaderIsLinked[3][4];

extern unsigned char FADER_AUTOMODE_STATE[3][4][8];
extern unsigned short VCA_IN_BUF[3][4][8];
extern unsigned short DAW_IN_BUF[3][4][8];
//extern unsigned char ActiveCutState[3][4]; 
//extern unsigned short SSL_FaderBuffer[3][4][8];

extern unsigned char ToUF_MotorStatus[3][4];
extern unsigned char GrpMasterNumber;
extern unsigned char GrpSlaveNumber;
extern unsigned char enableReadVolState[3][4];
extern unsigned char enableReadCutState[3][4];
extern unsigned char enableWriteVolState[3][4];
extern unsigned char enableWriteCutState[3][4];
