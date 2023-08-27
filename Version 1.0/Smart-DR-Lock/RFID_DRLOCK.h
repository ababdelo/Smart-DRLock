
#ifndef RFID_DRLOCK_H
#define RFID_DRLOCK_H

/*************************************Including needed Libraries*************************************/

#include <EEPROM.h>             // We are going to read and write PICC's UIDs from/to EEPROM
#include <MFRC522.h>            // Library for Mifare RC522 Devices
#include <SPI.h>                // RC522 Module uses SPI protocol
#include <LiquidCrystal_I2C.h>  // Library for LCD_I2C
/*
  Instead of a RELAYDOOR maybe you want to use a servo
  Servos can lock and unlock door locks too
  There are examples out there.
  // #include <Servo.h>
*/

/***************************************************************************************************/

/**************************************Definin' some Macros*****************************************/

//#define COMMON_ANODE

/*
  If u're using common anode led, Uncomment '#define COMMON_ANNODE' line 22
   digitalWriting HIGH turns OFF led
  Mind that if you are going to use common cathode led
  just simply comment out #define COMMON_ANODE,
*/

#ifdef COMMON_ANODE
#define LED_ON 0
#define LED_OFF 15
#else
#define LED_ON 15
#define LED_OFF 0
#endif

/***************************************************************************************************/

/**************************************Definin' Pins********************************************/

#define SS_PIN 10 // Configurable, see typical pin layout above
#define RST_PIN 9 // Configurable, see typical pin layout above

/*
  Typical pin layout used:

  -------------------------------------------------------------------------------------------------
  |           |  MFRC522    |  Arduino     |  Arduino  | Arduino  |  Arduino        |  Arduino    |
  |  Signal   |  Reader/PCD |  Uno/101     |  Mega     | Nano v3  |  Leonardo/Micro |  Pro Micro  |
  |           |  Pin        |  Pin         |  Pin      | Pin      |  Pin            |  Pin        |
  |-----------|-------------|--------------|-----------|----------|-----------------|-------------|
  | RST/Reset |  RST        |  9           |  5        | D9       |  RESET/ICSP-5   |  RST        |
  | SPI SS    |  SDA(SS)    |  10          |  53       | D10      |  10             |  10         |
  | SPI MOSI  |  MOSI       |  11 / ICSP-4 |  51       | D11      |  ICSP-4         |  16         |
  | SPI MISO  |  MISO       |  12 / ICSP-1 |  50       | D12      |  ICSP-1         |  14         |
  | SPI SCK   |  SCK        |  13 / ICSP-3 |  52       | D13      |  ICSP-3         |  15         |
  -------------------------------------------------------------------------------------------------

   More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
*/

// Set Led Pins
#define REDLED 6
#define GREENLED 5
#define BLUELED 3

/*
  For visualizing whats going on hardware
  we need some leds and to control door lock
  a RELAYDOOR and a wipe button (or some other hardware)
*/

#define RST 2

#define RELAYDOOR 4  // Set RELAYDOOR Pin

#define BTNERASE A0  // Button pin for WipeMode
#define BTNDOOR A1   // Button pin for WipeMode

/***************************************************************************************************/

/***************************************Creating instances******************************************/

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
LiquidCrystal_I2C lcd(0x26, 16, 2);  // to know the address of your i2c devices just use "I2c_scan" sketch example from the Libray "Wire" :)

/***************************************************************************************************/

/************************************Declaring used Variables***************************************/

String uname;
String urole;
String uid;

String msg;

boolean ismatch = false;        // initialize card match to false
boolean programMode = false;  // initialize programming mode to false
boolean replaceMaster = false;

unsigned int cntr = 9;  // Erasing Counter

int counter;
int successRead;  // boolean Variable to keep if we have Successful Read from Reader

byte storedCard[4];  // Stores an ID read from EEPROM
byte readCard[4];    // Stores scanned ID read from RFID Module
byte masterCard[4];  // Stores master card's ID read from EEPROM

/***************************************************************************************************/

/************************************Declaring used Functions***************************************/

// Declare FT_UID Functions :
int getID();
int findIDSLOT(byte find[]);

boolean isMaster(byte test[]);
boolean findID(byte find[]);
boolean checkTwo(byte a[], byte b[]);

void deleteID(byte a[]);
void readID(int number);
void writeID(byte a[]);
char Cap_Char(char c);

void WriteData2Tag(byte block, byte name[]);
void readDataFromTag(byte blockName, byte blockRole);

void stringToBytes(char* str, byte* bytes, int length);
String convbyte2string(byte *UID_B);



//Declare FT_UTILS Functions :
void granted(int setDelay, String permission);
void denied();
void cycleLeds();
void normalModeOn();
void dsplreaderdetails();
void successWrite();
void failedWrite();
void successRemove();
void print2lcd(String str1, int c1, String str2, int c2);

/***************************************************************************************************/

#endif
