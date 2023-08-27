
#include "RFID_DRLOCK.h"

/******************************************Get PICC's UID*******************************************/

int getID() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  // Serial.println(F("Scanned PICC's UID:"));
  for (int x = 0; x < 4; x++) {  //
    readCard[x] = mfrc522.uid.uidByte[x];
    // Serial.print(readCard[x], HEX);
  }
  // readDataFromTag(4, 5); // Pass block numbers to read name and role
  uid = convbyte2string(readCard);
  mfrc522.PICC_HaltA();       // Stop reading
  mfrc522.PCD_StopCrypto1();  //Used to exit the PCD from its authenticated state.
  // Remember to call this function after communicating with an authenticated PICC - otherwise no new communications can start.
  return 1;
}

/***************************************************************************************************/

/******************************** Check readCard IF is masterCard***********************************/

// Check to see if the ID passed is the master programing card
boolean isMaster(byte test[]) {
  if (checkTwo(test, masterCard))
    return true;
  else
    return false;
}

/***************************************************************************************************/

/*****************************************Find ID From EEPROM***************************************/

boolean findID(byte find[]) {
  int count = EEPROM.read(0);          // Read the first Byte of EEPROM that
  for (int i = 1; i <= count; i++) {   // Loop once for each EEPROM entry
    readID(i);                         // Read an ID from EEPROM, it is stored in storedCard[4]
    if (checkTwo(find, storedCard)) {  // Check to see if the storedCard read from EEPROM
      return true;
      //break;  // Stop looking we found it
    } else {}// If not, return false
  }
  return false;
}

/***************************************************************************************************/

/*****************************************Remove ID from EEPROM*************************************/

void deleteID(byte a[]) {
  if (!findID(a)) {  // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite();   // If not
    // Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  } else {
    int num = EEPROM.read(0);  // Get the numer of used spaces, position 0 stores the number of ID cards
    int slot;                  // Figure out the slot number of the card
    int start;                 // = ( num * 4 ) + 6; // Figure out where the next slot starts
    int looping;               // The number of times the loop repeats
    int j;
    int count = EEPROM.read(0);  // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT(a);        // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;                                                  // Decrement the counter by one
    EEPROM.write(0, num);                                   // Write the new count to the counter
    for (j = 0; j < looping; j++) {                         // Loop the card shift times
      EEPROM.write(start + j, EEPROM.read(start + 4 + j));  // Shift the array values to 4 places earlier in the EEPROM
    }
    for (int k = 0; k < 4; k++) {  // Shifting loop
      EEPROM.write(start + j + k, 0);
    }
    successRemove();
    // Serial.println(F("Succesfully removed ID record from EEPROM"));
  }
}

/***************************************************************************************************/

/**************************************** Read an ID from EEPROM************************************/

void readID(int number) {
  int start = (number * 4) + 2;              // Figure out starting position
  for (int i = 0; i < 4; i++) {              // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);  // Assign values read from EEPROM to array
  }
}

/***************************************************************************************************/

/****************************************Add ID to EEPROM*******************************************/

void writeID(byte a[]) {
  if (!findID(a)) {                   // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0);         // Get the numer of used spaces, position 0 stores the number of ID cards
    int start = (num * 4) + 6;        // Figure out where the next slot starts
    num++;                            // Increment the counter by one
    EEPROM.write(0, num);             // Write the new count to the counter
    for (int j = 0; j < 4; j++) {     // Loop 4 times
      EEPROM.write(start + j, a[j]);  // Write the array values to EEPROM in the right position
    }
    successWrite();
    // Serial.println(F("Succesfully added ID record to EEPROM"));
  } else {
    failedWrite();
    // Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}

/***************************************************************************************************/

/*****************************************Check Bytes***********************************************/

boolean checkTwo(byte a[], byte b[]) {
  if (a[0] != 0)              // Make sure there is something in the array first
    ismatch = true;                // Assume they match at first
  for (int k = 0; k < 4; k++) {  // Loop 4 times
    if (a[k] != b[k])            // IF a != b then set match = false, one fails, all fail
      ismatch = false;
  }
  if (ismatch) {    // Check to see if if match is still true
    return true;  // Return true
  } else {
    return false;  // Return false
  }
}

/***************************************************************************************************/

/*****************************************Find Slot*************************************************/

int findIDSLOT(byte find[]) {
  int count = EEPROM.read(0);          // Read the first Byte of EEPROM that
  for (int i = 1; i <= count; i++) {   // Loop once for each EEPROM entry
    readID(i);                         // Read an ID from EEPROM, it is stored in storedCard[4]
    if (checkTwo(find, storedCard)) {  // Check to see if the storedCard read from EEPROM is the same as the find[] ID card passed
      return i;                        // The slot number of the card
      //break;                           // Stop looking we found it
    }
  }
}

/***************************************************************************************************/

/*************************************** Read Serial Data *****************************************/
void readSerialData(char* name, char* role, int maxLength) {
  // Wait until there is data available on the serial port
  while (!Serial.available()) {
    delay(10);
  }

  // Read the entire string from the serial port
  String input = Serial.readString();

  // Split the input string into two parts using a comma as the delimiter
  int delimiterIndex = input.indexOf(',');
  if (delimiterIndex < 0) {
    //    Serial.println("Invalid input format");
    return;
  }

  String nameString = input.substring(0, delimiterIndex);
  String roleString = input.substring(delimiterIndex + 1);

  // Copy the name and role strings to the output buffers
  nameString.toCharArray(name, maxLength);
  roleString.toCharArray(role, maxLength);
}
/***************************************************************************************************/

/*************************************Write Data 2 Tag**********************************************/
void WriteData2Tag(byte nameBlock, byte roleBlock, char* name, char* role) {
  // Prepare key - all keys are set to FFFFFFFFFFFF at chip delivery from the factory.
  byte defaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  byte altKey[6] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
  MFRC522::MIFARE_Key currentKey;
  if (memcmp(mfrc522.uid.uidByte, altKey, 6) == 0) {
    memcpy(currentKey.keyByte, altKey, 6);
  } else {
    memcpy(currentKey.keyByte, defaultKey, 6);
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  MFRC522::StatusCode status;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, nameBlock, &currentKey, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    //    Serial.print(F("PCD_Authenticate() failed for name: "));
    //    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  Serial.println(F("PCD_Authenticate() success for name"));
  // Write name block
  byte nameBytes[16];
  stringToBytes(name, nameBytes, 16);
  status = mfrc522.MIFARE_Write(nameBlock, nameBytes, 16);
  if (status != MFRC522::STATUS_OK) {
    //    Serial.print(F("MIFARE_Write() failed for name: "));
    //    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else Serial.println(F("MIFARE_Write() success for name"));

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, roleBlock, &currentKey, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    //    Serial.print(F("PCD_Authenticate() failed for role: "));
    //    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  Serial.println(F("PCD_Authenticate() success for role"));
  // Write role block
  byte roleBytes[16];
  stringToBytes(role, roleBytes, 16);
  status = mfrc522.MIFARE_Write(roleBlock, roleBytes, 16);
  if (status != MFRC522::STATUS_OK) {
    //    Serial.print(F("MIFARE_Write() failed for role: "));
    //    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //  else
  //    Serial.println(F("MIFARE_Write() success for role"));
  mfrc522.PICC_HaltA();       // Stop reading
  mfrc522.PCD_StopCrypto1();  //Used to exit the PCD from its authenticated state.
  // Remember to call this function after communicating with an authenticated PICC - otherwise no new communications can start.
}
/***************************************************************************************************/

/************************************Read Data from Tag*********************************************/

void readDataFromTag(byte blockName, byte blockRole) {
  uname = "";
  urole = "";
  uid = "";

  byte defaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  byte altKey[6] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};

  byte readCard[4];    // Stores scanned ID read from RFID Module

  byte len = 18;
  byte buffer[18];

  // Try to authenticate with default key
  MFRC522::MIFARE_Key currentKey;
  memcpy(currentKey.keyByte, defaultKey, 6);
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockName, &currentKey, &(mfrc522.uid));

  for (int x = 0; x < 4; x++) {  //
    readCard[x] = mfrc522.uid.uidByte[x];
  }

  uid = convbyte2string(readCard);

  if (status != MFRC522::STATUS_OK) {
    // If default key authentication fails, try with alternate key
    memcpy(currentKey.keyByte, altKey, 6);
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockName, &currentKey, &(mfrc522.uid));
  }

  if (status != MFRC522::STATUS_OK) {
    return ;
  }

  // Read name from block
  status = mfrc522.MIFARE_Read(blockName, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    return ;
  }

  for (int i = 0; i < 16; i++) {
    uname += (char)buffer[i];
  }
  uname.trim();  // Remove all whitespace

  // Read role from block
  status = mfrc522.MIFARE_Read(blockRole, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    return ;
  }

  for (int i = 0; i < 16; i++) {
    urole += (char)buffer[i];
  }
  urole.trim();  // Remove all whitespace
  mfrc522.PICC_HaltA();  // Stop reading
  mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
}

/***************************************************************************************************/

/************************************ Convert String to Byte ***************************************/

void stringToBytes(char* str, byte* bytes, int length) {
  int strLength = strlen(str);
  int i;
  for (i = 0; i < strLength && i < length; i++) {
    bytes[i] = str[i];
  }
  for (; i < length; i++) {
    bytes[i] = ' ';
  }
}

/***************************************************************************************************/

/************************************** Capitalize letter ******************************************/

char Cap_Char(char c) {
  if (c >= 'a' && c <= 'z') {
    return (c - 32);
  }
  return c;
}

/***************************************************************************************************/

/************************************ Convert Byte to String ***************************************/

String convbyte2string(byte *src) {
  String str = "";
  for (byte i = 0; i < 4; i++) {
    if (src[i] < 0x10) {
      str += "0";
    }
    str += String(src[i], HEX);
  }
  for (byte i = 0; i < str.length(); i++) {
    str[i] = Cap_Char(str[i]);
  }
  return str;
}

/***************************************************************************************************/
