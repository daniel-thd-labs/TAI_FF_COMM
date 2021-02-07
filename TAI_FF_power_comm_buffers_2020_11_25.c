#include "TAI_FF_power_comm_buffers.h"
#include "Debug.h"

#define passThroughBufferSize 256
#define cfgRegBufferSize 64

unsigned char config_index_queue[cfgRegBufferSize];
unsigned char config_data_queue[cfgRegBufferSize];
unsigned char config_source_queue[cfgRegBufferSize];
unsigned char Cdqueue_size;
unsigned char Cdqueue_top;
unsigned char Cdqueue_bot;
unsigned char Cdqueue_quantity;
unsigned char Cdqueue_over;

unsigned char passThroughBuffer[passThroughBufferSize];
unsigned short passThroughBufferBot;
unsigned short passThroughBufferTop;
unsigned short passThroughBufferQuant;

extern unsigned char Config_registers[64];

//--------------------------------------------------------------
//--------------------------------------------------------------
//FONCTION: void InitCfgRegBuffers(void)
//
//
//--------------------------------------------------------------
//--------------------------------------------------------------
void InitCfgRegBuffers(void)
{
unsigned short n;
for (n=0;n<64;n++)
	{
	config_index_queue[n]=0;
	config_data_queue[n]=0;
	config_source_queue[n]=0;
	}
Cdqueue_size=64;
Cdqueue_top=0;
Cdqueue_bot=0;
Cdqueue_quantity=0;
Cdqueue_over=0;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//FONCTION: void InitPassThroughBuffer(void)
//
//
//--------------------------------------------------------------
//--------------------------------------------------------------
void InitPassThroughBuffer(void)
{
unsigned short n;
for (n=0;n<passThroughBufferSize;n++)
	passThroughBuffer[n]=0;
passThroughBufferBot=0;
passThroughBufferTop=0;
passThroughBufferQuant=0;
}

/****************************************************************
*****************************************************************
|FONCTIONS:void Push_Config_data_In_Queue(						|
|								unsigned char config_number, 	|
|								unsigned char data)				|
|Notes:	Pousse une donnée de config dans le buffer circulaire	|
|		avec test d'overflow, pou evnoyer au master				|
|arguments 	:no de config										|
|			:donnée de config									|
|																|
|																|
*****************************************************************
 ****************************************************************/
unsigned char Push_Config_data_In_Queue(unsigned char index, unsigned char data, unsigned char source)
{
if(Cdqueue_quantity>=Cdqueue_size)
	return 0;							//si oui roule au début
config_index_queue[Cdqueue_top]=index;
config_data_queue[Cdqueue_top]=data;
config_source_queue[Cdqueue_top]=source;
Cdqueue_top++;
if (Cdqueue_top >= Cdqueue_size)		//test si on doit rouler au début
	Cdqueue_top = 0;
Cdqueue_quantity++;
if(Cdqueue_quantity>=Cdqueue_size)
	DebugSet(0x0002);
return 1;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//FUNCTIONS:Pull_Config_data_In_Queue(void)
//										
//										
//Notes:	Retire une donnée de config dans le buffer circulaire
//		pour evnoyer au master
//arguments 	:no de config
//			:donnée de config
//			retourne la source sinon pour vide
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char Pull_Config_data_In_Queue(void)
{
unsigned char source;
if(!Cdqueue_quantity)
	return -1;
Config_registers[0] = (config_index_queue[Cdqueue_bot]);
Config_registers[Config_registers[0]]=(config_data_queue[Cdqueue_bot]);
source=config_source_queue[Cdqueue_bot];
Cdqueue_bot++;
Cdqueue_quantity--;
if (Cdqueue_bot >= Cdqueue_size)			//test si on doit rouler au début
	Cdqueue_bot = 0;					//si oui roule au début
Cdqueue_over=0;
return source;
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//FUNCTIONS:char PullPassThroughData(void *data)
//										
//										
//Notes:
//arguments data pointer
//Return error or status
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char PullPassThroughData(unsigned char *data)
{
if(!passThroughBufferQuant)
	return -1;
*data=passThroughBuffer[passThroughBufferBot];
passThroughBufferBot++;
if(passThroughBufferBot>=256)
	passThroughBufferBot=0;
passThroughBufferQuant--;
return passThroughBufferQuant;
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//FONCTIONS:char PushPassThroughData(unsigned char data)
//										
//										
//Notes:
//arguments data
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char PushPassThroughData(unsigned char data)
{
if(passThroughBufferQuant>=passThroughBufferSize)
	return 0;							//si oui roule au début
passThroughBuffer[passThroughBufferTop]=data;
passThroughBufferTop++;

if (passThroughBufferTop >= passThroughBufferSize)		//test si on doit rouler au début
	passThroughBufferTop = 0;

passThroughBufferQuant++;
if(passThroughBufferQuant >= passThroughBufferSize)
	DebugSet(0x0002);

return 1;
}
