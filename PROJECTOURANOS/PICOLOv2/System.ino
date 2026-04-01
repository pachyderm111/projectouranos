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

  Wire.begin(); // default I2C clock
  Wire1.setSCL(I2C_1_SCL);
  Wire1.setSDA(I2C_1_SDA);
  Wire1.begin(); // default I2C clock

  beginOLED(); 

  Serial.println("Initialising....");
  printOLED("Initialising....", true);
  pinMode(ERR_LED_PIN, OUTPUT); // red LED
  pinMode(LOOP_LED_PIN, OUTPUT); // yellow LED
  pinMode(LOCK_LED_PIN, OUTPUT); // blue LED

  // LED test
  digitalWrite(ERR_LED_PIN, HIGH);
  delay(100);
  digitalWrite(LOOP_LED_PIN, HIGH);
  delay(100);
  digitalWrite(LOCK_LED_PIN, HIGH);
  delay(100);
  digitalWrite(ERR_LED_PIN, LOW);
  delay(100);
  digitalWrite(LOOP_LED_PIN, LOW);
  delay(100);
  digitalWrite(LOCK_LED_PIN, LOW);

  if (bno.begin()){
    Serial.println("BNO Online!");
    printOLED("BNO Online!", true);
  }
  else {
    Serial.println("BNO Offline! Check wiring.");
    printOLED("BNO Offline!\nCheck wiring.", true);
    digitalWrite(ERR_LED_PIN, HIGH);
    delay(500);
    digitalWrite(ERR_LED_PIN, LOW);
    error = true;
  }


  if (MSsetup()){
    Serial.println("MS5611 Online!");
    printOLED("MS5611 Online!", true);
  }
  else {
    Serial.println("MS5611 Offline! Check wiring.");
    printOLED("MS5611 Offline!\nCheck wiring.", true);
    digitalWrite(ERR_LED_PIN, HIGH);
    delay(500);
    digitalWrite(ERR_LED_PIN, LOW);
    error = true;
  }

  inTher.begin(10);
  outTher.begin(10);
  inTher.update();
  outTher.update();
  inStatus = inTher.updateStatus();
  outStatus = outTher.updateStatus();

  if (inStatus){
    Serial.println("Internal Thermistor Connected!");
    printOLED("Internal Thermistor\nConnected!", true);
  }
  else {
    Serial.println("Internal Thermistor Offline. Check wiring.");
    printOLED("Internal Thermistor\nOffline.\nCheck wiring.", true);
    digitalWrite(ERR_LED_PIN, HIGH);
    delay(500);
    digitalWrite(ERR_LED_PIN, LOW);
    error = true;
  }
  
  if (outStatus){
    Serial.println("External Thermistor Connected!");
    printOLED("External Thermistor\nConnected!", true);
  }
  else {
    Serial.println("External Thermistor Offline. Check wiring.");
    printOLED("External Thermistor\nOffline.\nCheck wiring.", true);
    digitalWrite(ERR_LED_PIN, HIGH);
    delay(500);
    digitalWrite(ERR_LED_PIN, LOW);
    error = true;
  }

  GPSsetup();

  /*
     Add setup code for additional sensors here
  */

    //MQ-9B
  pinMode(MQ9B_PIN, INPUT);

    //SERVO
    pinMode(SERVO_FEEDBACK_PIN, INPUT);

  flightServo.attach(SERVO_PIN);
  flightServo.write(0); // Set to 0 degrees
  delay(1000);
  Serial.println("Servo Attached");
  flightServo.write(90); // Set to 90 degrees
  delay(1000);
  flightServo.write(0); // Set to 0 degrees
 

//SGP40
  if (sgp40.begin()) {
    Serial.println("SGP40 Online!");
    printOLED("SGP40 Online!", true);
  } else {
    Serial.println("SGP40 Offline! Check wiring.");
    printOLED("SGP40 Offline!\nCheck wiring.", true);
    digitalWrite(ERR_LED_PIN, HIGH);
    delay(500);
    digitalWrite(ERR_LED_PIN, LOW);
    error = true;
  }


  SDsetup(dataFilename, dataFileN1, dataFileN2);
  logData(header, dataFilename);

  loopTime = 1000 / DATA_RATE;

  Serial.println("Setup Finished");
  printOLED("Setup Finished", true);
  Serial.println(header);

  if (!SDstatus){
    error = true;
  }
  if (error){
    digitalWrite(ERR_LED_PIN, HIGH);
  }
}

// System wide update function. Updates all sensors.

void systemUpdate(){

  // updating timers

  nowTimeMS = millis();
  digitalWrite(LOOP_LED_PIN, LOW);
  nowTimeS = nowTimeMS / 1000.0;
  nowTimeMin = nowTimeS / 60;
  freq = 1.0/((nowTimeMS-prevTime)/1000.0);
  HHMMSS = timeToHhmmss(nowTimeMS);

  Volt = analogRead(3)*3; // battery voltage

  // updating sensors

  GPSupdate();
  MSupdate();
  BNOupdate();

  inTher.update();
  inTempF = inTher.getTempF();
  inTempC = inTher.getTempC();
  inTempRK = ((inTempC + 273.15) / 180.0) * PI;
  outTher.update();
  outTempF = outTher.getTempF();
  outTempC = outTher.getTempC();
  outTempRK = ((outTempC + 273.15) / 180.0) * PI;

  /*
    Addtional sensor update code here.
  */

  //Geiger
  prevHits = totHits;
  totHits = hits;
  hitsPerCycle = totHits - prevHits;
  hitsPerSec = (hitsPerCycle*1000.0)/(millis() - prevCountTime);
  prevCountTime = millis();

  digitalWrite(LOOP_LED_PIN, HIGH);

  //MQ-9B
  mq9b_raw = analogRead(MQ9B_PIN);
  //convert voltage (3.3v input pin)
  mq9b_pico_voltage = mq9b_raw * (3.3 / 1023.0);
  //convert back (5v) -- voltage divider
  mq9b_sensor_voltage = mq9b_pico_voltage * 1.5;

  //Update SGP40
  //VOC Index from 0 to 500 (100 is standard clean air)
  voc_index = sgp40.getVOCindex();

  //SERVO
  raw_servo_adc = analogRead(SERVO_FEEDBACK_PIN); //read voltage
  servoFeedbackVolts = raw_servo_adc * (3.3 / 1023.0); // reading to voltage
  //calibration
  float minVolts = 0.5; // voltage at 0
  float maxVolts = 2.5; // voltage at 180
  
  // calculate the physical angle
  servoFeedbackVolts = constrain(servoFeedbackVolts, minVolts, maxVolts);
  servoActualAngle = (servoFeedbackVolts - minVolts) * (180.0 / (maxVolts - minVolts));


  // rotate
  if (nowTimeMS - prevServoTime >= servoInterval) {
    prevServoTime = nowTimeMS; // reset timer
    
    // toggle position
    servoAt90 = !servoAt90; 
    
    if (servoAt90) {
      flightServo.write(90);
    } else {
      flightServo.write(0);
    }
  }

  

}



// Function to convert timer to HHMMSS format 

String timeToHhmmss(int milli) {

  int timerS = milli / 1000;
  int hours = timerS / 3600;
  int hoursRem = timerS % 3600;
  int mins = hoursRem / 60;
  int secs = hoursRem % 60;
  char timeStr[20];
  sprintf(timeStr, "%02d:%02d:%02d", hours, mins, secs);
  return String(timeStr);
}