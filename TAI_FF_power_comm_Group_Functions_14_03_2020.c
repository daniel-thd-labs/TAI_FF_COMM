#include "stdlib.h"
#include "TAI_FF_Versionner.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_Group_Fonctions.h"
#include "TAI_FF_power_comm_global_functions.h"
const char GrpBitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
#define OFF 0
#define WRITE 1
#define LATCH 21
#define TOUCH 2
#define READ 3
#define TRIMTOUCH 6
#define TRIMLATCH 6
#define TRIMWRITE 5
#define TRIMREAD 7
#define STOP 0
#define PLAY 2

#define GRPI 0x00
#define GRP0 0x09
unsigned short grpdelta;
//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS:unsigned short FF_GroupManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan)
//Notes:
//code du master
//arguments: slave_n
//valeur retrounées:void
//
//SSL--->REAPER
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
unsigned short FF_GroupManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan)
{
unsigned short groupDelta;
short currentFaderVal;//,newFaderVal;
unsigned char grpMst,grpSlv,grpMstChn;
char GroupChangedFlag,GroupChangedMasterFlag;
char grpMaster;
unsigned char grpchan;
int fader_number;
unsigned char motoDefeatState;
unsigned char localAutoMode=FADER_AUTOMODE_STATE[Master_Number][slave_n][chan];
GroupChangedFlag=0;
GroupChangedMasterFlag=0;
fader_number=(Master_Number*32)+(slave_n*8)+chan;
motoDefeatState=MotorDefeatCnt[fader_number];
currentFaderVal=rxFromSekaFaderData[fader_number]&0x03ff;
grpMaster=faderGroupMaster[fader_number];					//Get group is Master
grpchan=faderGroupNumber[fader_number];						//Get group membership for this channel

if((faderLinkNumber[fader_number])&&(!grpMaster))
	return FaderGrpLimitCalc(currentFaderVal);


if(grpchan)
	{
	grpMst=(groupMastersList[grpchan-1])/32;
	grpSlv=((groupMastersList[grpchan-1])/8)%4;
	grpMstChn=(groupMastersList[grpchan-1])%8;
	}

//----------------------------------- TEST FOR GROUP CHANGE ---------------------------------------
if(LastFaderAssignedGroup[fader_number]!=grpchan)				//group membership as change?one shot
	{
	if(!grpchan)												//Test for channel not assigned to group
		{
		if((LastFaderAssignedGroup[fader_number]&0x80)==0)
			MotorDefeatCnt[fader_number]=250;
		else
			{
			MotorDefeatCnt[fader_number]=0;
			groupOffset[grpchan-1]=0;							//reset GROUP offset	
			}
		}
	if(LastFaderAssignedGroup[fader_number]==0)
		{
		GroupChangedFlag=1;		//From ungrouped
		InitalFaderValue[fader_number]=currentFaderVal;
		}
	else
		GroupChangedFlag=2;		//From other goup
	LastFaderAssignedGroup[fader_number]=grpchan;
	}

if(LastAssignedGroupMaster[fader_number]!=grpMaster)
	{
	if(LastAssignedGroupMaster[fader_number]==0)
		GroupChangedMasterFlag=1;		//From ungrouped
	else
	if(!grpMaster)
		GroupChangedMasterFlag=2;		//To ungrouped
	LastAssignedGroupMaster[fader_number]=grpMaster;
	}


//---------------------------------------TEST IF GRP =OFF, MOTOR =0FF, motoDefeatState=0 --------------
if((!grpchan)&&(!grpMaster)&&(!motoDefeatState))
	{//on s'occupe des CUTS
	if((fromFaderCutState[Master_Number][slave_n]&GrpBitIndex[chan])
		||(fromDawCutState[Master_Number][slave_n]&GrpBitIndex[chan]))
		ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]|GrpBitIndex[chan];	//Update status un-mute transmi
	else
		ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]&~GrpBitIndex[chan];	//Update status un-mute transmi

	if(localAutoMode==OFF)
		{
		setFFMotor(fader_number,0);//
		return FaderGrpLimitCalc(currentFaderVal);
		}
	else
		{
		if((localAutoMode==WRITE)
			||(localAutoMode==TRIMWRITE))
			{
			setFFMotor(fader_number,0);//	
			if(!(enableWriteVolState[Master_Number][slave_n]&GrpBitIndex[chan]))
				return FaderGrpLimitCalc(DAW_IN_BUF[Master_Number][slave_n][chan]&0x03ff);
			return FaderGrpLimitCalc(currentFaderVal);
			}	
		else
			return FaderGrpLimitCalc(DAW_IN_BUF[Master_Number][slave_n][chan]&0x03ff);
		}			
	}	

if(motoDefeatState>1)	//On revient d'un group Membership on doit replace ler fader au besoin
	{
	setFFMotor(fader_number,1);//
	MotorDefeatCnt[fader_number]--;
	//FaderIsGrouped[Master_Number][slave_n]=FaderIsGrouped[Master_Number][slave_n]&~GrpBitIndex[chan];
	return FaderGrpLimitCalc(InitalFaderValue[fader_number]);
	}
else
if(motoDefeatState==1)	//On revient d'un group Membership le fadera a‚t‚ updat‚
	{
	MotorDefeatCnt[fader_number]=0;
	switch(localAutoMode)
		{
		case OFF:setFFMotor(fader_number,0);//
		break;
		case WRITE:
		case TRIMWRITE:
				if(!(enableWriteVolState[Master_Number][slave_n]&GrpBitIndex[chan]))
					setFFMotor(fader_number,1);
				else
					setFFMotor(fader_number,0);
		break;	

		case READ:
		case TRIMREAD:
		case TOUCH:
		case TRIMTOUCH:
			if(enableReadVolState[Master_Number][slave_n]&GrpBitIndex[chan])
				setFFMotor(fader_number,1);
			else
				setFFMotor(fader_number,0);
		break;
		}
	return FaderGrpLimitCalc(InitalFaderValue[fader_number]);
	}

//-----------------------------------------------------------------------------------------------------
//--------------------------------------- MANAGE GROUP MASTER -------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//Group master is touched, this overrides any data (from daw or other groupers) comming in
switch(GroupChangedMasterFlag)
	{
	case 0:
		if(grpMaster)
			{
			if((fromFaderCutState[Master_Number][slave_n]&GrpBitIndex[chan])
				||(fromDawCutState[Master_Number][slave_n]&GrpBitIndex[chan]))
				ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]|GrpBitIndex[chan];	//Update status un-mute transmi
			else
				ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]&~GrpBitIndex[chan];	//Update status un-mute transmi

			GrpFdrWasTouchedState[Master_Number][slave_n]=GrpFdrWasTouchedState[Master_Number][slave_n]&~GrpBitIndex[chan];
			groupDelta=abs(groupOffset[grpMaster-1]-(short)(currentFaderVal-100));	
			if(groupDelta>1)
				engageGroupMotor[grpMaster-1]=100;
			else
			if(engageGroupMotor[grpMaster-1])
				engageGroupMotor[grpMaster-1]--;
			groupOffset[grpMaster-1]=(short)(currentFaderVal-100);	//Recalculate GROUP DELTA from db	
			if(groupOffset[grpMaster-1]>800)
				groupOffset[grpMaster-1]=900;

			switch(localAutoMode)
				{
				case READ:												//Si le group en READ ou TOUCH
				case TRIMREAD:
				case TOUCH:
				case TRIMTOUCH:
					if(enableReadVolState[Master_Number][slave_n]&GrpBitIndex[chan])
						{
						setFFMotor(fader_number,1);//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]|GrpBitIndex[chan];
						groupOffset[grpMaster-1]=(short)(DAW_IN_BUF[Master_Number][slave_n][chan]-100);								//Recalculate GROUP DELTA 
						}
					else
						{
						setFFMotor(fader_number,0);//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]&~GrpBitIndex[chan];
						groupOffset[grpMaster-1]=(short)(currentFaderVal-100);								//Recalculate GROUP DELTA 
						}
				break;
					case OFF:
				case WRITE:
				case TRIMWRITE:
					groupOffset[grpMaster-1]=(short)((currentFaderVal)-100);														//Recalculate GROUP DELTA	
					setFFMotor(fader_number,0);//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]&~GrpBitIndex[chan];
					break;	
				default:
				break;
				}

			if(abs(groupOffset[grpMaster-1]-(short)(currentFaderVal-100))>1)		//Check if goupr DELTA change from 0db	
				groupOffset[grpMaster-1]=(short)(currentFaderVal-100);				//Recalculate GROUP DELTA from db	
			}
	break;
	case 1:
		groupDelta=abs(groupOffset[grpMaster-1]-(short)(currentFaderVal-100));	
		if(groupDelta>1)
			engageGroupMotor[grpMaster-1]=100;
		else
		if(engageGroupMotor[grpMaster-1])
			engageGroupMotor[grpMaster-1]--;
		groupOffset[grpMaster-1]=(short)(currentFaderVal-100);	//Recalculate GROUP DELTA from db	
		if(groupOffset[grpMaster-1]>800)
			groupOffset[grpMaster-1]=900;
	break;
	case 2:
	break;
	}

//-----------------------------------------------------------------------------------------------------
//--------------------------------------  MANAGE GROUPED FADERS  --------------------------------------
//-----------------------------------------------------------------------------------------------------
if(grpchan)//if fader is touched
	{
	//Mute from Groups 
	if((fromFaderCutState[Master_Number][slave_n]&GrpBitIndex[chan])
		||(fromDawCutState[Master_Number][slave_n]&GrpBitIndex[chan]))
		ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]|GrpBitIndex[chan];	//Update status un-mute transmi
	else
		ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]&~GrpBitIndex[chan];	//Update status un-mute transmi

	if(fromFaderTouchState[Master_Number][slave_n]&GrpBitIndex[chan])														//if fader is touched
		{
		//Tant que le fader est touché,
		setFFMotor(fader_number,0);													//Stop le moteur				
		InitalFaderValue[fader_number]=currentFaderVal-groupOffset[grpchan-1];		//calcule une nouvelle valeur initiale
		GrpFdrWasTouchedState[Master_Number][slave_n]=GrpFdrWasTouchedState[Master_Number][slave_n]|GrpBitIndex[chan];				
		}
	else	//if Fader is grouped but NOT touched is it at the correct location? 
		{
		unsigned short faderdelta=abs(InitalFaderValue[fader_number]+groupOffset[grpchan-1]-currentFaderVal);	
		if((faderdelta>10)||engageGroupMotor[grpchan-1])
			{
			engageFaderMotor[fader_number]=100;
			setFFMotor(fader_number,1);					//Remettre le motor a ON
			}

		if(engageFaderMotor[fader_number])
			{
			setFFMotor(fader_number,1);					//Remettre le motor a ON
			engageFaderMotor[fader_number]--;
			}
		else
			setFFMotor(fader_number,0);					//Remettre le motor a ON

		GrpFdrWasTouchedState[Master_Number][slave_n]=GrpFdrWasTouchedState[Master_Number][slave_n]&~GrpBitIndex[chan];	

		switch(localAutoMode)
			{
			case OFF:
				return FaderGrpLimitCalc((short)(InitalFaderValue[fader_number]+groupOffset[grpchan-1]));
			break;

			case READ:	
			case TOUCH:
			case TRIMREAD:
			case TRIMTOUCH:
				if(enableReadVolState[Master_Number][slave_n]&GrpBitIndex[chan])
					return FaderGrpLimitCalc((short)(DAW_IN_BUF[Master_Number][slave_n][chan]+groupOffset[grpchan-1]));
				else
					return FaderGrpLimitCalc((short)(InitalFaderValue[fader_number]+groupOffset[grpchan-1]));
			break;

			case WRITE:
			case TRIMWRITE:
				if(!(enableWriteVolState[Master_Number][slave_n]&GrpBitIndex[chan]))
					return FaderGrpLimitCalc((short)(DAW_IN_BUF[Master_Number][slave_n][chan]+groupOffset[grpchan-1]));
				else
					return FaderGrpLimitCalc((short)(InitalFaderValue[fader_number]+groupOffset[grpchan-1]));
			break;
			}		
		}
	}
GroupChangedMasterFlag=0;		//From ungrouped
GroupChangedFlag=0;
return FaderGrpLimitCalc(currentFaderVal);
}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS:unsigned short FaderGrpLimitCalc(int faderRelval)
//Verifie MIN/MAX Overflow value for FADER position VS GROUP
//
//
//
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
unsigned short FaderGrpLimitCalc(int faderRelval)
{
if(faderRelval<=0)
	return 0;
else
if(faderRelval>=1000)
	return 999;
return faderRelval;
}
