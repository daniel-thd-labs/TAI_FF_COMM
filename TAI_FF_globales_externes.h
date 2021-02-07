#define SRC_RASP 5
#include "TAI_FF_Versionner.h"

extern unsigned short Tx_From_Message_address;
extern unsigned char MsgLenght;
extern unsigned char rxFromSekaSwtData[SYSTEMSIZE];
extern unsigned short rxFromSekaFaderData[SYSTEMSIZE];

extern unsigned short txToSekaFaderData[SYSTEMSIZE];
extern unsigned char txToSekaFaderLedState[SYSTEMSIZE];

extern unsigned short InitalFaderValue[SYSTEMSIZE];
extern unsigned short DAW_IN_BUF[3][4][8];
extern unsigned short SPI_BUF_SENT[3][4][8];

extern unsigned char ledFlashState1[SYSTEMSIZE];
extern unsigned char ledFlashState2[SYSTEMSIZE];
extern unsigned char Tx_data_count;
extern unsigned char TxChan;
extern unsigned char Config_registers[128];
extern unsigned int WAIT_FOR_ACK_FLAG;
extern unsigned char SEKA_ONLINE_FLAG;
extern unsigned char RxDataIndex;
extern unsigned char TxDataIndex;
//extern unsigned char swtCnt[SYSTEMSIZE];
extern unsigned char swtMode[SYSTEMSIZE];

extern unsigned char MCMASwtMode[4];
extern unsigned char MCMALedState[4];

extern unsigned char prevSwtOnMode[SYSTEMSIZE];
extern unsigned char prevSwtOffMode[SYSTEMSIZE];
extern unsigned char prevMCMASwtOffMode[4];
extern unsigned char prevMCMASwtOnMode[4];

extern unsigned char multiSwtCode[SYSTEMSIZE];
extern unsigned char AUTOMODE[SYSTEMSIZE];
extern unsigned char lastAutoModeWasTouchFlag[3][4];

extern unsigned short TempsRxmsg1;
//extern unsigned char ffTouchState[3][4];
extern unsigned char FF_Solo_State[12];

extern unsigned char FF_MuteRec_State[12];
extern unsigned char last_FF_MuteRec_State[12];
extern unsigned char FF_Select_State[12];
extern unsigned char FF_WasTouchedState[3][4];

extern unsigned char LedOverrideState[12];
extern unsigned char automodeRegisters[3][4][3];

extern unsigned char flashByte1;
extern unsigned char flashByte2;
extern unsigned short flashSpeed1;
extern unsigned short flashSpeed2;
extern unsigned short flashCounter1;
extern unsigned short flashCounter2;

extern unsigned char enableReadVolState[3][4];
extern unsigned char enableReadCutState[3][4];
extern unsigned char enableWriteVolState[3][4];
extern unsigned char enableWriteCutState[3][4];

extern unsigned char fromFaderCutState[3][4];
extern unsigned char ActiveCutState[3][4];
extern unsigned char toFaderCutState[3][4];
extern unsigned char fromDawCutState[3][4];
extern unsigned char toDawCutState[3][4];
extern unsigned char taiCutState[3][4];

extern unsigned char fromFaderTouchState[3][4];
extern unsigned char UDPActiveChannelFlag[3][4];

extern unsigned char masterSwtNbr;
extern unsigned char globalSwtNbr;

extern unsigned char masterSwtStates;




//------------------ grouping variables ------------------
extern unsigned char GrpFdrWasTouchedState[3][4];
extern short groupOffset[16];
extern unsigned char creatGroupMode;
extern unsigned char groupMasterExist;
extern unsigned char currentGroupNumber;
extern unsigned char faderGroupNumber[SYSTEMSIZE];//State & membership MSN(0x80 is master) LSN (0x00-0x0F)
extern unsigned char faderGroupMaster[SYSTEMSIZE];//State & membership MSN(0x80 is master) LSN (0x00-0x0F)
extern unsigned char LastFaderAssignedGroup[SYSTEMSIZE];
extern unsigned char LastAssignedGroupMaster[SYSTEMSIZE];
extern unsigned char groupDisplayFlag[16];
extern unsigned char groupMastersList[16];
extern unsigned char engageGroupMotor[16];
extern unsigned char engageFaderMotor[SYSTEMSIZE];

//------------------ linking variables ------------------
extern short linkOffset[SYSTEMSIZE];
extern unsigned char engageLinkMotor[SYSTEMSIZE];
extern unsigned char faderLinkNumber[SYSTEMSIZE];//State & membership MSN(0x80 is master) LSN (0x00-0x0F)
extern unsigned char linkDisplayFlag[SYSTEMSIZE];
extern unsigned char creatLinkMode;
extern unsigned char linkMemberTouched;
extern unsigned char currentLinkNumber;
extern unsigned char lnkMemberCnt[SYSTEMSIZE];
extern unsigned char tempLinkMaster;
extern unsigned char LastFaderAssignedLnk[SYSTEMSIZE];
//------------------ stereo linking variables ------------------
extern unsigned char linkType[SYSTEMSIZE];
extern unsigned char stereoLinkMode;

extern unsigned char MotorDefeatCnt[SYSTEMSIZE];

//------------------ Other variables ------------------
extern unsigned char RaspMsgType;
extern char offset[SYSTEMSIZE][16];
extern unsigned char shiftSwtState;
extern unsigned char MCMASwitch[4];
extern unsigned char newMCALedStat;
extern unsigned char newMGALedStat;

extern unsigned char passThroughMode;
extern unsigned char passThroughFlag;
extern unsigned short EIA485PassThroughTxMsgCnt;
extern unsigned short EIA485PassThroughRxMsgCnt;

extern unsigned char PassThroughRx485Buffer[256];
extern unsigned char passThroughRx485Flag;
extern unsigned char passThroughTx485Index;
extern unsigned char passThroughRx485Index;
extern unsigned char passThroughTx232Index;


