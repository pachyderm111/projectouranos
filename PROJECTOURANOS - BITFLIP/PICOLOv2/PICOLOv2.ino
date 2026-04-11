/*
_____________________________________________________________
Code for the PICOLO Flight Computer
Code by: Radhakrishna Vojjala
Date of last modification: 16 Mar 2026
Version 2.0
_____________________________________________________________

*/

#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MS5611.h>   // Download from: https://github.com/jarzebski/Arduino-MS5611

// Additional libraries here
#include "Variables.h"

#define DATA_RATE 1.0 // Max rate of data aqusition in Hz. Set to 100 or some huge number to remove the limiter
#define VERSION "2.0"

// Config variables.
//Geiger counter

bool usingM8N = true; // true for M8N, false for M9N

// File header. Edit to add columns for other sensors.

String header = "hh:mm:ss,T(min),T(s),T(ms),Counts/Cycle,Totalhits,Hits/Second,Methane(V),ServoOutput,Servo(V),ServoDeg,VOC_Index,Raw_O3_Gas,O3_Gas(V),Raw_O3_Ref,O3_Ref(V),Raw_O3_Temp,O3_Temp(V),Hz,Batt (V),Fix Type,PVT,Sats,Date,Time,Lat,Lon,Alt(Ft),Alt(M),HorizAccuracy(MM),VertAccuracy(MM),VertVel(Ft/S),VertVel(M/S),ECEFstat,ECEFX(M),ECEFY(M),ECEFZ(M),NedVelNorth(M/S),NedVelEast(M/S),NedVelDown(M/S),GndSpd(M/S),Head(Deg),PDOP,ExtT(F),ExtT(C),ExtT(RK),IntT(F),IntT(C),IntT(RK),Pa,kPa,ATM,PSI,density(kg/m^3),MSTemp(C),MSTemp(F),MSTemp(RK),MS Alt SL(Ft), MS Alt SL(M),MS Alt Rel(Ft),MS Alt Rel(M),VertVel(ft/s),VertVel(m/s),Accel(x),Accel(y),Accel(z),Deg/S(x),Deg/S(y),Deg/S(z),Ori(x),Ori(y),Ori(z),Mag_T(x),Mag_T(y),Mag_T(z)z,Version:" + String(VERSION);

void setup() {

  systemSetup();
}

void loop() {
  // 1. Scrub memory for flips
  for (int i = 0; i < MONITOR_SIZE; i++) {
    if (goldArray[i] != GOLD_PATTERN) {
      
      // BIT FLIP DETECTED!
      uint8_t flippedValue = goldArray[i];
      totalFlips++;
      
      // Log the event immediately
      String errorLog = "FLIP," + String(millis()) + "," + String(i) + "," + String(flippedValue, HEX);
      SDstatus = logData(errorLog, dataFilename);
      
      // Repair the bit so we can catch the next one
      goldArray[i] = GOLD_PATTERN;
      
      digitalWrite(ERR_LED_PIN, HIGH); // Visual alert
    }
  }

  // 2. Periodic Pressure/Health Log (Slow down the data rate)
  if ((millis() - nowTimeMS) >= 5000) { // Log pressure every 5 seconds
    nowTimeMS = millis();
    MSupdate(); // Keep your pressure update

    nowTimeS = nowTimeMS / 1000.0;
    nowTimeMin = nowTimeS / 60.0;
    
    data = String(nowTimeS) + "," + String(pressPa) + "," + String(totalFlips);
    Serial.println(data);
    SDstatus = logData(data, dataFilename);
    
    digitalWrite(LOOP_LED_PIN, !digitalRead(LOOP_LED_PIN)); // Heartbeat
  }

}
