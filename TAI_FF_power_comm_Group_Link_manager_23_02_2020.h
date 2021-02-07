
char MCA_linkSwtManager(unsigned char switchState);
char MCA_groupSwtManager(unsigned char switchState);

void selectSwtManager(unsigned char chan,unsigned char switchState);
void groupTouchedMng(unsigned char chan);

void setDisplayedGroup(unsigned char dsplGrp,int state);

void manageNewGrp(unsigned char chan,unsigned char switchState);
void manageExistingGrp(unsigned char chan,unsigned char switchState);

void deleteGrpMember(unsigned char chan,unsigned char switchState);
void deleteGrpMaster(unsigned char chan,unsigned char switchState);
void displayGrp(unsigned char chan);
void addAsMemberToOtherGrp(unsigned char chan,unsigned char switchState);
void removeAsMemberToOtherGrp(unsigned char chan,unsigned char switchState);



void flash1GroupChannel(unsigned char chan,int state);
void flash2GroupChannel(unsigned char chan,int state);
void deleteGroupMember(unsigned char groupNbr);

void grpMasterLedMng(unsigned char groupNbr,int state);
void grpMemberLedMng(unsigned char groupNbr,int state);
void clrGrpLeds(void);

void linkSwtManager(unsigned char chan,unsigned char switchState);
void setDisplayedLink(unsigned char dsplLink,int state);
void manageNewLink(unsigned char chan,unsigned char switchState);
void manageExistingLink(unsigned char chan,unsigned char switchState);

void globalLinkLedManager(unsigned char linkNbr,int state);
void flash1LinkChannel(unsigned char chan,int state);
void deleteLinkMember(unsigned char linkNbr);

