void InitCfgRegBuffers(void);
unsigned char Push_Config_data_In_Queue(unsigned char index, unsigned char data, unsigned char source);
char Pull_Config_data_In_Queue(void);

void InitPassThroughBuffer(void);
char PullPassThroughData(unsigned char *data);
char PushPassThroughData(unsigned char data);
