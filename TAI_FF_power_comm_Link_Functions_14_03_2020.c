#include "stdlib.h"
#include "TAI_FF_Versionner.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_Group_Fonctions.h"
#include "TAI_FF_power_comm_global_functions.h"
const char lnkBitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
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

unsigned short lnkdelta;
//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//unsigned short FF_LinkManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan,unsigned short faderValue)

//Notes:
//code du master
//arguments: slave_n
//valeur retrounées:void
//
//SSL--->REAPER
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
unsigned short FF_LinkManager(unsigned char Master_Number,unsigned char slave_n,unsigned char chan,unsigned short faderValue)
{
short rxSekaFaderValue;
short txfaderValue;
char linkChangedFlag;
//short localLinkOffset;
unsigned char lnkchan;
int fader_number;
unsigned char localAutoMode=FADER_AUTOMODE_STATE[Master_Number][slave_n][chan];

unsigned char motoDefeatState;

fader_number=(Master_Number*32)+(slave_n*8)+chan;
motoDefeatState=MotorDefeatCnt[fader_number];

if(faderGroupNumber[fader_number])
	return faderValue;

rxSekaFaderValue=rxFromSekaFaderData[fader_number]&0x03ff;
txfaderValue=txToSekaFaderData[fader_number]&0x03ff;

linkChangedFlag=0;
lnkchan=faderLinkNumber[fader_number];						//Get group membership for this channel

//---------------------------------------TEST LINK CHANGE---------------------------------------
if(LastFaderAssignedLnk[fader_number]!=lnkchan)				//link membership as change?
	{
	if(!lnkchan)											//Test for channel not assigned to group
		{
		if(LastFaderAssignedLnk[fader_number]==0)
			MotorDefeatCnt[fader_number]=250;
		else
			{
			MotorDefeatCnt[fader_number]=0;
			//linkOffset[lnkchan-1]=0;						//reset Lnik offset	
			}
		}
	else
	if(LastFaderAssignedLnk[fader_number]==0)
		{
		linkChangedFlag=1;		//From unlinked
		InitalFaderValue[fader_number]=rxSekaFaderValue;
		if(lnkchan)
			{
			if(lnkMemberCnt[lnkchan-1]==1)
				linkOffset[lnkchan-1]=0;					//reset Link offset
			}
		}
	else
		linkChangedFlag=2;		//From other lnk
	LastFaderAssignedLnk[fader_number]=lnkchan;			
	}

//----------------- TEST IF link=0FF and motor defeat off -------------------------
if((!lnkchan)&&(!motoDefeatState))
	{//on s'occupe des CUTS
	if((fromFaderCutState[Master_Number][slave_n]&lnkBitIndex[chan])
		||(fromDawCutState[Master_Number][slave_n]&lnkBitIndex[chan]))
			ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]|lnkBitIndex[chan];	//Update status mute transmit
		else
			ActiveCutState[Master_Number][slave_n]=ActiveCutState[Master_Number][slave_n]&~lnkBitIndex[chan];	//Update status un-mute transmit
		
	if(localAutoMode==OFF)
		{
		setFFMotor(fader_number,0);//
		return FaderLnkLimitCalc(rxSekaFaderValue);
		}
	else
		{
		if((localAutoMode==WRITE)
			||(localAutoMode==TRIMWRITE))
			{
			setFFMotor(fader_number,0);//	
			if(!(enableWriteVolState[Master_Number][slave_n]&lnkBitIndex[chan]))
				return FaderLnkLimitCalc(DAW_IN_BUF[Master_Number][slave_n][chan]&0x03ff);
			else
				return FaderLnkLimitCalc(rxSekaFaderValue);
			}	
		else
			return FaderLnkLimitCalc(DAW_IN_BUF[Master_Number][slave_n][chan]&0x03ff);
		}			
	}

//----------------- TEST IF link=0FF and motor defeat cont ON -------------------------
if(motoDefeatState>1)	//On revient d'un group Membership on doit replace ler fader au besoin
	{
	MotorDefeatCnt[fader_number]--;
	//FaderIsGrouped[Master_Number][slave_n]=FaderIsGrouped[Master_Number][slave_n]&~lnkBitIndex[chan];
	return FaderLnkLimitCalc(InitalFaderValue[fader_number]);
	}
else
if(motoDefeatState==1)	//On revient d'un group Membership on doit replace ler fader au besoin
	{
	MotorDefeatCnt[fader_number]=0;
	if(localAutoMode==OFF)
		{
		setFFMotor(fader_number,0);//
		}//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]&~lnkBitIndex[chan];
	else
		{
		if((localAutoMode!=WRITE)
			&&(localAutoMode!=TRIMWRITE)
			&&(enableReadVolState[Master_Number][slave_n]&lnkBitIndex[chan]))
			{
			setFFMotor(fader_number,1);//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]|lnkBitIndex[chan];
			}
		else
			{
			if((localAutoMode==WRITE)
				&&(localAutoMode==TRIMWRITE))
				{
				if(!(enableWriteVolState[Master_Number][slave_n]&lnkBitIndex[chan]))
					setFFMotor(fader_number,1);//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]|lnkBitIndex[chan];
				else
					setFFMotor(fader_number,0);//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]&~lnkBitIndex[chan];
				}
			}
		}
	return FaderLnkLimitCalc(InitalFaderValue[fader_number]);
	}

//--------------------- LINK FADER MANAGER ---------------------
if(lnkchan)
	{	
	//--------------------- TEST FOR TOUCHED FADER ---------------------
	if(fromFaderTouchState[Master_Number][slave_n]&lnkBitIndex[chan])	//one fader is touched,
		{
		if(!engageLinkMotor[lnkchan-1])
			{
			if(!(GrpFdrWasTouchedState[Master_Number][slave_n]&lnkBitIndex[chan]))
				{
				GrpFdrWasTouchedState[Master_Number][slave_n]=GrpFdrWasTouchedState[Master_Number][slave_n]|lnkBitIndex[chan];
				tempLinkMaster=fader_number+1;
				}
			}
		}
	else
	if(GrpFdrWasTouchedState[Master_Number][slave_n]&lnkBitIndex[chan])
		GrpFdrWasTouchedState[Master_Number][slave_n]=GrpFdrWasTouchedState[Master_Number][slave_n]&~lnkBitIndex[chan];

	//--------------------- UN-TOUCHED FADER ---------------------
	if(tempLinkMaster==(fader_number+1))
		{
		int newOffset;
		if(linkType[fader_number])
			{
			newOffset =rxSekaFaderValue-InitalFaderValue[fader_number];
			lnkdelta = abs(linkOffset[lnkchan-1]-newOffset);
			}
		else
			{
			lnkdelta=abs(rxSekaFaderValue-linkOffset[lnkchan-1]);
			newOffset = rxSekaFaderValue;
			}

		if(lnkdelta>1)	//Recalculate GROUP DELTA from db	
			{
			engageLinkMotor[lnkchan-1]=100;
			setFFMotor(fader_number,0);//ToUF_MotorStatus[Master_Number][slave_n]=ToUF_MotorStatus[Master_Number][slave_n]&GrpBitIndex[chan];
			linkOffset[lnkchan-1]=newOffset;	//Recalculate GROUP DELTA from db	
			if(linkOffset[lnkchan-1]>800)
				linkOffset[lnkchan-1]=900;
			if(linkType[fader_number])
				return FaderGrpLimitCalc(rxSekaFaderValue+linkOffset[lnkchan-1]);
			return FaderGrpLimitCalc(linkOffset[lnkchan-1]);
			}
		else
			{
			if(engageLinkMotor[lnkchan-1])
				engageLinkMotor[lnkchan-1]--;
			}
		return FaderGrpLimitCalc(rxSekaFaderValue);
		}
	else	
		{	
		switch(localAutoMode)
			{
			case OFF:
			case WRITE:
			case TRIMWRITE:
					if(engageLinkMotor[lnkchan-1])
						setFFMotor(fader_number,1);//mettre le motor a ON
					else
						setFFMotor(fader_number,0);//mettre le motor a OFF
					if(linkType[fader_number])
						return FaderLnkLimitCalc((short)(InitalFaderValue[fader_number]+linkOffset[lnkchan-1]));
					return FaderLnkLimitCalc((short)(linkOffset[lnkchan-1]));
			break;
			case READ:												//Si le group en READ ou TOUCH
			case TRIMREAD:
			case TOUCH:
			case TRIMTOUCH:
					if(enableReadVolState[Master_Number][slave_n]&lnkBitIndex[chan])
						{
						if(linkMemberTouched)
							setFFMotor(fader_number,1);//mettre le motor a ON
						else
							setFFMotor(fader_number,0);//mettre le motor a OFF
						if(linkType[fader_number])
							return FaderLnkLimitCalc((short)(DAW_IN_BUF[Master_Number][slave_n][chan]+linkOffset[lnkchan-1]));
						return FaderLnkLimitCalc((short)(linkOffset[lnkchan-1]));
						}
					else
						{
						if(linkMemberTouched)
							setFFMotor(fader_number,1);//mettre le motor a ON
						else
							setFFMotor(fader_number,0);//mettre le motor a OFF
						if(linkType[fader_number])
							return FaderLnkLimitCalc((short)(InitalFaderValue[fader_number]+linkOffset[lnkchan-1]));
						return FaderLnkLimitCalc((short)(linkOffset[lnkchan-1]));
						}
			break;
			default:return FaderLnkLimitCalc(rxSekaFaderValue);
			break;
			}
		}
	}	

//-----------------------------------------------------------------------------------------------------
//--------------------------------------- MUTE LINK MANAGER-------------------------------------------------
//-----------------------------------------------------------------------------------------------------

//Mute from Link 
//-----------------------------------------------------------------------------------------------------
/*
if((!(lnkchan&0x80))&&(lnkchan&0x0f))
	{
	if((fromFaderCutState[Master_Number][slave_n]&lnkBitIndex[lnkchan-1])												//PAS en MUTE
		||(fromFaderCutState[Master_Number][slave_n]&lnkBitIndex[chan])
		||(fromDawCutState[Master_Number][slave_n]&lnkBitIndex[chan]))
		taiCutState[Master_Number][slave_n]=taiCutState[Master_Number][slave_n]|lnkBitIndex[chan];					//Update status un-mute transmi
	else
		taiCutState[Master_Number][slave_n]=taiCutState[Master_Number][slave_n]&~lnkBitIndex[chan];					//Update status un-mute transmi
	}
*/
return FaderLnkLimitCalc(rxSekaFaderValue);
}

//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//FONCTIONS:unsigned short FaderLnkLimitCalc(int faderRelval)
//Verifie MIN/MAX Overflow value for FADER position VS GROUP
//
//
//
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
unsigned short FaderLnkLimitCalc(int faderRelval)
{
if(faderRelval<=0)
	return 0;
else
if(faderRelval>=1000)
	return 999;
return faderRelval;
}
