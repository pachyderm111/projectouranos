/*
_____________________________________________________________
Variables for Pi Pico W data logger
Code by: Radhakrishna Vojjala
Date of last modification: 16 Mar 2026
_____________________________________________________________
This file contains most of the variables used for the Pi Pico W flight logger.
If other sensors are added, add their variables to this file for cleaner code formatting.
*/

// Variables for the system

#define SERIAL_BAUD 115200
#define ERR_LED_PIN 20
#define LOOP_LED_PIN 21
#define I2C_1_SDA 2
#define I2C_1_SCL 3

int nowTimeMS = 0;
float nowTimeS = 0;
float nowTimeMin = 0;
int prevTime = 0;
String HHMMSS = "";
String data = "";
float freq = 0;
int loopTime = 0;
bool error = false;
int Volt = 0;
// Variables for SPI SD card reader

#define CS 17    // Chip select pin is GP17
#define MOSI 19  // Master out Slave in (SPI TX) is GP19
#define MISO 16  // Master in Slave out (SPI RX) is GP16
#define SCK 18   // SPI clock signal pin is GP18

File dataLog;
bool SDstatus = true;
bool SDfull = true;
bool filesAvailable = true;
char dataFilename[] = "PICOLO00.csv";  //Make sure dataFileN matches first and second place of the zeros in terms of the arrays index
const byte dataFileN1 = 6;
const byte dataFileN2 = 7;

// Variables for MS5611

MS5611 ms;
const double R = 287.05; // Gas constant for air
bool msOnline = false;
double refPress = 0;
double MStempC = 999;
double MStempF = 1830.2;  // 999C in F
double MStempRK = 3.14159265359;
double pressPa = 0;
double presskPa = 0;
double pressATM = 0;
double pressPSI = 0;
double absAltM = 0;
double absAltFt = 0;
double relAltM = 0;
double relAltFt = 0;
double vertVelM = 0;
double vertVelFt = 0;
double density = 0;
// Additional variables here

#define MONITOR_SIZE 20480 // 20KB of SRAM to monitor (Adjust based on free memory)
uint8_t goldArray[MONITOR_SIZE];
const uint8_t GOLD_PATTERN = 0xAA; // Binary 10101010
uint32_t totalFlips = 0;


