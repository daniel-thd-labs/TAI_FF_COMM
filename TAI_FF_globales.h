#include "TAI_FF_Versionner.h"
unsigned short Tx_From_Message_address;
unsigned char MsgLenght;
unsigned char Tx_data_count;
unsigned char TxChan;
unsigned int WAIT_FOR_ACK_FLAG;
unsigned char SEKA_ONLINE_FLAG;
unsigned char RxDataIndex;
unsigned char TxDataIndex;
unsigned char Config_registers[128];

unsigned short DAW_IN_BUF[3][4][8];
unsigned short SPI_BUF_SENT[3][4][8];

unsigned char rxFromSekaSwtData[SYSTEMSIZE];
unsigned char txToSekaFaderLedState[SYSTEMSIZE];
unsigned short rxFromSekaFaderData[SYSTEMSIZE];
unsigned short txToSekaFaderData[SYSTEMSIZE];
unsigned short InitalFaderValue[SYSTEMSIZE];
short linkOffset[SYSTEMSIZE];
unsigned char ledFlashState1[SYSTEMSIZE];
unsigned char ledFlashState2[SYSTEMSIZE];
unsigned char swtMode[SYSTEMSIZE];
unsigned char prevSwtOffMode[SYSTEMSIZE];
unsigned char prevSwtOnMode[SYSTEMSIZE];

unsigned char MCMASwtMode[4];
unsigned char MCMALedState[4];


unsigned char prevMCMASwtOffMode[4];
unsigned char prevMCMASwtOnMode[4];
unsigned char multiSwtCode[SYSTEMSIZE];
unsigned char AUTOMODE[SYSTEMSIZE];
unsigned char lastAutoModeWasTouchFlag[3][4];

unsigned short TempsRxmsg1;
unsigned char FF_Solo_State[12];
unsigned char FF_Select_State[12];
unsigned char FF_MuteRec_State[12];
unsigned char last_FF_MuteRec_State[12];

unsigned char FF_WasTouchedState[3][4];
unsigned char LedOverrideState[12];

unsigned char automodeRegisters[3][4][3];
unsigned char enableReadVolState[3][4];
unsigned char enableReadCutState[3][4];
unsigned char enableWriteVolState[3][4];
unsigned char enableWriteCutState[3][4];

unsigned char flashByte1;
unsigned char flashByte2;
unsigned short flashSpeed1;
unsigned short flashSpeed2;
unsigned short flashCounter1;
unsigned short flashCounter2;

unsigned char fromFaderCutState[3][4];
unsigned char ActiveCutState[3][4];

unsigned char toFaderCutState[3][4];
unsigned char fromDawCutState[3][4];
unsigned char toDawCutState[3][4];
unsigned char taiCutState[3][4];

unsigned char masterSwtNbr;
unsigned char globalSwtNbr;

unsigned char masterSwtStates;

//------------------ grouping variables ------------------
unsigned char GrpFdrWasTouchedState[3][4];
short groupOffset[16];
unsigned char creatGroupMode;
unsigned char groupMasterExist;
unsigned char currentGroupNumber;
unsigned char faderGroupNumber[SYSTEMSIZE];//State & membership MSN(0x80 is master) LSN (0x00-0x0F)
unsigned char faderGroupMaster[SYSTEMSIZE];//State & membership MSN(0x80 is master) LSN (0x00-0x0F)
unsigned char LastFaderAssignedGroup[SYSTEMSIZE];
unsigned char LastAssignedGroupMaster[SYSTEMSIZE];
unsigned char groupDisplayFlag[16];
unsigned char groupMastersList[16];
unsigned char engageGroupMotor[16];
unsigned char engageFaderMotor[SYSTEMSIZE];

//------------------ linking variables ------------------
unsigned char engageLinkMotor[SYSTEMSIZE];
unsigned char faderLinkNumber[SYSTEMSIZE];//State & membership MSN(0x80 is master) LSN (0x00-0x0F)
unsigned char linkDisplayFlag[SYSTEMSIZE];
unsigned char creatLinkMode;
unsigned char linkMemberTouched;
unsigned char currentLinkNumber;
unsigned char lnkMemberCnt[SYSTEMSIZE];
unsigned char tempLinkMaster;
unsigned char LastFaderAssignedLnk[SYSTEMSIZE];
//------------------ stereo linking variables ------------------
unsigned char linkType[SYSTEMSIZE];
unsigned char stereoLinkMode;


unsigned char MotorDefeatCnt[SYSTEMSIZE];



//------------------ Other variables ------------------
unsigned char RaspMsgType;
char offset[SYSTEMSIZE][16];
unsigned char shiftSwtState;
unsigned char MCMASwitch[4];
unsigned char newMCALedStat;
unsigned char newMGALedStat;

unsigned char passThroughFlag;
unsigned char passThroughMode;
unsigned short EIA485PassThroughTxMsgCnt;
unsigned short EIA485PassThroughRxMsgCnt;


unsigned char PassThroughRx485Buffer[256];
unsigned char passThroughRx485Flag;
unsigned char passThroughTx485Index;
unsigned char passThroughRx485Index;
unsigned char passThroughTx232Index;
