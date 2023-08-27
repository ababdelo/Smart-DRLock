
#include"RFID_DRLOCK.h"

/****************************************Setup Function*********************************************/
void setup() {
  digitalWrite(RST, HIGH);        // first thing to do is to set rst pin to HIGH to make sure arduino won't reset itself from the start
  digitalWrite(RELAYDOOR, HIGH);  // and Make sure that the door is locked

  //Arduino Pin Configuration
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);

  pinMode(BTNERASE, INPUT_PULLUP);  // Enable pin's pull up resistor
  pinMode(BTNDOOR, INPUT_PULLUP);   // Enable pin's pull up resistor
  pinMode(RELAYDOOR, OUTPUT);       //Be careful how relay circuit behave on while resetting or power-cycling your Arduino
  pinMode(RST, OUTPUT);             // Pin that'ill reset arduino after an erasing of memory

  // digitalWrite(RELAYLAMP, LOW);     // Make sure lamp is turned off
  analogWrite(REDLED, LED_OFF);    // Make sure led is off
  analogWrite(GREENLED, LED_OFF);  // Make sure led is off
  analogWrite(BLUELED, LED_OFF);   // Make sure led is off

  //Protocol Configuration
  Serial.begin(9600);  // Initialize serial communications with PC
  SPI.begin();         // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();  // Initialize MFRC522 Hardware

  //If you set Antenna Gain to Max it will increase reading distance but it's preferred not to use it
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  lcd.init();  //Initialize LCD_I2C

  lcd.begin(16, 2);                     // Initialize the interface to LCD_I2C screen
  lcd.backlight();                      // Turn on LCD_I2C Backlight
  // Serial.println(F("Access Control"));  // For debugging purposes
  print2lcd(" Access Control", 0, " Scan Your Tag!", 0);
  // dsplreaderdetails();  // Show details of PCD - MFRC522 Card Reader details

  // Check if master card defined, if not let user choose a master card
  // This also useful to just redefine Master Card
  // You can keep other EEPROM records just write other than 42 to EEPROM address 1
  // EEPROM address 1 should hold magical number which is '42'
  if (EEPROM.read(1) != 42) {
    print2lcd(" Define  Master ", 0, "", 0);
    // Serial.println(F("No Master Card Defined"));
    // Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
      successRead = getID();          // sets successRead to 1 when we get read from reader otherwise 0
      digitalWrite(BLUELED, LED_ON);  // Visualize Master Card need to be defined
      delay(200);
      digitalWrite(BLUELED, LED_OFF);
      delay(200);
    } while (!successRead);              // Program will not go further while you did not got a successful read
    for (int j = 0; j < 4; j++) {        // Loop 4 times
      EEPROM.write(2 + j, readCard[j]);  // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1, 42);  // Write to EEPROM we defined Master Card.
    // Serial.println(F("Master Card Defined"));
    print2lcd(" Master Defined ", 0, "", 0);
    Serial.print("Master Defined !");
    Serial.print("\n");
    delay(1000);
  }
  // Serial.println(F("-------------------"));
  // Serial.println(F("Master Card's UID"));
  for (int i = 0; i < 4; i++) {          // Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i);  // Write it to masterCard
    //    Serial.print(masterCard[i], HEX);
  }
  // Serial.println();
  // Serial.println(F("-------------------"));
  // Serial.println(F("Everything is Ready"));
  // Serial.println(F("Waiting PICCs to be scanned"));
  print2lcd(" Access Control", 0, " Scan Your Tag!", 0);
  cycleLeds();  // Everything ready lets give user some feedback by cycling leds
}
/***************************************************************************************************/

/*****************************************Loop Function*********************************************/
void loop() {
  do {
    successRead = getID();  // sets successRead to 1 when we get read from reader otherwise 0

    if (!programMode) {
      if (digitalRead(BTNDOOR) == LOW) {
        granted(2000, "NO");
      }
    }

    //If wipe Button is pressed 'til 10  sec it wipes EEPROM
    while (digitalRead(BTNERASE) == LOW) {

      if (programMode) {
        if (counter > 0) {
          // Serial.println(F("This will remove all records from the EEPROM !"));
          print2lcd("Erasing EEPROM", 1, ("Cancel before: " + String(cntr)), 0);
          // Serial.print(F("Cancel before : "));
          // Serial.println(cntr);
          digitalWrite(REDLED, LED_ON);    // Red Led stays on to inform user we are going to wipe
          digitalWrite(BLUELED, LED_OFF);  // Red Led stays on to inform user we are going to wipe
          delay(1000);
          cntr--;
          if (cntr == 0) {  // If button still be pressed, wipe EEPROM
            // Serial.println(F("Starting Erasing EEPROM"));
            for (int x = 0; x < EEPROM.length(); x = x + 1) {  //Loop end of EEPROM address
              if (EEPROM.read(x) == 0) {                       //If EEPROM address 0
                // do nothing, already clear, go to the next address in order to save time and reduce writes to EEPROM
              } else {
                EEPROM.write(x, 0);  // if not write 0 to clear, it takes 3.3mS
              }
            }
            // Serial.println(F("EEPROM Successfully Erased"));
            print2lcd("EEPROM  Erased", 1, "Successfully", 2);
            Serial.print("| Role --> " + urole + " |      | Action --> Erased EEPROM |");
            Serial.print("\n");
            analogWrite(REDLED, LED_OFF);  // visualize successful wipe
            delay(200);
            analogWrite(REDLED, LED_ON);
            delay(200);
            analogWrite(REDLED, LED_OFF);
            delay(200);
            analogWrite(REDLED, LED_ON);
            delay(200);
            analogWrite(REDLED, LED_OFF);

            digitalWrite(RST, LOW);  // Reseting arduino to apply the wiping
            delay(50);
            digitalWrite(RST, HIGH);
          }
        }
        if (digitalRead(BTNERASE) != LOW) {
          // Serial.println(F("Erasing Cancelled"));
          print2lcd("    Erasing     ", 0, "Cancelled", 3);
          analogWrite(REDLED, LED_OFF);
          cntr = 9;
          delay(1000);
          print2lcd("Entered", 4, "Program Mode", 2);
        }
        counter--;
      } else {
        break;
      }
    }
    counter = cntr;
    if (programMode) {
      cycleLeds();  // Program Mode cycles through RGB waiting to read a new card
    } else {
      normalModeOn();  // Normal mode, blue Power LED is on, all others are off
    }
  } while (!successRead);  //the program will not go further while you not get a successful read

  if (programMode) {
    if (isMaster(readCard)) {  //If master card scanned again exit program mode
      // Serial.println(F("Master Card Scanned"));
      print2lcd("Exited", 5, "Program Mode", 2);
      // Serial.println(F("Exiting Program Mode"));
      // Serial.println(F("-----------------------------"));
      programMode = false;
      delay(1000);
      print2lcd(" Access Control", 0, " Scan Your Tag!", 0);
      return;
    } else {
      if (findID(readCard)) {  // If scanned card is known delete it
        print2lcd("Removed Existing", 0, " ID from EEPROM ", 0);
        Serial.print("| Role --> Master |      | Action --> Removed Existing ID |");
        Serial.print("\n");
        // Serial.println(F("Existing UID, removing..."));
        deleteID(readCard);
        // Serial.println("-----------------------------");
        // Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      } else {  // If scanned card is not known add it
        print2lcd(" Added New ID ", 1, "to  EEPROM", 3);
        Serial.print("| Role --> Master |      | Action --> Added New ID |");
        Serial.print("\n");
        // Serial.println(F("New UID, adding..."));
        writeID(readCard);
        // Serial.println(F("-----------------------------"));
        // Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      }
    }
    delay(1000);
    print2lcd("Entered", 4, "Program Mode", 2);
  } else {
    if (isMaster(readCard)) {  // If scanned card's ID matches Master Card's ID enter program mode
      programMode = true;
      // Serial.println(F("Hello Master - Entering Program Mode"));
      print2lcd("Entered", 4, "Program Mode", 2);
      int count = EEPROM.read(0);       // Read the first Byte of EEPROM that
      // Serial.print(F("Known UID : "));  // stores the number of ID's in EEPROM
      // Serial.print(count);
      // Serial.print(F(" on EEPROM"));
      // Serial.println();
      // Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM\nOrScan Master Card again to Exit Owner Mode"));
      // Serial.println();
      // Serial.println(F("-----------------------------"));
    } else {
      if (findID(readCard)) {  // If not, see if the card is in the EEPROM
        // Serial.println(F("Access Granted"));
        //String Name[] = {"Hamza", "Mohammed", "Fatima Zahra", "Hossain", "Abderrahmane"}; // before i added the "readdatafromtag" function i wanted to test displaying the tag's username to lcd
        //int rand = (int)random(0, 5);                                                //. it's just 4 testing how could we get which col in lcd to start printing the tag's username .
        //print2lcd("Welcome Back", 2, Name[rand], ((16 - Name[rand].length()) / 2));  // c2 = ( 16 - Name[ran].lentgh()) / 2
        print2lcd("Welcome Back", 2, uname, ((16 - uname.length()) / 2));  // c2 = ( 16 - name.lentgh()) / 2 .
        Serial.print("| Name : " + uname + " |" + "     | Role --> " + urole + " |      | Action --> Entered the Club |");
        Serial.print("\n");
        granted(2000, "YES");                                                     // Open the door lock for 2000 ms
        print2lcd(" Access Control", 0, " Scan Your Tag!", 0);
      } else {  // If not, show that the ID was not valid
        // Serial.println(F("Access Denied"));
        print2lcd(" Access  Denied ", 0, "Unknown :)", 3);
        Serial.print("| UID --> " + uid + " |      | Action --> Attempted entering the Club |");
        Serial.print("\n");
        denied();
        print2lcd(" Access Control", 0, " Scan Your Tag!", 0);
      }
    }
  }
}
/***************************************************************************************************/
