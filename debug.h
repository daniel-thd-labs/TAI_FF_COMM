extern unsigned short global_error_flag;
extern unsigned short Buffer_Over_Run;
//#define __DEBUGBUFFER

/********************************************************************
*******************************************************************
* Function:        void DebugSet(unsigned char err)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Note:            None
*
*******************************************************************
 *******************************************************************/
void DebugSet(unsigned short err);


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
void DebugClear(unsigned short err);


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
void Overflow_Process1(void);
