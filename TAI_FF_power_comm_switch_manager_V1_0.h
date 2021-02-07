void switchTester(unsigned char Slave_n,unsigned char master);
void MCMASwitchTester(void);
void switchChangeManager(unsigned char Chan,unsigned char switchState,unsigned char index);
void MCMASwitchChangeManager(unsigned char Chan,unsigned char switchState,unsigned char index);

void chnSwtPressedManager(unsigned char chan,unsigned char switchState,unsigned char index);													//Switch OFF processing
void chnSwtReleasedManager(unsigned char chan,unsigned char switchState,unsigned char index);
void chnSwtHoldManager(unsigned char chan,unsigned char switchState,unsigned char index);

void MCA_swtPressedManager(unsigned char switchState,unsigned char index);													//Switch OFF processing
void MCA_swtReleasedManager(unsigned char switchState,unsigned char index);													//Switch OFF processing
void MCA_swtHoldManager(unsigned char switchState,unsigned char index);													//Switch OFF processing

void GCA_swtPressedManager(unsigned char switchState,unsigned char index);													//Switch OFF processing
void GCA_swtReleasedManager(unsigned char switchState,unsigned char index);													//Switch OFF processing
void GCA_swtHoldManager(unsigned char switchState,unsigned char index);													//Switch OFF processing

void faderTouchManager(unsigned char chan,unsigned char switchState);
void muteSwtManager(unsigned char chan,unsigned char switchState);
void soloSwtManager(unsigned char chan,unsigned char switchState);
void muteRecSwtManager(unsigned char chan,unsigned char switchState);
//void selectSwtManager(unsigned char chan,unsigned char switchState);

char automodeSelect(unsigned char chan,unsigned char AutoSwitch);

char ledAutoModeMng(unsigned char chan);
extern unsigned char gotoMixOffFlag;
extern unsigned char gotoMixOnFlag;
extern unsigned char USBStateFlag;
extern unsigned char REAPER_FLAG;
extern unsigned char HUI_FLAG;
extern unsigned char UDP_FLAG;

//void MCA_groupSwtManager(unsigned char switchState);
//void MCA_linkSwtManager(unsigned char switchState);
void MCA_rsiSwtManager(unsigned char switchState);
void MCA_otherSwtManager(unsigned char switchState);
void MCA_plusSwtManager(unsigned char switchState);
void MCA_minusSwtManager(unsigned char switchState);
void MCA_otherSwtManager(unsigned char switchState);
void MCA_shiftSwtManager(unsigned char switchState);
void MCA_runSwtManager(unsigned char switchState);
void updateFFChanLeds(int code,int chan,char state);
void GCA_soloSwtManager(unsigned char switchState);
void GCA_muteRecSwtManager(unsigned char switchState);
void GCA_soloSwtManager(unsigned char switchState);
void GCA_soloSwtManager(unsigned char switchState);
void GCA_soloSwtManager(unsigned char switchState);
void GCA_soloSwtManager(unsigned char switchState);
