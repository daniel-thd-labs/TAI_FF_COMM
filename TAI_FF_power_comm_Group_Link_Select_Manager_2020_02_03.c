#include "TAI_FF_Versionner.h"
#include "TAI_FF_power_comm_Group_Link_manager_23_02_2020.h"
#include "TAI_FF_globales_externes.h"
//#include "TAI_FF_power_comm_EIA485_functions_V1.h"
#include "TAI_FF_power_comm_EIA485_functions_V1.h"
#include "TAI_FF_power_comm_EIA232_functions_V1.h"
#include "TAI_FF_power_comm_Message_central_manager_V1.h"
#include "TAI_FF_power_comm_buffers.h"
#define ON 1
#define OFF 0

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FUNCTIONS:	char MCA_groupSwtManager(unsigned char switchState)
//Notes:	Send					
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
// 0x80=group
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
char MCA_groupSwtManager(unsigned char switchState)
{
if(!creatLinkMode)
	{
	if(switchState) //1 means toggle
		masterSwtStates^=0x80;
	else
		masterSwtStates=masterSwtStates&0x7f;
	if(masterSwtStates&0x80)
		{
		MCMALedState[1]=MCMALedState[1]|0x01;
		creatGroupMode=1;
		}
	else
		{
		MCMALedState[1]=MCMALedState[1]&0xfe;
		creatGroupMode=0;
		if(!groupMasterExist)			//Delete group member
			{
			if(currentGroupNumber)
				groupMastersList[currentGroupNumber-1]=0;
			clrGrpLeds();
			deleteGroupMember(currentGroupNumber);
			}
		else
			{
			setDisplayedGroup(currentGroupNumber-1,OFF);
			clrGrpLeds();
			groupMasterExist=0;
			//if(currentGroupNumber)
			//	groupMastersList[currentGroupNumber-1]=0;
			}
		currentGroupNumber=0;
		}
	if((Config_registers[11]&0x03)==3)
		midiSendSingleConfig(26,creatGroupMode);
	Push_Config_data_In_Queue(0x26,creatGroupMode,0);	
	PushRaspSingleData(26,creatGroupMode);
	RaspMsgType=1;
	}
return creatGroupMode;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FUNCTIONS:
//char MCA_linkSwtManager(unsigned char chan, unsigned char switchState)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
// 0x01=link
// 0x08=select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
char MCA_linkSwtManager(unsigned char switchState)
{
if(!creatGroupMode)
	{
	if(switchState)					
		masterSwtStates^=0x01;
	else
		masterSwtStates=masterSwtStates&0xfe;
	if(masterSwtStates&0x01)
		{
		MCMALedState[1]=MCMALedState[1]|0x02;
		creatLinkMode=1;
		}
	else		//LINK function is off
		{
		MCMALedState[1]=MCMALedState[1]&0xfd;
		if(!currentLinkNumber)						//Delete group member
			{
			globalLinkLedManager(currentLinkNumber,OFF);
			}
		else
			{
			setDisplayedLink(currentLinkNumber-1,OFF);
			if(lnkMemberCnt[currentLinkNumber-1]==1)
				{
				lnkMemberCnt[currentLinkNumber-1]=0;
				deleteLinkMember(currentLinkNumber);
				}
			globalLinkLedManager(currentLinkNumber,OFF);
			}
		creatLinkMode=0;
		currentLinkNumber=0;
		}
	if((Config_registers[11]&0x03)==3)
		midiSendSingleConfig(26,creatLinkMode*2);
	Push_Config_data_In_Queue(0x26,creatLinkMode*2,0);	
	PushRaspSingleData(26,creatLinkMode*2);
	RaspMsgType=1;
	}
return creatLinkMode;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	selectSwtManager(unsigned char chan ,unsigned char switchState)
//Notes:						
//Arguments:
//Variables locals:
//Variables globals:
//Overview:        	0x01=Mute/MuteLed
//					0x02=Mute Record LED
//					0x04=Solo LED
//					0x08=Select LED
//					0x10=Record LED
//					0x20=automatch LED
//					0x40=UP LED
//					0x80=Down LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void selectSwtManager(unsigned char chan,unsigned char switchState)
{
if(!switchState)
	{
	if(creatGroupMode)								//Currently in create group mode
		{
		if(!faderLinkNumber[chan])					//test if this channel is a link member
			{
			int chanGrpStatus=0;

			if(faderGroupNumber[chan])
				chanGrpStatus=1;
			if(faderGroupMaster[chan])
				chanGrpStatus=chanGrpStatus|2;

			switch(chanGrpStatus)
				{
				case 0:								//is not a part of a group;
					manageNewGrp(chan,switchState);
				break;
				case 1:								//is a group member
					if(currentGroupNumber==faderGroupNumber[chan])
						deleteGrpMember(chan,switchState);
					else
						{
						currentGroupNumber=faderGroupNumber[chan];
						displayGrp(chan);
						}
				break;

				case 2:			//is a group master
					if(currentGroupNumber==faderGroupMaster[chan])
						deleteGrpMaster(chan,switchState);
					else
					if(currentGroupNumber)
						addAsMemberToOtherGrp(chan,switchState);
					else
					if(!currentGroupNumber)
						{
						currentGroupNumber=faderGroupMaster[chan];
						displayGrp(chan);				
						}
				break;

				case 3:			//Is a group master and member
					if(currentGroupNumber==faderGroupMaster[chan])
						deleteGrpMaster(chan,switchState);
					else
					if(currentGroupNumber==faderGroupNumber[chan])
						deleteGrpMember(chan,switchState);
				break;
				}
			}
		}
	else
	if(creatLinkMode)														//Currently in create link mode
		{
		if(!faderGroupNumber[chan])											//test if this channel is link member
			{
			if(!faderLinkNumber[chan])										//test if this channel is link member
				manageNewLink(chan,switchState);							//Start new link group
			else
				{
				if(!(linkDisplayFlag[faderLinkNumber[chan]-1]))
					{
					if(currentLinkNumber)
						{
						if(lnkMemberCnt[currentLinkNumber-1]==1)
							{
							lnkMemberCnt[currentLinkNumber-1]=0;
							deleteLinkMember(currentLinkNumber);
							}
						}
					clrGrpLeds();;
					setDisplayedLink(faderLinkNumber[chan]-1,ON);			//display link members if any member is selected
					globalLinkLedManager(faderLinkNumber[chan],ON);
					currentLinkNumber=faderLinkNumber[chan];
					}
				else
					manageExistingLink(chan,switchState);					//manager active member of the same link
				}		
			}
		}
	}
}	

//-----------------------------------------------------------------------
//Function:
//void groupTouchedMng(unsigned char chan)
//-----------------------------------------------------------------------
void groupTouchedMng(unsigned char chan)
{
if(faderGroupMaster[chan]
	||faderGroupNumber[chan])
	{
	if(faderGroupMaster[chan])
		currentGroupNumber=faderGroupMaster[chan];						//get group number membership
	else
		currentGroupNumber=faderGroupNumber[chan];						//get group number membership
	if(currentGroupNumber)
		{
		if(!groupDisplayFlag[currentGroupNumber-1])
			{
			setDisplayedGroup(currentGroupNumber-1,ON);						//display group members if any member is selected
			clrGrpLeds();
			grpMasterLedMng(currentGroupNumber,ON);
			grpMemberLedMng(currentGroupNumber,ON);
			groupMasterExist=1;
			}
		}
	}
else
	{
	if(currentGroupNumber)
		setDisplayedGroup(currentGroupNumber-1,OFF);						//display group members if any member is selected
	currentGroupNumber=0x00;
	clrGrpLeds();
	groupMasterExist=0x00;
	}
}
//-----------------------------------------------------------------------
//Function:
//void setDisplayedGroup(unsigned char dsplGrp,int state)
//-----------------------------------------------------------------------
void setDisplayedGroup(unsigned char dsplGrp,int state)
{
int n;
for(n=0;n<16;n++)
	groupDisplayFlag[n]=0;
if(state)
	groupDisplayFlag[dsplGrp]=ON;
}
			
//-----------------------------------------------------------------------
//select switch for group select
//					0x08=Select LED
//-----------------------------------------------------------------------
void manageNewGrp(unsigned char chan,unsigned char switchState)
{
int n=0;
if(!switchState)
	{
	if(!groupMasterExist)									//if first member of this new group, select as master
		{
		if(!currentGroupNumber)
			{
			while(groupMastersList[n]&&(n<16))
				n++;
			groupMasterExist=1;									//if first member of this new group, select as master
			groupMastersList[n]=chan+1;					
			currentGroupNumber=n+1;
			}
		else
			{
			groupMasterExist=1;									//if first member of this new group, select as master
			groupMastersList[currentGroupNumber-1]=chan+1;					
			}
		faderGroupMaster[chan]=currentGroupNumber;
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
		ledFlashState1[chan]=ledFlashState1[chan]|0x08;
		ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
		}
	else											//if not first member of this new group, select as member
		{
		faderGroupNumber[chan]=currentGroupNumber;
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
		ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
		ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
		}
	setDisplayedGroup(currentGroupNumber-1,ON);
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void deleteGrpMember(unsigned char chan,unsigned char switchState)
{
faderGroupNumber[chan]=0x00;
ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;//Select led = off
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void deleteGrpMaster(unsigned char chan,unsigned char switchState)
{
faderGroupMaster[chan]=0x00;							//clear group master
groupMasterExist=0;
ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;//Select led = off
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void displayGrp(unsigned char chan)
{
setDisplayedGroup(currentGroupNumber-1,ON);						//display group members if any member is selected
clrGrpLeds();
grpMasterLedMng(currentGroupNumber,ON);
grpMemberLedMng(currentGroupNumber,ON);
groupMasterExist=1;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void addAsMemberToOtherGrp(unsigned char chan,unsigned char switchState)
{
faderGroupNumber[chan]=currentGroupNumber;
txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
ledFlashState1[chan]=ledFlashState1[chan]|0x08;
ledFlashState2[chan]=ledFlashState2[chan]|0x08;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void removeAsMemberToOtherGrp(unsigned char chan,unsigned char switchState)
{
chan++;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void manageExistingGrp(unsigned char chan,unsigned char switchState)
{
if(!switchState)
	{
	if(faderGroupMaster[chan])									//test if group master
	//manage clearing the group Master
		{
		faderGroupMaster[chan]=0x00;							//clear group master
		groupMasterExist=0;
		ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
		ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
		}

	if(faderGroupNumber[chan])									//test if group member, Z
	//manage clearing a group member
		{
		faderGroupNumber[chan]=0x00;
		ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
		ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
		}
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	
//void grpMemberLedMng(unsigned char groupNbr,int state)
//Notes:
//
//Argument(s):groupNbr, state
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderGroupNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void grpMemberLedMng(unsigned char groupNbr,int state)
{
int chan;
for (chan=0;chan<SYSTEMSIZE;chan++)
	{
	if(faderGroupNumber[chan]==groupNbr)
		{
		if(state)
			{
			txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
			if(faderGroupMaster[chan])
				ledFlashState2[chan]=ledFlashState2[chan]|0x08;
			else
				ledFlashState2[chan]=ledFlashState2[chan]&~0x08;

			}
		else
			{
			txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
			ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
			}
		}
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	
//void grpMasterLedMng(unsigned char groupNbr,int state)
//Notes:
//
//Argument(s):groupNbr, state
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderGroupNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void grpMasterLedMng(unsigned char groupNbr,int state)
{
int chan;
for (chan=0;chan<SYSTEMSIZE;chan++)
	{
	if(faderGroupMaster[chan]==groupNbr)
		{
		if(state)
			{
			txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
			ledFlashState1[chan]=ledFlashState1[chan]|0x08;
			ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
			}
		else
			{
			txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
			ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
			ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
			}
		}
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	
//void clrGrpLeds(void)
//Notes:
//
//Argument(s):groupNbr, state
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderGroupNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void clrGrpLeds(void)
{
int chan;
for (chan=0;chan<SYSTEMSIZE;chan++)
	{
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
	ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
	ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	void flash1GroupChannel(unsigned char chan,int state)
//Notes:						
//
//Argument(s):chan, state
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderGroupNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void flash1GroupChannel(unsigned char chan,int state)
{
if(state)
	ledFlashState1[chan]=ledFlashState1[chan]|0x08;
else
	ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	void flash2GroupChannel(unsigned char chan,int state)
//Notes:						
//
//Argument(s):chan, state
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderGroupNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void flash2GroupChannel(unsigned char chan,int state)
{
if(state)
	ledFlashState2[chan]=ledFlashState2[chan]|0x08;
else
	ledFlashState2[chan]=ledFlashState2[chan]&~0x08;
txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	void deleteGroupMember(unsigned char groupNbr)
//Notes:						
//
//Argument(s):groupNbr
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderGroupNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void deleteGroupMember(unsigned char groupNbr)
{
int chan;
for (chan=0;chan<SYSTEMSIZE;chan++)
	{
	if(faderGroupNumber[chan]==groupNbr)
		faderGroupNumber[chan]=0x00;
	}

}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	
//void globalLinkLedManager(unsigned char linkNbr,int state)
//Notes:
//
//Argument(s):linkNbr, state
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderLinkNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void globalLinkLedManager(unsigned char linkNbr,int state)
{
int chan;
for (chan=0;chan<SYSTEMSIZE;chan++)
	{
	if(!state)
		{
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
		ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
		}
	else
	if(faderLinkNumber[chan]==linkNbr)
		{
		if(state)
			{
			txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
			if(faderLinkNumber[chan])
				ledFlashState1[chan]=ledFlashState1[chan]|0x08;
			else
				ledFlashState1[chan]=ledFlashState1[chan]&~0x08;;
			}
		else
			{
			txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
			ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
			}
		}
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	void deleteLinkMember(unsigned char linkNbr)
//Notes:						
//
//Argument(s):groupNbr
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderLinkNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void deleteLinkMember(unsigned char linkNbr)
{
int chan;
for (chan=0;chan<SYSTEMSIZE;chan++)
	{
	if((faderLinkNumber[chan])==linkNbr)
		faderLinkNumber[chan]=0x00;
	}

}

//-----------------------------------------------------------------------
//Function:
//void setDisplayedLink(unsigned char dsplLink,int state)
//-----------------------------------------------------------------------
void setDisplayedLink(unsigned char dsplLink,int state)
{
int n;
for(n=0;n<SYSTEMSIZE;n++)
	linkDisplayFlag[n]=0;
if(state)
	linkDisplayFlag[dsplLink]=ON;
}

//-----------------------------------------------------------------------
//select switch for group select
//					0x08=Select LED
//-----------------------------------------------------------------------
void manageNewLink(unsigned char chan,unsigned char switchState)
{
int n=0;
if(!switchState)
	{
	if(!currentLinkNumber)									//No link created yetif first link
		{
		for(n=0;n<SYSTEMSIZE;n++)
			{
			if(currentLinkNumber<faderLinkNumber[n])
				currentLinkNumber=faderLinkNumber[n];
			}
		currentLinkNumber=currentLinkNumber+1;
		faderLinkNumber[chan]=currentLinkNumber;
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
		ledFlashState1[chan]=ledFlashState1[chan]|0x08;
		lnkMemberCnt[currentLinkNumber-1]=1;
		}
	else											//if not first member of this new group, select as member
		{
		faderLinkNumber[chan]=currentLinkNumber;
		txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
		ledFlashState1[chan]=ledFlashState1[chan]|0x08;
		lnkMemberCnt[currentLinkNumber-1]++;
		}
	setDisplayedLink(currentLinkNumber-1,ON);
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void manageExistingLink(unsigned char chan,unsigned char switchState)
{
if(!switchState)
	{
	if(lnkMemberCnt[faderLinkNumber[chan]-1])
		lnkMemberCnt[faderLinkNumber[chan]-1]--;	
	faderLinkNumber[chan]=0x00;
	ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&~0x08;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Function:	void flash1LinkChannel(unsigned char chan,int state)
//Notes:						
//
//Argument(s):chan, state
//Variables locals: n
//Variables globals: ledFlashState1,txToSekaFaderLedState,faderGroupNumber
//Overview:	0x08=Select LED
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void flash1LinkChannel(unsigned char chan,int state)
{
if(state)
	ledFlashState1[chan]=ledFlashState1[chan]|0x08;
else
	ledFlashState1[chan]=ledFlashState1[chan]&~0x08;
txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;
}
