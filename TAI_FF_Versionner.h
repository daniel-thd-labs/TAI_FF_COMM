//Enlever commentaires pour mettre les buffer 
//a la grosseur du PIC24fj64

//#define __PIC24FJ64
//#define __PROTECT

#define PICPLUG_V1c
//#define PICPLUG_V2

//#define VCA 			0x00
//#define ULTIMATION 	0x80
#define FLYINGFADER 	0xC0
//#define FPMBSSL 		0x10
//#define FPMBGML 		0x20
//#defind FPMBOEM		0x30

//Enlever commentaire pour ajouter le code du boot loader
//#define __BOOTLOADER

//Enlever commentaire pour avoir le code du main prog (applicatino du TAI...)
//#define __MAINPROG

#define SYSTEMSIZE 96

//Enlever commentaire pour communcation du Raspberri pi a haute vistesse (1mHz)
#define __HISPEED232

//Enlever commentaire Si le serveur Raspberry pi est installer
#define __RASP

// a definir pour bypasser le eeprom
// et debugger avec prog connecteur dans le socket 8 pin du eeprom
//#ifndef __RASP_DEBUG
//#define __EEPROM
//#endif

// Enlever commentaire Pour mettre en mode "CODEUR/Stand alone" pour bypasser la detection USB et DAW online
//#define __STANDALONE

// Enlever commentaire Pour mettre en mode "Jig de test" pour bypasser la detection USB et DAW online
//#define __TEST_JIG
