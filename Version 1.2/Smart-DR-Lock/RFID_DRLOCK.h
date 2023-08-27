/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RFID_DRLOCK.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ababdelo <ababdelo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 10:07:01 by ababdelo          #+#    #+#             */
/*   Updated: 2023/08/27 10:07:03 by ababdelo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RFID_DRLOCK_H
#define RFID_DRLOCK_H

/*************************************Including needed Libraries*************************************/

#include <EEPROM.h>             // We are going to read and write PICC's UIDs from/to EEPROM
#include <Arduino.h>
#include <rdm6300.h>           // RC522 Module uses SPI protocol
#include <LiquidCrystal_I2C.h>  // Library for LCD_I2C

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

// Set Led Pins
#define REDLED 6
#define GREENLED 5
#define BLUELED 3

#define RDM6300_RX_PIN 7
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

Rdm6300 rdm6300;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // to know the address of your i2c devices just use "I2c_scan" sketch example from the Libray "Wire" :)

/***************************************************************************************************/

/************************************Declaring used Variables***************************************/

String uid = "";

boolean ismatch = false;        // initialize card match to false
boolean programMode = false;  // initialize programming mode to false
boolean replaceMaster = false;

int counter;
int successRead;  // boolean Variable to keep if we have Successful Read from Reader
unsigned int cntr = 9;  // Erasing Counter

uint32_t storedCard[10];  // Stores an ID read from EEPROM
uint32_t *readCard;    // Stores scanned ID read from RFID Module
uint32_t masterCard[10];  // Stores master card's ID read from EEPROM

/***************************************************************************************************/

/************************************Declaring used Functions***************************************/

// Declare FT_UID Functions :
int getID();
int findIDSLOT(uint32_t find[]);

boolean isMaster(uint32_t test[]);
boolean findID(uint32_t find[]);
boolean checkTwo(uint32_t a[], uint32_t b[]);

void deleteID(uint32_t a[]);
void readID(int number);
void writeID(uint32_t a[]);

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
