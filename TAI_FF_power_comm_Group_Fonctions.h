unsigned short FF_GroupManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan);
unsigned short FF_LinkManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan,unsigned short faderValue);
unsigned short FaderGrpLimitCalc(int finalval);
unsigned short FaderLnkLimitCalc(int finalval);

extern unsigned char FADER_AUTOMODE_STATE[3][4][8];
//extern unsigned short VCA_IN_BUF[3][4][8];
extern unsigned short DAW_IN_BUF[3][4][8];

extern unsigned char ToUF_MotorStatus[3][4];
extern unsigned char GrpMasterNumber;
extern unsigned char GrpSlaveNumber;

