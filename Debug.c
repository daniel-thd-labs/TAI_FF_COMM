#include "Debug.h"
unsigned short FlagResetCntr;

/********************************************************************
*******************************************************************
* Function:        void DebugSet(unsigned short err)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Note:            None
*
*******************************************************************
 *******************************************************************/
void DebugSet(unsigned short err)
{
if(err)
	{
	Buffer_Over_Run=Buffer_Over_Run|err;
	global_error_flag=global_error_flag|Buffer_Over_Run;
	}
}
//0x0001 USB BUFFER OVERFLOW FLAG
//0x0002 CONFIG REG QUEUE BUFFER OVERFLOW FLAG
//0x0004 EI485 RX BUFFER OVERFLOW FLAG
//0x0008 EI485 TX BUFFER OVERFLOW FLAG
//0x0010 MIDI RX BUFFER OVERFLOW FLAG
//0x0020 MIDI TX BUFFER OVERFLOW FLAG
//0x0040 RASP RX BUFFER OVERFLOW FLAG
//0x0080 RASP TX BUFFER OVERFLOW FLAG
//0x0100 RASP RECALL TX BUFFER OVERFLOW FLAG
//0x0200 EI485 MSG TX BUFFER OVERFLOW FLAG


/********************************************************************
*******************************************************************
* Function:        void DebugClear(unsigned short err)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Note:            None
*
*******************************************************************
 *******************************************************************/
void DebugClear(unsigned short err)
{
if(err)
	{
	Buffer_Over_Run=Buffer_Over_Run&~err;
	global_error_flag=Buffer_Over_Run;
	}	
}

	
/********************************************************************
*******************************************************************
* Function:        void Overflow_Process(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Notes:
*
*
*******************************************************************
*******************************************************************/
void Overflow_Process1(void)
{
if(Buffer_Over_Run!=Buffer_Over_Run)
	{
	if(!FlagResetCntr)
		FlagResetCntr=1000;
	else
		FlagResetCntr--;
	}
if(FlagResetCntr==1)
	{
	Buffer_Over_Run=Buffer_Over_Run;
	FlagResetCntr=0;
	}
}
