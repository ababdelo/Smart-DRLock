/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FT_UTILS.ino                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ababdelo <ababdelo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 10:07:06 by ababdelo          #+#    #+#             */
/*   Updated: 2023/08/27 10:07:08 by ababdelo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RFID_DRLOCK.h"

/************************************** Count the uid lenght ***************************************/

uint32_t digit_count(uint32_t nbr) {
  int count = 0;
  if (nbr == 0)
    return 1;
  while (nbr) {
    count++;
    nbr = nbr / 10;
  }
  return count;
}
/***************************************************************************************************/

/******************************************** GetID ************************************************/

int getID() {
  if (rdm6300.get_new_tag_id()) {
    uint32_t newTagId = rdm6300.get_tag_id();
    int nbrcntr = digit_count(newTagId);
    uid = String(newTagId, HEX);
    uid.toUpperCase();
    // Serial.print("UID: ");
    // Serial.println(newTagId);

    readCard = new uint32_t[nbrcntr];
    for (int i = nbrcntr - 1; i >= 0; i--) {
      readCard[i] = newTagId % 10;
      newTagId /= 10;
    }
    return (1);
  }
  return (0);
}


/***************************************************************************************************/

/******************************** Check readCard IF is masterCard***********************************/

// Check to see if the ID passed is the master programing card
boolean isMaster(uint32_t test[]) {
  if (checkTwo(test, masterCard))
    return true;
  else
    return false;
}

/***************************************************************************************************/

/*****************************************Find ID From EEPROM***************************************/

boolean findID(uint32_t find[]) {
  int count = EEPROM.read(0);          // Read the first Byte of EEPROM that
  for (int i = 1; i <= count; i++) {   // Loop once for each EEPROM entry
    readID(i);                         // Read an ID from EEPROM, it is stored in storedCard[4]
    if (checkTwo(find, storedCard)) {  // Check to see if the storedCard read from EEPROM
      return true;
      //break;  // Stop looking we found it
    } else {
    }  // If not, return false
  }
  return false;
}

/***************************************************************************************************/

/*****************************************Remove ID from EEPROM*************************************/

void deleteID(uint32_t a[]) {
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

void writeID(uint32_t a[]) {
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

boolean checkTwo(uint32_t a[], uint32_t b[]) {
  if (a[0] != 0)                 // Make sure there is something in the array first
    ismatch = true;              // Assume they match at first
  for (int k = 0; k < 4; k++) {  // Loop 4 times
    if (a[k] != b[k])            // IF a != b then set match = false, one fails, all fail
      ismatch = false;
  }
  if (ismatch) {  // Check to see if if match is still true
    return true;  // Return true
  } else {
    return false;  // Return false
  }
}

/***************************************************************************************************/

/*****************************************Find Slot*************************************************/

int findIDSLOT(uint32_t find[]) {
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

/******************************************Access Granted*******************************************/
void granted(int setDelay, String permission) {
  if (permission == "YES") {
    analogWrite(BLUELED, LED_OFF);  // Turn off blue LED
    analogWrite(REDLED, LED_OFF);   // Turn off red LED
    analogWrite(GREENLED, LED_ON);  // Turn on green LED
  } else {
    analogWrite(BLUELED, LED_ON);    // Turn off blue LED
    analogWrite(REDLED, LED_OFF);    // Turn off ßred LED
    analogWrite(GREENLED, LED_OFF);  // Turn on green LED
  }

  digitalWrite(RELAYDOOR, LOW);   // Unlock door!
  delay(setDelay);                // Hold door lock open for given seconds
  digitalWrite(RELAYDOOR, HIGH);  // Relock door
}
/***************************************************************************************************/

/*****************************************Access Denied*********************************************/
void denied() {
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is off
  analogWrite(BLUELED, LED_OFF);   // Make sure blue LED is off
  analogWrite(REDLED, LED_ON);     // Turn on red LED
  delay(1000);
}
/***************************************************************************************************/

/*************************************Cycle Leds (Program Mode)*************************************/
void cycleLeds() {
  analogWrite(REDLED, LED_OFF);   // Make sure red LED is off
  analogWrite(GREENLED, LED_ON);  // Make sure green LED is on
  analogWrite(BLUELED, LED_OFF);  // Make sure blue LED is off
  delay(200);
  analogWrite(REDLED, LED_OFF);    // Make sure red LED is off
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is off
  analogWrite(BLUELED, LED_ON);    // Make sure blue LED is on
  delay(200);
  analogWrite(REDLED, LED_ON);     // Make sure red LED is on
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is off
  analogWrite(BLUELED, LED_OFF);   // Make sure blue LED is off
  delay(200);
}
/***************************************************************************************************/

/***************************************Normal Mode Led*********************************************/
void normalModeOn() {
  analogWrite(BLUELED, LED_ON);    // Blue LED ON and ready to read card
  analogWrite(REDLED, LED_OFF);    // Make sure Red LED is off
  analogWrite(GREENLED, LED_OFF);  // Make sure Green LED is off
  digitalWrite(RELAYDOOR, HIGH);   // Make sure Door is Locked
}
/***************************************************************************************************/

/**************************************Success Write to EEPROM***************************************/
// Flashes the green LED 3 times to indicate a successful write to EEPROM
void successWrite() {
  analogWrite(BLUELED, LED_OFF);   // Make sure blue LED is off
  analogWrite(REDLED, LED_OFF);    // Make sure red LED is off
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is on
  delay(200);
  analogWrite(GREENLED, LED_ON);  // Make sure green LED is on
  delay(200);
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is off
  delay(200);
  analogWrite(GREENLED, LED_ON);  // Make sure green LED is on
  delay(200);
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is off
  delay(200);
  analogWrite(GREENLED, LED_ON);  // Make sure green LED is on
  delay(200);
}
/***************************************************************************************************/

/*************************************Failed Write to EEPROM****************************************/
// Flashes the red LED 3 times to indicate a failed write to EEPROM
void failedWrite() {
  analogWrite(BLUELED, LED_OFF);   // Make sure blue LED is off
  analogWrite(REDLED, LED_OFF);    // Make sure red LED is off
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is off
  delay(200);
  for (int x = 0; x < 3; x++) {
    digitalWrite(REDLED, LED_ON);  // Make sure blue LED is on
    delay(200);
    digitalWrite(REDLED, LED_OFF);  // Make sure red LED is off
    delay(200);
  }
  analogWrite(REDLED, LED_OFF);  // just making sure the Red LED is OFF
  analogWrite(BLUELED, LED_ON);  // Make sure blue LED is on
}
/***************************************************************************************************/

/*********************************Success Remove UID From EEPROM************************************/
// Flashes the blue LED 3 times to indicate a success delete to EEPROM
void successRemove() {
  analogWrite(BLUELED, LED_OFF);   // Make sure blue LED is off
  analogWrite(REDLED, LED_OFF);    // Make sure red LED is off
  analogWrite(GREENLED, LED_OFF);  // Make sure green LED is off
  delay(200);
  analogWrite(REDLED, LED_ON);  // Make sure red LED is on
  delay(200);
  analogWrite(REDLED, LED_OFF);  // Make sure red LED is off
  delay(200);
  analogWrite(REDLED, LED_ON);  // Make sure red LED is on
  delay(200);
  analogWrite(REDLED, LED_OFF);  // Make sure red LED is off
  delay(200);
  analogWrite(REDLED, LED_ON);  // Make sure red LED is on
  delay(200);
}
/***************************************************************************************************/

/******************************************Print 2 LCD**********************************************/
void print2lcd(String str1, int c1, String str2, int c2) {
  lcd.clear();
  lcd.setCursor(c1, 0);
  lcd.print(str1);
  if (str2.length() > 0) {
    lcd.setCursor(c2, 1);
    lcd.print(str2);
  }
}
/***************************************************************************************************/
