/*
_____________________________________________________________
Code for the entire system of the Pi Pico W.
Code by: Radhakrishna Vojjala
Date of last modification: 16 Mar 2026
_____________________________________________________________
This file contains the Setup and Update functions for the entire system. The code is meant to be called in setup() and loop().
*/

// System wide setup function.

void systemSetup() {

  Serial.begin(SERIAL_BAUD);

// Give you time to open the monitor
  while (!Serial && millis() < 3000); 

  Wire.begin(); 
  Wire1.setSCL(I2C_1_SCL);
  Wire1.setSDA(I2C_1_SDA);
  Wire1.begin(); 

  Serial.println("Initialising....");

  // Arm the memory trap
  for (int i = 0; i < MONITOR_SIZE; i++) {
    goldArray[i] = GOLD_PATTERN;
  }
  Serial.println("Memory Trap Armed.");

  Serial.println("Initialising....");

  if (MSsetup()){
    Serial.println("MS5611 Online!");
  }
  else {
    Serial.println("MS5611 Offline! Check wiring.");
    digitalWrite(ERR_LED_PIN, HIGH);
    delay(500);
    digitalWrite(ERR_LED_PIN, LOW);
    error = true;
  }

  SDsetup(dataFilename, dataFileN1, dataFileN2);
  logData(header, dataFilename);

  loopTime = 1000 / DATA_RATE;

  Serial.println("Setup Finished");
}

  /*
     Add setup code for additional sensors here
  */
