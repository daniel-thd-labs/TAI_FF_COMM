#include "PicPlug_Hardware_Mapping.h"
#include "TAI_FF_globales_externes.h"
#include "TAI_FF_power_comm_switch_manager_V1_0.h"
#include "TAI_FF_power_comm_Group_Link_manager_23_02_2020.h"

const char SwBitIndex[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
#define ON 1
#define OFF 0

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void switchTester(unsigned char chanBlock,unsigned char master)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void switchTester(unsigned char chanBlock,unsigned char master)
{
unsigned char currentSwtState,i,n,chan;
for(n=0;n<8;n++)
	{
	chan=(master*32)+(chanBlock*8)+n;
	if(rxFromSekaSwtData[chan]!=0xff)
		{
		currentSwtState=rxFromSekaSwtData[chan];
		if(currentSwtState!=swtMode[chan])//il y a un changement
			{
			for(i=0;i<8;i++) //on le traite
				{
				if((currentSwtState&SwBitIndex[i])!=(swtMode[chan]&SwBitIndex[i]))	//test pour la switch qui a changée, 
					{
					swtMode[chan]=(swtMode[chan]&~SwBitIndex[i])|(currentSwtState&SwBitIndex[i]);	//update switch bit state
					switchChangeManager(chan,currentSwtState,i);
					}
				}
			}
		}
	}
	
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:MCMASwitchTester(void)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCMASwitchTester(void)
{
unsigned char index,mstNum;

for(mstNum=0;mstNum<4;mstNum++)
	{
	if(MCMASwitch[mstNum]!=0xff)
		{
		if(MCMASwitch[mstNum]!=MCMASwtMode[mstNum])				//il y a un changement
			{
			for(index=0;index<8;index++)						//on le traite
				{
				if((MCMASwitch[mstNum]&SwBitIndex[index])!=(MCMASwtMode[mstNum]&SwBitIndex[index]))	//test pour la switch qui a changée, 
					MCMASwitchChangeManager(mstNum,MCMASwtMode[mstNum]&SwBitIndex[index],SwBitIndex[index]);
				}
			MCMASwtMode[mstNum]=MCMASwitch[mstNum];			//update switch bit state
			}
		}
	}	
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void SwitchStateManager(unsigned char chan,unsigned char switchState,unsigned char Mode);
//Notes:		SWT_Mode[]is the current switch state		
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void switchChangeManager(unsigned char chan,unsigned char switchState,unsigned char index)
{
//0: newly pressed goto 1
//1: debouncing to ON then goto 2
//2:Switch on management process on then goto 3
//3:Switch hold management test on hold activity test for switch OFF
//4 debounce to OFF

if(switchState&SwBitIndex[index])												//switch=1
	{
	if(!(prevSwtOnMode[chan]&SwBitIndex[index]))								//premiere fois quon passe par ici
		{
		chnSwtPressedManager(chan,switchState,index);						//Channel Switch ON processing
		prevSwtOnMode[chan]=prevSwtOnMode[chan]|SwBitIndex[index];
		prevSwtOffMode[chan]=prevSwtOffMode[chan]&~SwBitIndex[index];
		}
	}
else																			//switch=0
	{
	if(!(prevSwtOffMode[chan]&SwBitIndex[index]))
		{
		chnSwtReleasedManager(chan,prevSwtOnMode[chan],index);				//Channel Switch OFF processing
		prevSwtOffMode[chan]=prevSwtOffMode[chan]|SwBitIndex[index];
		prevSwtOnMode[chan]=prevSwtOnMode[chan]&~SwBitIndex[index];
		}
	}
}
//MCMA
//MCA
//GCA
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:void MCMASwitchChangeManager(unsigned char mstNum,unsigned char switchState,unsigned char index)
//Notes:		SWT_Mode[]is the current switch state		
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCMASwitchChangeManager(unsigned char mstNum,unsigned char switchState,unsigned char index)
{
//0: newly pressed goto 1
//1: debouncing to ON then goto 2
//2:Switch on management process on then goto 3
//3:Switch hold management test on hold activity test for switch OFF
//4 debounce to OFF

if(mstNum==0)
	{
	if(switchState)
		GCA_swtPressedManager(switchState,index);				//Global Switch OFF processing
	else
		GCA_swtReleasedManager(switchState,index);				//Global Switch OFF processing
	}

else
if(mstNum==1)
	{
	if(switchState)
		MCA_swtPressedManager(switchState,index);				//Master Switch ON processing
	else
		MCA_swtReleasedManager(switchState,index);				//Master Switch ON processing
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:chnSwtPressedManager(unsigned char chan,unsigned char bitmask)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
//Overview:
// 0x01= mute write switch
// 0x02= Solo switch
// 0x04= Select switch
// 0x08= write switch
// 0x10= automatch switch (read)
// 0x20= 
// 0x40=Fader Touch
// 0x80=MuteSwt
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void chnSwtPressedManager(unsigned char chan,unsigned char switchState,unsigned char index)													//Switch OFF processing
{
unsigned char mode;
mode=switchState&SwBitIndex[index];

switch(mode)
	{
	case 0x01:
	break;
	case 0x02:
	break;

//Select switch
	case 0x04:
			selectSwtManager(chan,1);
	break;

//WRITE switch
	case 0x08:
			if((!creatGroupMode)&&(!creatGroupMode))
				if(switchState&0x10)
					multiSwtCode[chan]=multiSwtCode[chan]|0x18;
	break;
	
//AUTOMATCH (read)switch
	case 0x10:
			if((!creatGroupMode)&&(!creatGroupMode))
				if(switchState&0x08)//+WRITE switch
					multiSwtCode[chan]=multiSwtCode[chan]|0x18;
	break;

	break;
	case 0x20:
	break;

//TOUCH switch
	case 0x40:
			faderTouchManager(chan,1);
	break;

//Mute switch
	case 0x80:
			muteSwtManager(chan,mode);
	break;
	default:break;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:chnSwtReleasedManager(unsigned char chan,unsigned char switchState,unsigned char index)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void chnSwtReleasedManager(unsigned char chan,unsigned char switchState,unsigned char index)
{
unsigned char mode=switchState&SwBitIndex[index];
switch(mode)
	{
	case 0x01:
			if((!creatGroupMode)&&(!creatGroupMode))
				muteRecSwtManager(chan,mode);
			//AutomodeSelect(chan,bitmask);					//mute record switch
	break;

//Solo switch
	case 0x02:
			if((!creatGroupMode)&&(!creatGroupMode))
				soloSwtManager(chan,mode);					
	break;

//Select switch
	case 0x04:
			selectSwtManager(chan,0);					
	break;
	case 0x08:

//AUTOMATCH switch
	case 0x10:
			if((!creatGroupMode)&&(!creatGroupMode))
				automodeSelect(chan,mode);					//WRITE switch
	break;

	case 0x20:
	break;

//TOUCH switch
	case 0x40:
			faderTouchManager(chan,0);
	break;

	case 0x80:
	break;
	default:break;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:MCA_SwtPressedManager(unsigned char switchState,unsigned char index)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
// 0x01=link
// 0x02=RSI
// 0x04=OTHER
// 0x08=RUN
// 0x10=+
// 0x20=-
// 0x40=shift
// 0x80=group
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_swtPressedManager(unsigned char switchState,unsigned char index)		//Switch OFF processing
{
switch(index)
	{

// 0x01=link
	case 0x01:
			MCA_linkSwtManager(switchState);
	break;

// 0x02=RSI
	case 0x02:
		if((!creatGroupMode)&&(!creatLinkMode))
			MCA_rsiSwtManager(switchState);
	break;

// 0x04=OTHER
	case 0x04:
		if((!creatGroupMode)&&(!creatLinkMode))
			MCA_otherSwtManager(switchState);
	break;

//RUN
	case 0x08:
		MCA_runSwtManager(switchState);
	break;

//GROUP switch
	case 0x80:
			MCA_groupSwtManager(switchState);
	break;

//+ switch
	case 0x10:
		if((!creatGroupMode)&&(!creatLinkMode))
			MCA_plusSwtManager(switchState);				
	break;

//- switch
	case 0x20:
		if((!creatGroupMode)&&(!creatLinkMode))
			MCA_minusSwtManager(switchState);				
	break;

//Shift switch
	case 0x40:
		if((!creatGroupMode)&&(!creatLinkMode))
			MCA_shiftSwtManager(1);							
	break;

	default:break;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:MCA_swtReleasedManager(unsigned charChan,unsigned char bitmask)
//Notes: Uncomment to enable release switch management				
//
//Arguments:
//Variables locals:
//
//Variables globals:
// 0x01=link
// 0x02=RSI
// 0x04=OTHER
// 0x08=RUN
// 0x10=+
// 0x20=-
// 0x40=shift
// 0x80=group
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_swtReleasedManager(unsigned char switchState,unsigned char index)													//Switch OFF processing
{
switch(index)	
	{
// 0x01=link
	case 0x01:
		//if(!creatGroupMode)
		//	MCA_linkSwtManager(switchState);
	break;

	case 0x02:
		//rsiSwtManager(chan,switchState);
	break;
	case 0x04:
		//otherSwtManager(chan,switchState);
	break;
	case 0x80:
		//if(!creatGroupMode)
		//	MCA_groupSwtManager(switchState);				//GROUP switch
	break;
	case 0x10:
		//if((!creatGroupMode)&&(!creatGroupMode))
			MCA_plusSwtManager(switchState);				//+ switch
	break;
	case 0x20:
		//if((!creatGroupMode)&&(!creatGroupMode))
			MCA_minusSwtManager(switchState);				//- switch
	break;
	case 0x40:
		//if((!creatGroupMode)&&(!creatGroupMode))
			MCA_shiftSwtManager(switchState);				//Shift switch
	break;
	case 0x08://RUN
		//runSwtManager(chan,switchState);
	break;
	default:break;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:GCA_swtPressedManager(unsigned char switchState,unsigned char index)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
// 0x01= mute write switch
// 0x02= Solo switch
// 0x04= Select switch
// 0x08= write switch
// 0x10= automatch switch (read)
// 0x20= 
// 0x40=Fader Touch
// 0x80=MuteSwt

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void GCA_swtPressedManager(unsigned char switchState,unsigned char index)													//Switch OFF processing
{
unsigned char test;
switch(index)
	{
	case 0x01:
	break;
	case 0x02:
	break;
	case 0x04:
	break;
	case 0x08:													
		if((!creatGroupMode)&&(!creatGroupMode))
			if(switchState&SwBitIndex[index])									
				test=0; 					//multiSwtCode[chan]=multiSwtCode[chan]|0x18;
	break;
	case 0x10:														//AUTOMATCH switch
		if((!creatGroupMode)&&(!creatGroupMode))
			if(switchState&SwBitIndex[index])									//WRITE switch
				test=0; //multiSwtCode[chan]=multiSwtCode[chan]|0x18;
	break;

	break;
	case 0x20:
	break;
	case 0x40:													//TOUCH switch
	break;
	case 0x80:
			//muteSwtManager(mode);								//Mute switch
	break;
	default:break;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:GCA_swtReleasedManager(unsigned charChan,unsigned char bitmask)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
// 0x01= mute write switch
// 0x02= Solo switch
// 0x04= Select switch
// 0x08= write switch
// 0x10= automatch switch (read)
// 0x20= 
// 0x40=Fader Touch
// 0x80=MuteSwt
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void GCA_swtReleasedManager(unsigned char switchState,unsigned char index)													//Switch OFF processing
{
switch(index)
	{
	case 0x01:break;			//mute write switch
	case 0x02:break;			//Solo switch
	case 0x04:break;			//Select switch
	case 0x80:break;			//write switch
	case 0x10:break;			//automatch switch (read)
	case 0x20:break;			//Fader Touch
	case 0x40:break;			//
	case 0x08:break;			//
	default:break;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:chnSwtHoldManager(unsigned char chan,unsigned char switchState,unsigned char index)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void chnSwtHoldManager(unsigned char chan,unsigned char switchState,unsigned char index)
{

}



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void automodeSelect(unsigned char chan,unsigned char Switch)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
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
char automodeSelect(unsigned char chan,unsigned char AutoSwitch)
{
int Master=chan/32;
int Slave=(chan/8)%4;
int Chnidx=chan%8;
if(UDPActiveChannelFlag[Master][Slave]&SwBitIndex[Chnidx])
	{
	switch(AutoSwitch)
		{
		case 0x01:	//mute record
		break;		

		case 0x08:	//Record
			if(!(multiSwtCode[chan]&0x18))
				{
				if(AUTOMODE[chan]!=1)
					AUTOMODE[chan]=1;
					else
					AUTOMODE[chan]=0;
				multiSwtCode[chan]=multiSwtCode[chan]&~0x18;
				}
			else
			if((multiSwtCode[chan]&0x18)==0x18)
				{
				if((prevSwtOnMode[chan]&0x18)==0x08)
					multiSwtCode[chan]=multiSwtCode[chan]&~0x18;
				AUTOMODE[chan]=2;
				}
		break;	

		case 0x10:	//Automatch switch
			if(!(multiSwtCode[chan]&0x18))
				{
				if(AUTOMODE[chan]!=3)
					AUTOMODE[chan]=3;
				else
					AUTOMODE[chan]=0;
				multiSwtCode[chan]=multiSwtCode[chan]&~0x18;
				}
			else
			if((multiSwtCode[chan]&0x18)==0x18)
				{
				if((prevSwtOnMode[chan]&0x18)==0x10)
					multiSwtCode[chan]=multiSwtCode[chan]&~0x18;
				AUTOMODE[chan]=2;
				}
		break;
		}
	ledAutoModeMng(chan);
	}
//else
//	AUTOMODE[chan]=0;
return 1;
}
			
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FUNCTIONS:char ledAutoModeMng(unsigned char chan)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
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
char ledAutoModeMng(unsigned char chan)
{
int Master=chan/32;
int Slave=(chan/8)%4;
char bitmax=SwBitIndex[chan%8];
char invbitmax=~SwBitIndex[chan%8];

switch(AUTOMODE[chan])
	{
	case 0:	//TxChannelMsgSect1[chan]=(TxChannelMsgSect1[chan]&0xbfff);		//motor automode =OFF 0x1011 1111 1111 1111 
			//FF_MuteRec_State[chan/12]=FF_MuteRec_State[chan/12]&invbitmax;
			txToSekaFaderLedState[chan]=(txToSekaFaderLedState[chan]&0xcf);						// 0x1100 1111
			automodeRegisters[Master][Slave][0]=(automodeRegisters[Master][Slave][0]&invbitmax);
			automodeRegisters[Master][Slave][1]=(automodeRegisters[Master][Slave][1]&invbitmax);
			automodeRegisters[Master][Slave][2]=(automodeRegisters[Master][Slave][2]&invbitmax);

	break;
	case 1:	//TxChannelMsgSect1[chan]=(TxChannelMsgSect1[chan]&0xbfff);		//motor automode =OFF
			txToSekaFaderLedState[chan]=(txToSekaFaderLedState[chan]&0xcf)|0x10;				// 0xyy01 xxxx
			automodeRegisters[Master][Slave][0]=(automodeRegisters[Master][Slave][0]&invbitmax)|bitmax;
			automodeRegisters[Master][Slave][1]=(automodeRegisters[Master][Slave][1]&invbitmax);
			automodeRegisters[Master][Slave][2]=(automodeRegisters[Master][Slave][2]&invbitmax);
	break;
	case 2:	//TxChannelMsgSect1[chan]=(TxChannelMsgSect1[chan]|0x4000);		//motor automode =ON 0x0100 0000 0000 0000
			txToSekaFaderLedState[chan]=(txToSekaFaderLedState[chan]&0xcf)|0x30;				// 0xyy11 xxxx
			automodeRegisters[Master][Slave][0]=(automodeRegisters[Master][Slave][0]&invbitmax);
			automodeRegisters[Master][Slave][1]=(automodeRegisters[Master][Slave][1]&invbitmax)|bitmax;
			automodeRegisters[Master][Slave][2]=(automodeRegisters[Master][Slave][2]&invbitmax);
	break;
	case 3:	//TxChannelMsgSect1[chan]=(TxChannelMsgSect1[chan]|0x4000);		//motor automode =ON
			txToSekaFaderLedState[chan]=(txToSekaFaderLedState[chan]&0xcf)|0x20;
			automodeRegisters[Master][Slave][0]=(automodeRegisters[Master][Slave][0]&invbitmax)|bitmax;
			automodeRegisters[Master][Slave][1]=(automodeRegisters[Master][Slave][1]&invbitmax)|bitmax;
			automodeRegisters[Master][Slave][2]=(automodeRegisters[Master][Slave][2]&invbitmax);
	break;
	case 4:
	break;
	case 5:
	break;
	case 6:
	break;
	case 7:
	break;
	}
return 1;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void faderTouchManager(unsigned char chan)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
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
void faderTouchManager(unsigned char chan,unsigned char switchState)
{
int bank=chan/32;
int slave=(chan/8)%4;

if(!switchState)
	{
	fromFaderTouchState[bank][slave]=fromFaderTouchState[bank][slave]&~SwBitIndex[chan%8];
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xbf;
	}
else
	{
	fromFaderTouchState[bank][slave]=fromFaderTouchState[bank][slave]|SwBitIndex[chan%8];
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x40;
	if(creatGroupMode)
		groupTouchedMng(chan);
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void muteSwtManager(unsigned char chan)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
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
void muteSwtManager(unsigned char chan,unsigned char switchState)
{
int bank=chan/32;
int slave=(chan/8)%4;
if(fromFaderCutState[bank][slave]&(SwBitIndex[chan%8]))
	{
	fromFaderCutState[bank][slave]=fromFaderCutState[bank][slave]&~SwBitIndex[chan%8];
//	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfe;
	}
else
	{
	fromFaderCutState[bank][slave]=fromFaderCutState[bank][slave]|SwBitIndex[chan%8];
//	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x01;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void MCA_rsiSwtManager(unsigned char switchState)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
// 0x02=RSI
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_rsiSwtManager(unsigned char switchState)
{
masterSwtStates^=0x02;
if(masterSwtStates&0x02)
	MCMALedState[1]=MCMALedState[1]|0x04;
else
	MCMALedState[1]=MCMALedState[1]&0xfb;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void MCA_otherSwtManager(unsigned char chan,unsigned char switchState)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
// 0x04=OTHER
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_otherSwtManager(unsigned char switchState)
{
masterSwtStates^=0x04;
if(masterSwtStates&0x04)
	MCMALedState[1]=MCMALedState[1]|0x08;
else
	MCMALedState[1]=MCMALedState[1]&0xf7;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void MCA_runSwtManager(unsigned char switchState)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
// 0x01=run
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_runSwtManager(unsigned char switchState)
{
if((Config_registers[10]&0x01)
	&&!gotoMixOnFlag)									//MIX OFF
	{
	if(!gotoMixOffFlag)
		gotoMixOffFlag=1;
	}
else
if((USBStateFlag&&(REAPER_FLAG||HUI_FLAG||UDP_FLAG))//MIX ON
	&&!gotoMixOffFlag)									//pas en recallIt
		gotoMixOnFlag=1;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void MCA_plusSwtManager(unsigned char switchState)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
// 0x10=plus
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_plusSwtManager(unsigned char switchState)
{
if(switchState)
	{
	masterSwtStates=masterSwtStates|0x10;
	MCMALedState[1]=MCMALedState[1]|0x20;
	}
else
	{
	masterSwtStates=masterSwtStates&0xef;
	MCMALedState[1]=MCMALedState[1]&0xdf;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void MCA_minusSwtManager(unsigned char chan,unsigned char switchState)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
// 0x20=minus
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_minusSwtManager(unsigned char switchState)
{
if(switchState)
	{
	masterSwtStates=masterSwtStates|0x20;
	MCMALedState[1]=MCMALedState[1]|0x80;
	}
else
	{
	masterSwtStates=masterSwtStates&0xdf;
	MCMALedState[1]=MCMALedState[1]&0x7f;
	}
}



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void MCA_shiftSwtManager(unsigned char chan,unsigned char switchState)
//Notes:						
//
//Arguments:
//Variables locals:
//
//Variables globals:
//
// Master LEDS
//	0x01=GROUPS
//	0x02=LINKS
//	0x04=RSI
//	0x08=OTHERS
//	0x10=RUN
//	0x20=PLUS
//	0x40=MINUS
//	0x80=SHIFT

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void MCA_shiftSwtManager(unsigned char switchState)
{
if(switchState)
	{
	shiftSwtState=0x01;
	MCMALedState[1]=MCMALedState[1]|0x80;
	}
else
	{
	MCMALedState[1]=MCMALedState[1]&~0x80;
	shiftSwtState=0x00;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void soloSwtManager(unsigned char chan,unsigned char switchState)
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
void soloSwtManager(unsigned char chan,unsigned char switchState)
{
if(FF_Solo_State[chan/12]&(SwBitIndex[chan%8]))
	{
	FF_Solo_State[chan/12]=FF_Solo_State[chan/12]&~SwBitIndex[chan%8];
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfb;
	}
else
	{
	FF_Solo_State[chan/12]=FF_Solo_State[chan/12]|SwBitIndex[chan%8];
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x04;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:	void muteRecSwtManager(unsigned char chan)
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
void muteRecSwtManager(unsigned char chan,unsigned char switchState)
{
if(FF_MuteRec_State[chan/12]&(SwBitIndex[chan%8]))
	{
	FF_MuteRec_State[chan/12]=FF_MuteRec_State[chan/12]&~SwBitIndex[chan%8];
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfd;
	}
else
	{
	FF_MuteRec_State[chan/12]=FF_MuteRec_State[chan/12]|SwBitIndex[chan%8];
	txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x02;
	}
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
char selectSwtHold(int chan) { if(swtMode[chan]&0x04)return 1; return 0; }
char writeSwtHold(int chan) { if(swtMode[chan]&0x08)return 1; return 0; }
char soloSwtHold(int chan) { if(swtMode[chan]&0x02)return 1; return 0; }
char touchSwtHold(int chan) { if(swtMode[chan]&0x40)return 1; return 0; }
char muteSwtHold(int chan) { if(swtMode[chan]&0x80)return 1; return 0; }


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//FONCTIONS:void updateFFChanLeds(int code,int chan,char state)
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
void updateFFChanLeds(int code,int chan,char state)
{
if((chan!=masterSwtNbr)&&(chan!=globalSwtNbr))
	{
	if(state)
		{
		switch(code)
			{
			case 0x01:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x01;break;
			case 0x02:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x02;break;
			case 0x04:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x04;break;
			case 0x08:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x08;break;
			case 0x10:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x10;break;
			case 0x20:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x20;break;
			case 0x40:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x40;break;
			case 0x80:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]|0x80;break;
			}	
		}
	else
		{
		switch(code)
			{
			case 0x01:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfe;break;
			case 0x02:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfd;break;
			case 0x04:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xfb;break;
			case 0x08:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xf7;break;
			case 0x10:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xef;break;
			case 0x20:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xdf;break;
			case 0x40:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0xbf;break;
			case 0x80:txToSekaFaderLedState[chan]=txToSekaFaderLedState[chan]&0x7f;break;
			}
		}
	}
}
