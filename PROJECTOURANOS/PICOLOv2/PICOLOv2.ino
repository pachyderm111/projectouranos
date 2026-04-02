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
#include <Adafruit_BNO055.h> 
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MS5611.h>   // Download from: https://github.com/jarzebski/Arduino-MS5611
#include "Thermistor.h"
#include "StemmaQtOLED.h" // Custom Library to control the StemmQT OLED screen

// Additional libraries here
#include <Servo.h>
#include <SparkFun_SGP40_Arduino_Library.h>


#include "Variables.h"

#define GPS_RUN_RATE  2.0 // Max GPS update speed in Hz. May not update at this speed.
#define DATA_RATE 1.0 // Max rate of data aqusition in Hz. Set to 100 or some huge number to remove the limiter
#define VERSION "2.0"

// Config variables.
//Geiger counter
#define GEIGER_PIN 9

bool usingM8N = true; // true for M8N, false for M9N

// File header. Edit to add columns for other sensors.

String header = "hh:mm:ss,T(min),T(s),T(ms),Counts/Cycle,Totalhits,Hits/Second,Methane(V),ServoOutput,Servo(V),ServoDeg,Raw_O3_Gas,O3_Gas(V),Raw_O3_Ref,O3_Ref(V),Raw_O3_Temp,O3_Temp(V),Hz,Batt (V),Fix Type,PVT,Sats,Date,Time,Lat,Lon,Alt(Ft),Alt(M),HorizAccuracy(MM),VertAccuracy(MM),VertVel(Ft/S),VertVel(M/S),ECEFstat,ECEFX(M),ECEFY(M),ECEFZ(M),NedVelNorth(M/S),NedVelEast(M/S),NedVelDown(M/S),GndSpd(M/S),Head(Deg),PDOP,ExtT(F),ExtT(C),ExtT(RK),IntT(F),IntT(C),IntT(RK),Pa,kPa,ATM,PSI,density(kg/m^3),MSTemp(C),MSTemp(F),MSTemp(RK),MS Alt SL(Ft), MS Alt SL(M),MS Alt Rel(Ft),MS Alt Rel(M),VertVel(ft/s),VertVel(m/s),Accel(x),Accel(y),Accel(z),Deg/S(x),Deg/S(y),Deg/S(z),Ori(x),Ori(y),Ori(z),Mag_T(x),Mag_T(y),Mag_T(z)z,Version:" + String(VERSION);

void setup() {

  systemSetup();
}

void setup1() {
  pinMode(GEIGER_PIN,INPUT);
}

void loop1(){
  if(digitalRead(GEIGER_PIN) == HIGH){
    hits++;
  }
  delayMicroseconds(350);
}

void loop() {

  if ((millis() - nowTimeMS) >= loopTime) {
    systemUpdate();

    // assembling the data srting;

    data = "";
    OLEDstr = "";
    
    // data.concat(HHMMSS);
    // data.concat(",");
    // data.concat(String(nowTimeMin));
    // data.concat(",");
    // data.concat(String(nowTimeS));
    // data.concat(",");
    // data.concat(String(nowTimeMS));
    // data.concat(",");
    //Geiger
    // data.concat(String(hitsPerCycle));
    // data.concat(",");
    // data.concat(String(totHits));
    // data.concat(",");
    // data.concat(String(hitsPerSec));
    // data.concat(",");
    //MQ9B
    // data.concat(String(mq9b_sensor_voltage));
    // data.concat(",");

    //SERVO
    data.concat(String(raw_servo_adc));
    data.concat(",");
    data.concat(String(servoFeedbackVolts));
    data.concat(",");
    data.concat(String(servoActualAngle));
    data.concat(",");

    //OZONE
    data.concat(String(raw_o3_vgas));
    data.concat(",");
    data.concat(String(o3_vgas_volts));
    data.concat(",");
    data.concat(String(raw_o3_vref));
    data.concat(",");
    data.concat(String(o3_vref_volts));
    data.concat(",");
    data.concat(String(raw_o3_vtemp));
    data.concat(",");
    data.concat(String(o3_vtemp_volts));
    data.concat(",");

    // data.concat(String(freq));
    // data.concat(",");
    // data.concat(String(Volt));
    // data.concat(",");
    // data.concat(fixTypeGPS);
    // data.concat(",");
    // data.concat(String(pvtStatus));
    // data.concat(",");
    // data.concat(String(SIV));
    // data.concat(",");
    // data.concat(String(gpsMonth));
    // data.concat("/");
    // data.concat(String(gpsDay));
    // data.concat("/");
    // data.concat(String(gpsYear));
    // data.concat(",");
    // data.concat(String(gpsHour));
    // data.concat(":");
    // data.concat(String(gpsMinute));
    // data.concat(":");
    // data.concat(String(gpsSecond));
    // data.concat(".");

    if (gpsMillisecond < 10) {
      // data.concat("00");
      // data.concat(String(gpsMillisecond));
      // data.concat(",");
    }
    else if (gpsMillisecond < 100) {
      // data.concat("0");
      // data.concat(String(gpsMillisecond));
      // data.concat(",");
    }
    else{
      // data.concat(String(gpsMillisecond)); 
      // data.concat(",");
    }

    char paddedNumber[8]; // Buffer to hold the padded number (7 digits + null terminator)
    // data.concat(String(gpsLatInt));
    // data.concat(".");
    // Format the number with padded zeros using sprintf()
    sprintf(paddedNumber, "%07ld", gpsLatDec);
    data.concat(String(paddedNumber)); // Pad the number with zeros up to 7 digits
    data.concat(",");
    OLEDstr.concat("Lat: " + String(gpsLatInt) + "." + String(paddedNumber) + "\n");

    data.concat(String(gpsLonInt)); 
    data.concat(".");
    // Format the number with padded zeros using sprintf()
    sprintf(paddedNumber, "%07ld", gpsLonDec);
    data.concat(String(paddedNumber)); // Pad the number with zeros up to 7 digits
    data.concat(",");
    OLEDstr.concat("Lon: " + String(gpsLonInt) + "." + String(paddedNumber) + "\n");

    // data.concat(String(gpsAltFt));
    // data.concat(",");
    // OLEDstr.concat("GPSft: " + String(gpsAltFt) + "\n");
    // data.concat(String(gpsAltM));
    // data.concat(",");
    // data.concat(String(gpsHorizAcc));
    // data.concat(",");
    // data.concat(String(gpsVertAcc));
    // data.concat(",");
    // data.concat(String(gpsVertVelFt));
    // data.concat(",");
    // data.concat(String(gpsVertVelM));
    // data.concat(",");
    // data.concat(String(ecefStatus));
    // data.concat(",");
    // data.concat(String(ecefX));
    // data.concat(",");
    // data.concat(String(ecefY)); 
    // data.concat(",");
    // data.concat(String(ecefZ));
    // data.concat(","); 
    // data.concat(String(velocityNED[0]));
    // data.concat(",");
    // data.concat(String(velocityNED[1])); 
    // data.concat(",");
    // data.concat(String(velocityNED[2]));
    // data.concat(","); 
    // data.concat(String(gpsGndSpeed));
    // data.concat(",");
    // data.concat(String(gpsHeading));
    // data.concat(",");
    // data.concat(String(gpsPDOP));
    // data.concat(",");
    // data.concat(String(outTempF));
    // data.concat(",");
    // data.concat(String(outTempC));
    // data.concat(",");
    // data.concat(String(outTempRK));
    // data.concat(",");
    // data.concat(String(inTempF));
    // data.concat(",");
    // data.concat(String(inTempC));
    // data.concat(",");
    // data.concat(String(inTempRK));
    // data.concat(",");
    // data.concat(String(pressPa));
    // data.concat(",");
    // data.concat(String(presskPa));
    // data.concat(",");
    // data.concat(String(pressATM));
    // data.concat(",");
    // data.concat(String(pressPSI));
    // data.concat(",");
    // data.concat(String(density));
    // data.concat(",");
    // data.concat(String(MStempC));
    // data.concat(",");
    // data.concat(String(MStempF));
    // data.concat(",");
    // data.concat(String(MStempRK));
    // data.concat(",");
    // data.concat(String(absAltFt));
    // data.concat(",");
    // OLEDstr.concat("MSft: " + String(absAltFt) + "\n");
    // data.concat(String(absAltM));
    // data.concat(",");
    // data.concat(String(relAltFt));
    // data.concat(",");
    // data.concat(String(relAltM));
    // data.concat(",");
    // data.concat(String(vertVelFt));
    // data.concat(",");
    // data.concat(String(vertVelM));
    // data.concat(",");
    // data.concat(String(accelerometer[0]));
    // data.concat(",");
    // data.concat(String(accelerometer[1]));
    // data.concat(",");
    // data.concat(String(accelerometer[2]));
    // data.concat(",");
    // data.concat(String(gyroscope[0]));
    // data.concat(",");
    // data.concat(String(gyroscope[1]));
    // data.concat(",");
    // data.concat(String(gyroscope[2]));
    // data.concat(",");
    // data.concat(String(orientation[2]));
    // data.concat(",");
    // data.concat(String(orientation[1]));
    // data.concat(",");
    // data.concat(String(orientation[0]));
    // data.concat(",");
    // data.concat(String(magnetometer[0]));
    // data.concat(",");
    // data.concat(String(magnetometer[1]));
    // data.concat(",");
    // data.concat(String(magnetometer[2]));
    // data.concat(",");

    /*
      data form additional sensors
    */

    Serial.println(data);
    SDstatus = logData(data, dataFilename);

    OLEDstr.concat("Sats: " + String(SIV) + "  Hz: " + String(freq) + "\n");
    OLEDstr.concat("Ext: " + String(outTempF) + " F\nInt: " + String(inTempF) + " F\nMS: " + String(MStempF) + " F");
    printOLED(OLEDstr);

    prevTime = nowTimeMS;
  }
}
