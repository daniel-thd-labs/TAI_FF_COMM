/*
	[1] = LIBRE
//Config_registers[2] = Set automation index... 0 setall 
												//1 a 127 individual channels, 
												//128 a 139 bank set, 140 all auto mode index 
												//0x80= bank0; 0x81=bank1; 0x82= bank2;0x83= bank3; 
												//0x84= bank4; 0x85=bank5; 0x86= bank6;0x87= bank7;
												//0x88= bank8; 0x89=bank9; 0x8a= bank10;0x8b= bank11; 
												//0x90= Off
												//0x91= Write
												//0x92= Latch
												//0x93= Read
												//0x94=TRIM Off/LIVE
												//0x95=TRIM Write
												//0x96=TRIM Latch
												//0x97=TRIM Read
//Config_registers[3] = 0x80 set all...	automode


//Config_registers[4] = Recall_it mode
//Config_registers[5] = Recall_it channel/switch select

//Config_registers[6] = OTB/HYBRIDE mode 0x01/0x00 et 0x02 pour SSL mode
//Config_registers[7] = On stop mode (0x00=nothing,0x01=read,0x02=read trim)
//Config_registers[8] = Master relay bit 0x00= channel 32=32, 0x01 channel 32=master
//Config_registers[9] = Fader_scaling_flag 0 =pt_legacy_scale, 1=pt11_scale
//Config_registers[10] = Mix on off = 0x01 et  interface modele bit7 : 1=MF  0=VCA
//Config_registers[11] = Csurf mode  0=HUI 1=TAI			
//Config_registers[12] = fader bank slide(1/0=up/down, lsb = slave number 0b00:0b01:0b10:0b11)
//Config_registers[13] = LSB0-1=Group_focus for bank slide LSB 2-3 =group mask
//Config_registers[14] = Automation status swtich Toggle modes

//Config_registers[15] = MAX SLAVE NUMBER +1.
//Config_registers[16] = VCA BANK LOCATION
//Config_registers[17] = systeme status/Utility flags. 0x01 = save to eeprom
							0x01 = save to eeprom flag
							0x02 = USBSENS
							0x04 = USB CONFIGURED
							0x08 = DAW FLAG ONLINE 
//Config_registers[18] = MASTER1 System_size.
//Config_registers[18] = MASTER2 System_size.
//Config_registers[19] = MASTER3 System_size.
//Config_registers[20] = Lock track state.
//Config_registers[21] = Master Fader interface channel number
//Config_registers[22] = Automation modifier modes(snap,rehears, write on stop,etc)  
//	
//Config_registers[24] = level match
//Config_registers[25] = moving fader setup mode register
//Config_registers[28] = SlaveVersion.
//Config_registers[29] = MasterVersion.
//Config_registers[30] = CommVersion.
//Config_registers[31] = RESET request
*/