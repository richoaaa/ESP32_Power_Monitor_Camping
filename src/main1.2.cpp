/* This project lends off Trailer level v10
  Designed for Camping
  Using 8 pins for LCD screen (D5, D18, D22, D23, GND, 3.3V, 5V)
  SENSOR     |   PIN
  ADS712 SOL     D34
  ADS712 AUX out D39
  Charging shunt D36
  Batt sense     D35
  DS18B20        D32
    */
// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <ESP8266WebServer.h>
// #include <ESP8266mDNS.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#include <Arduino.h>
#include "esp_heap_caps.h"
#include <SPIFFS.h>
#include <WiFi.h>
#include <TimeLib.h>  // Make sure to include the Time library
// #include <ESP8266WebServer.h>
// #include <ESPAsyncWebServer.h>
// #include <ESPAsync_WiFiManager.h>
#include <WebServer.h>
#include <ElegantOTA.h>
#include <EEPROM.h>       //0:
#define EEPROM_SIZE 4000  //4096
#include <ESPmDNS.h>
// #include <Update.h>

#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "index.h"
#include "config.h"
#include "test.h"
#include "stats.h"
#include "chart1.2.h"
// #include "updateESP.h"
#include <OneWire.h>  //Library for temperature sensor DS18B20
#include <Wire.h>     // Must include Wire library for I2C
int eprompreburn;

// Declare pins
int solarPin = 34;
int ampsOutPin = 39;
int chargerPin = 36;
const int voltsPin  = 35; // CharBattery voltage
int tempPin = 32;

OneWire ds(tempPin);                     //Temperature sensor setup
unsigned long lastMeasureTime = 0;       // Storing the temperature time shift
char Temp1[10];                          // Storing the temperature string result for parsing to client
float tempsum1 = 0.0;                    // Storing the sum of the temperatures read over a given time period
int counter;                             // Stores the number of samples over a given sample period for voltage & current sensors
int counter2;                            // Stores the number of samples over a given sample period for temperature
int counter15 = 0;                       // Stores the number of samples over a given sample period for 15 minute intervzls
int counter2hr = 0;                      // Stores the number of samples over a given sample period for 2 hr intervals
String getContentType(String filename);  // convert the file extension to the MIME type
bool handleFileRead(String path);        // send the right file to the client (if it exists)
static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char FS_INIT_ERROR[] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND[] PROGMEM = "FileNotFound";
#define DBG_OUTPUT_PORT Serial

int BatCapacity;
int ChargeFactor;
float Currentvoltage;  //battery voltage
// const float QOV = 2.468;
const float VCC = 5;     // Maximum volatage on the input pin meauring the voltage from the sensor.
float cutOffLimit = 0.02;  // set the current which below that value is zero
int count = 0;
unsigned long days;
const int ampsPin = 35;  // Note that the ESP32 cannot read ADC2 pins while WiFi is in use
unsigned int freeMem;
int LedSwitch = 0;
int LED = 2;
const char* ssid = "Camper Mate";
const char* password = "";

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18, /* data R/W)=*/ 23, /* CS=*/ 5, /* reset=*/ 22);
uint8_t z = 127;	// start value
uint32_t lcg_rnd(void) {
    z = (uint8_t)((uint16_t)65*(uint16_t)z + (uint16_t)17);
    return (uint32_t)z;
}

WebServer server(80);
WiFiClient client;  // Declare a WiFiClient object  *******NEW*******
bool updating = false;
bool clientConnected = false; // Declare the variable with initial value

// This section deals with the Amp sampling and graph array.
unsigned long previousMillis = 0;
unsigned long TemperaturepreviousMillis = 0;
unsigned long VoidCycleTime = 0;
unsigned long currentMillis = 0;
unsigned long minute15 = 0; //Captures every 15 minute millis
unsigned long millis2hr = 0; //Captures every 2  hour millis
unsigned long CycleTime;
const unsigned long const15minute = 1000 * 60 * 15;   // milliseconds in 15 minutes
const unsigned long const2hr = 1000 * 60 * 60 * 2;    // milliseconds in 2 hrs
const unsigned long const24hr = 1000 * 60 * 60 * 24;  // milliseconds in 24 hrs
unsigned long minutes;
unsigned long hours;
int solAmpsSum;  //sums the ADC measurements for averaging
int chargerAmpsSum;  //sums the ADC measurements for averaging
int ampsOutAmpsSum;  //sums the ADC measurements for averaging
float tempavg;   // Temperature: average variable
float volts;  // measures the voltage on the battery
float voltsADC;  // measures the voltage ADC
float voltsSum = 0.00;
float Cycle15minCounter = 0;  // counts the number of cucles over the 15 minute interval
float volts15log[1344];  // 14 days x 24 hrs x 4 = 1344
float volts2hr = 0.00;
float floatVoltage = 14.4;
float SOC = 100;

// float Amps = 0.00;
float ampsIn = 0.00;
float ampsIn15 = 0.00;
float amphIn15 = 0.00;
float ampsInSum = 0.00;   //  sum all of ampin readings for 15 min graph.
float ampsIn15log[1344];  // 14 days x 24 hrs x 4 = 1344amphIn
float amphIn15log[1344];
float amphIn = 0.00;

float solarAmps = 0.00;
float solarADC;
float solarampsIn = 0.00;
float solarampsIn15 = 0.00;
float solarampsIn15log[1344];  // 14 days x 24 hrs x 4 = 1344

float chargerAmps = 0.00;
float chargerADC;
float chargerampsIn = 0.00;
float chargerampsIn15 = 0.00;
float chargerampsIn15log[1344];  // 14 days x 24 hrs x 4 = 1344

float ampsOut = 0;
float ampsOutADC;
float ampsOutSum = 0.00;  //  sum all of ampin readings for 15 min graph.
float ampsOut15 = 0;
float amphOut15 = 0;
float ampsOut15log[1344];  // 14 days x 24 hrs x 4 = 1344
float amphOut15Log[1344];
float amphOut = 0.00;

int interval15s = 0;  //  these are the number of 15 minutes cycles
int interval24s = 0;  //  these are the number of 24 hour cycles
// float volthr = 0.00;
float MaxampIn = 0;
String MaxampInTime = "";
float MaxampOut = 0;
String MaxampOutTime = "";
float MinVolts = 100;
String MinVoltsTime = "";
float MaxVolts = 0;
String MaxVoltsTime = "";
float MinSOC = 100;
String MinSOCTime = "";
float MaxSOC = 0;
String MaxSOCTime = "";
float FridgeMin = 100;
String FridgeMinTime = "";
float FridgeMax = -100;
String FridgeMaxTime = "";
float RadMin = 100;
String RadMinTime = "";
float RadMax = -100;
String RadMaxTime = "";
float MaxPowerOut = 0;
String MaxPowerOutTime = "";
float MaxPowerIn = 0;
String MaxPowerInTime = "";
String Message = "initialising...";
String Status = "initialising...";
String json;
// JSONVar SensorReadings;
JsonVariant SensorReadings;
int hrAdj = 0;
int minAdj = 0;
unsigned long millisAdj = 0;  //  Millis when time adjustment is captured

unsigned long interval = 1000;  // time to spend taking a sample
String RegisteredTime;         // to track the config time
time_t registeredMillis = 0;   //  Millis at which the updated date/time was stored
time_t registeredEpochTime;
String readableTime;           // to track the server time
String readableShortTime;           // to track the server time
String ServerUpTime;           // to track the server time
unsigned long GraphTimeCommon = 0; // (seconds) to use a common time for graphing multiple series data
unsigned long GraphTimeStarted = 0; // (seconds) to use a common time for graphing multiple series data

size_t getFreeHeap() {
  size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
  // Serial.printf("Free heap memory: %d bytes\n", freeHeap);
  return freeHeap;
}


void handleRoot() {  // Evoked when somebody accesses the root of the server.
  // The next line will execute function handleNotFound() if somebody would try to access anything else other then the root of the server.
  server.send(200, "text/html", MAIN_page);  //Send web page
}

time_t stringToTime_t(String timeString) {
  // Convert the string to an integer
  long epochTimeSeconds = timeString.toInt();
  // Cast the integer to a time_t object
  return (time_t)epochTimeSeconds;
}

void handlePage2() {  //  CONFIGURATION PAGE
  // Serial.println("Settings page");
  if (server.method() == HTTP_POST) {
    String tempy3 = server.arg("ChargeEfficiency");
    String tempy4 = server.arg("battery");
    String tempy5 = server.arg("clockID"); // Ignore clockID as time is extracted from dateTimeID
    String tempy6, tempy7;
    String tempy8 = server.arg("Reset");
    RegisteredTime = server.arg("dateTimeID"); // Use dateTimeID for time selection
    registeredMillis = millis();   //  Millis at which the updated date/time was stored
    // Serial.print("Javascript epochtime: "); Serial.println(server.arg("epochTime"));
    String epochTimeString = server.arg("epochTime");
    registeredEpochTime = stringToTime_t(epochTimeString);

    // Serial.print("Device time: "); Serial.print(RegisteredTime);
    // Serial.print(",  Registered Epochtime: "); Serial.println(registeredEpochTime);
    // Device time: 2/13/2024, 9:30:07 AM  Registered time: 1707859807


    unsigned long TempMillis = millis();
    // Serial.println("Running EEPROM burn...");

    if (TempMillis >= 40) {
      EEPROM.write(0, tempy4.toInt());  //Battery capacity
      BatCapacity = tempy4.toInt();
    }
    if (TempMillis >= 60) {
      EEPROM.write(1, tempy3.toInt());  // Charge Efficiency
      ChargeFactor = tempy3.toInt();
    }
    if (TempMillis >= 80) {
      EEPROM.commit();
    }
    if (tempy8 == "on") {
      amphIn = 0;
      amphOut = 0;
    }
  }
  server.send(200, "text/html", CONFIG_page);  //Send web page
}

void handlePage3() {                          //REFRESH STATS PAGE
  server.send(200, "text/html", STATS_page);  
}

void handleTestPage() {                          //REFRESH TEST PAGE
  server.send(200, "text/html", TEST_page);  
}

void handlePage4() {  //ampsOut GRAPH PAGE
  server.send(200, "text/html", GRAPH_page);
}
// void ampsOutUpdatePage() {                        // Evoked when somebody accesses the root of the server.
//   server.send(200, "text/html", UPDATE_page);  //Send web page
// }
void handleUpload() {
  updating = true;  // Set updating flag to true
  HTTPUpload& upload = server.upload();
 // Serial.print("UpampsOuting ");
  // Serial.print(upampsOut.filename.c_str());
  // Serial.print(" (");
  // Serial.print(upampsOut.totalSize);
  // Serial.println(" bytes)");

  // Serial.println("Serial write...");
  // Serial.write(upampsOut.buf, upampsOut.currentSize);
  int totalBytesWritten = 0;
  int totalBytesToWrite = upload.currentSize;

  while (totalBytesWritten < totalBytesToWrite) {
    int bytesWritten = Serial.write(upload.buf + totalBytesWritten, totalBytesToWrite - totalBytesWritten);
    if (bytesWritten < 0) {
      // Serial.println("Error writing to serial port");
      break;
    }
    totalBytesWritten += bytesWritten;

    // Calculate progress percentage and print to serial monitor
    float progress = (float)totalBytesWritten / totalBytesToWrite * 100;
    // Serial.printf("Progress: %.2f%%\n", progress);
  }

  updating = false;  // Set updating flag back to false
  if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
    Serial.println("Error: write failed");
    // Serial.write(upload.buf, upload.currentSize);  // This will print the data being received from the web page to the Serial Monitor

  }

  if (upload.totalSize == upload.currentSize) {
    if (Update.end()) {
      // Serial.println("OTA update successful");
    } else {
      // Serial.println("OTA update failed");
    }
  }
}


String convertToHumanReadableTime(time_t timestamp) {
  // Serial.print(" Converting an Epochtime into Human: "); Serial.println(timestamp);
  timestamp += 8 * 3600; // 8 hours * 3600 seconds/hour
  struct tm *timeInfo2;
  timeInfo2 = localtime(&timestamp);

  char buffer[80];
  // strftime(buffer, sizeof(buffer), "%d/%m/%Y, %I:%M:%S %p", timeInfo2); eg 11/02/2024, 08:56:18 PM
  strftime(buffer, sizeof(buffer), "%a %H:%M", timeInfo2);  // was "%a %I:%M %p"
  String formattedTime = String(buffer);
  return readableTime = formattedTime;
}

void convertToHumanReadableShortTime(time_t timestamp) {
  timestamp += 8 * 3600; // 8 hours * 3600 seconds/hour
  struct tm *timeInfo2;
  timeInfo2 = localtime(&timestamp);

  char buffer[80];
  strftime(buffer, sizeof(buffer), "%H:%M", timeInfo2);
  readableShortTime = String(buffer);
}

unsigned long GraphTime() {// Returns the actual epoch time (seconds)for live graphing. Converted to millseconds in JS.
  if (registeredEpochTime != 0) {
    return GraphTimeCommon = registeredEpochTime + millis()/1000 - registeredMillis/1000;
  } else {
    return GraphTimeCommon = millis()/1000;
  }
}

unsigned long GraphTimeHistory() {// returns the actual epoch time (seconds) of when the ESP started
  unsigned long GraphTimeStarted2;
  if (registeredEpochTime != 0) {
    GraphTimeStarted = GraphTimeCommon - millis()/1000;
  } else {
    GraphTimeStarted = millis()/1000;
  }
  return GraphTimeStarted;
  }
  
void getServerUpTime() {
  unsigned long currentMillis;
  unsigned long seconds;
  unsigned long days;
  ServerUpTime ="";
  currentMillis = millis();
  seconds = currentMillis / 1000;
  minutes = (seconds / 60);
  hours = (minutes / 60);
  days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  
  if (days > 0) {
    ServerUpTime = String(days) + " ";
  }
  if (hours > 0) {
    ServerUpTime += String(hours) + ":";
  }
  if (minutes < 10) {
    ServerUpTime += "0";
  }
  ServerUpTime += String(minutes) + ":";
  if (seconds < 10) {
    ServerUpTime += "0";
  }
  ServerUpTime += String(seconds);
}

void getConfig() {
  BatCapacity = EEPROM.read(0);
  ChargeFactor = EEPROM.read(1);

  //  Serial.print("Batt size read is ");
  //  Serial.println(BatCapacity);
  String text1 = "{\"data\":[";

  text1 += "{\"dataValue\":\"";
  text1 += BatCapacity;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += ChargeFactor;
  text1 += "\"},";  //Note this end stopper is different

  text1 += "{\"dataValue\":\"";
  text1 += getFreeHeap();
  text1 += "\"}";  //Note this end stopper is different
  text1 += "]}";  //
  server.send(200, "text/html", text1);
  //  Serial.println(text1);
  //  Serial.println("getConfig has been served");
}

void getFreeMemory() {
  freeMem = ESP.getFreeHeap();
}

void getTest() {
  getFreeMemory();
  // int tmpVolts = analogRead(voltsPin);
  // int tmpAmps = analogRead(ampsPin);
  String text1 = "{\"data\":[";

  text1 += "{\"dataValue\":\"";
  text1 += voltsADC,0;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += chargerADC,0;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += solarADC,0;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += ampsOutADC,0;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += freeMem;
  text1 += "\"}";  //Note this end stopper is different

  text1 += "]}";  //
  server.send(200, "text/html", text1);
  //  Serial.println(text1);
  //  Serial.println("getConfig has been served");
}

void getStats() {
  // readings();

  String text1 = "{\"data\":[";
  text1 += "{\"dataValue\":\"";
  text1 += MaxampIn;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MaxampInTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MaxampOut;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MaxampOutTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MinVolts;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MinVoltsTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MaxVolts;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MaxVoltsTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MinSOC;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MinSOCTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MaxSOC;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += MaxSOCTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += FridgeMin;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += FridgeMinTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += FridgeMax;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += FridgeMaxTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += RadMin;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += RadMinTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += RadMax;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += RadMaxTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += readableTime;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += getFreeHeap();
  text1 += "\"}";  //Note this end stopper is different

  text1 += "]}";  //
  server.send(200, "text/html", text1);
}



void demoMode() {
  // voltsADC = random(1900,2200);
  // chargerADC = random(1400,1800);
  // solarADC = random(1400,1800);
  // ampsOutADC = random(1400,1800);
  // SOC = random(10,1000)/10;
  voltsADC = 1900 + millis() / minute15 + 50;
  chargerADC = 3000 + millis() / minute15 + 50;
  solarADC = 3100 + millis() / minute15 + 50;
  ampsOutADC = 3200 + millis() / minute15 +  50;
  SOC = 47 + millis() / minute15 + 1;

}
void getData() {  // Content for main page
  // readings();
  // char temp[200];

  String text2 = "{\"data\":[";
  text2 += "{\"dataValue\":\"";
  text2 += volts;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += solarAmps;
  text2 += "\"},";
  text2 += "{\"dataValue\":\"";
  text2 += chargerAmps;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += ampsOut;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += SOC;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += Temp1;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += amphIn;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += amphOut;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += Message;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += Status;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += convertToHumanReadableTime(GraphTimeCommon);
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += ServerUpTime;
  text2 += "\"}";  //Note this end stopper is different


  text2 += "]}";  //
  server.send(200, "text/html", text2);
  // count++;
}


void (*resetFunc)(void) = 0;  //declare reset function at address 0

void SystemReboot() {
  resetFunc();  //call reset to restart the microcontroller
}

String getContentType(String filename) {  // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  return "text/plain";
}

bool handleFileRead(String path) {  // send the right file to the client (if it exists)
  //  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  if (SPIFFS.exists(path)) {                             // If the file exists
    File file = SPIFFS.open(path, "r");                  // Open it
    size_t sent = server.streamFile(file, contentType);  // And send it to the client. This is commented out as compiling identified an issue with it
    file.close();                                        // Then close the file again
    return true;
  }
  // Serial.print("HandleFileRead: File Not Found. Can not find ");
  // Serial.println(path);
  return false;  // If the file doesn't exist, return false
}

////////////////////////////////
// Utils to return HTTP codes

void replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}

void replyOKWithMsg(String msg) {
  server.send(200, FPSTR(TEXT_PLAIN), msg);
}

void replyNotFound(String msg) {
  server.send(404, FPSTR(TEXT_PLAIN), msg);
}

void replyBadRequest(String msg) {
  DBG_OUTPUT_PORT.println(msg);
  server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void replyServerError(String msg) {
  DBG_OUTPUT_PORT.println(msg);
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

////////////////////////////////
// Request handlers

/*
   The "Not Found" handler catches all URI not explicitely declared in code
   First try to find and return the requested file from the filesystem,
   and if it fails, return a 404 page with debug information
*/
void handleNotFound() {
  String uri = WebServer::urlDecode(server.uri());  // required to read paths with blanks

  if (handleFileRead(uri)) {
    return;
  }

  // Dump debug data
  String message;
  message.reserve(100);
  message = F("Error: File not found\n\nURI: ");
  message += uri;
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += '\n';
  for (uint8_t i = 0; i < server.args(); i++) {
    message += F(" NAME:");
    message += server.argName(i);
    message += F("\n VALUE:");
    message += server.arg(i);
    message += '\n';
  }
  message += "path=";
  message += server.arg("path");
  message += '\n';
  DBG_OUTPUT_PORT.print(message);

  return replyNotFound(message);
}


void SaveConFig() {
  if (server.method() == HTTP_POST) {
    // Serial.println("Button 1 pressed");
  }
}

void SetClock() {
  if (server.method() == HTTP_POST) {
    // Serial.println("Button 1 pressed");
  }
}

void ProcessStatsRst() {
  // Serial.println("Stats resetting...");
  amphIn = 0;
  amphOut = 0;
  MaxampIn = 0;
  MaxampInTime = "";
  MaxampOut = 0;
  MaxampOutTime = "";
  MinVolts = 100;
  MinVoltsTime = "";
  MaxVolts = 0;
  MaxVoltsTime = "";
  MinSOC = 100;
  MinSOCTime = "";
  MaxSOC = 0;
  MaxSOCTime = "";
  FridgeMin = 100;
  FridgeMinTime = "";
  FridgeMax = -100;
  FridgeMaxTime = "";
  RadMin = 100;
  RadMinTime = "";
  RadMax = -100;
  RadMaxTime = "";
}

float readTempData1() {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8] = { 0x28, 0x3D, 0x6C, 0x16, 0xA8, 0x01, 0x3C, 0xB2};

  float celsius;

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);  // start conversion,
  delay(50);

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);  // Read Scratchpad

  for (i = 0; i < 9; i++) {  // we need 9 bytes
    data[i] = ds.read();
  }

  int16_t raw1 = (data[1] << 8) | data[0];

  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw1 = raw1 & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20)
    raw1 = raw1 & ~3;  // 10 bit res, 187.5 ms
  else if (cfg == 0x40)
    raw1 = raw1 & ~1;  // 11 bit res, 375 ms
  //// default is 12 bit resolution, 750 ms conversion time

  celsius = (float)raw1 / 16.0;
  return celsius;
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  // Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    // Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    // Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      // Serial.print("  DIR : ");
      // Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      // Serial.print("  FILE: ");
      // Serial.print(file.name());
      // Serial.print("\tSIZE: ");
      // Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void getGraphData() {
  String text1 = "{\"graph\":[";
  text1 += String(GraphTimeCommon);
  // Serial.print("Graph time (secs): "); Serial.println(GraphTimeCommon);
  text1 += ",";
  text1 += ampsIn,1;
  text1 += ",";
  text1 += ampsOut,1;
  text1 += ",";
  text1 += volts,1;
  text1 += "]}";

  server.send(200, "text/html", text1);
  // Serial.print("Current readings: ");
  // Serial.print("\t");
  // Serial.println(text1);
  // 2658459488
  // 1707688526
  // 1707761712
}


// void getGraphAmpsIn() {   // is this redundant? I think so...
//   // Serial.print("Intervals: ");
//   // Serial.println(interval15s);
//   String text1 = "{\"ampin\":[";
//   for (int i = 1; i <= interval15s; i--) {
//     text1 += "[";
//     text1 += (GraphTimeStarted + (interval15s - i * 15 * 60 * 1000));
//     text1 += ",";
//     text1 += ampsIn15log[i],1;
//     text1 += "], ";
//   }
//   text1 += "[";
//   text1 += GraphTimeCommon;
//   text1 += ",";
//   text1 += ampsIn15log[0],1;
//   text1 += "]";
//   text1 += "]}";

//   server.send(200, "text/html", text1.c_str());
//   // Serial.println(text1);
// }

void getGraphData2() {
String text1 = "{\"ampin\": ["; // Create arrays for the additional variables
String text2 = "{\"ampout\": ["; // Create arrays for the additional variables
String text3 = "{\"volts\": ["; // Create arrays for the additional variables
// Serial.print("Interval15s:");
// Serial.println(interval15s);
  for (int i = interval15s; i > 1 && i <= interval15s; i--) {   // interval15s are the number of 15 minute readings since start
    // Serial.print("Graphdata i = "); Serial.println(i);
    unsigned long TimePlot = GraphTimeStarted + ((interval15s + 1 - i) * 15 * 60);
    text1 += "[";
    text1 += TimePlot;
    text1 += ",";
    text1 += ampsIn15log[i-1],1;
    text1 += "], ";

    text2 += "[";
    text2 += TimePlot;
    text2 += ",";
    text2 += ampsOut15log[i-1],1;
    text2 += "], ";

    text3 += "[";
    text3 += TimePlot;
    text3 += ",";
    text3 += volts15log[i-1],1;
    text3 += "], ";
  }
  text1 += "[";
  text1 += GraphTimeCommon;
  text1 += ",";
  text1 += ampsIn15log[0],1;
  text1 += "]";
  text1 += "]}";

  text2 += "[";
  text2 += GraphTimeCommon;
  text2 += ",";
  text2 += ampsOut15log[0],1;
  text2 += "]";
  text2 += "]}";

  text3 += "[";
  text3 += GraphTimeCommon;
  text3 += ",";
  text3 += volts15log[0],1;
  text3 += "]";
  text3 += "]}";

  String combinedText = "[" + text1 + ", " + text2 + ", " + text3 + "]";
  // Serial.println(combinedText);
  server.send(200, "text/html", combinedText.c_str());
}

void getGraphAh() {
    String text1 = "{\"amphrin\":[";
    String text2 = "{\"amphrout\":[";
    for (int i = interval15s; i > 1 && i <= interval15s; i--) {
        // Add data point to the JSON string
        unsigned long TimePlot = GraphTimeStarted + ((interval15s + 1 - i) * 15 * 60);
        text1 += "[";
        text1 += TimePlot;
        text1 += ",";
        text1 += amphIn15log[i-1],1;
        text1 += "], ";

        text2 += "[";
        text2 += TimePlot;
        text2 += ",";
        text2 += amphOut15Log[i-1],1;
        text2 += "], ";
    }
    text1 += "[";
    text1 += GraphTimeCommon;
    text1 += ",";
    text1 += amphIn15log[0],1;
    text1 += "]";
    text1 += "]}";

    text2 += "[";
    text2 += GraphTimeCommon;
    text2 += ",";
    text2 += amphOut15Log[0],1;
    text2 += "]";
    text2 += "]}";

    String combinedText = "[" + text1 + ", " + text2 + "]";
    Serial.println(combinedText);
    server.send(200, "text/html", combinedText.c_str());
}


void draw_m0_h_with_extra_blank()
{
    u8g2.clearBuffer();

    u8g2.setFontMode(1);
    // u8g2.setFont(u8g2_font_cu12_tr);
    // u8g2.setFont(u8g2_font_6x13_mf);
    // u8g2.setFont(u8g2_font_helvB12);
    u8g2.setFont(u8g2_font_samim_16_t_all);
    u8g2.setCursor(0,12);
    u8g2.print(volts,1);
    u8g2.print(F("V  "));
    u8g2.setCursor(80,12);
    u8g2.print(readableShortTime);
    u8g2.setCursor(0,29);
    u8g2.print(F("S: "));
    u8g2.print(solarAmps,1);
    u8g2.print("A ");
    u8g2.setCursor(68,29);
    u8g2.print(F("C: "));
    u8g2.print(chargerAmps,1);
    u8g2.print("A ");
    u8g2.setCursor(0, 46);
    u8g2.print(F("OUT: "));
    u8g2.print(ampsOut,1);
    u8g2.print("A ");
    u8g2.setCursor(0, 63);
    u8g2.print(F("SOC: "));
    u8g2.print(SOC,1);
    u8g2.print(" %");
    u8g2.setCursor(94, 63);
    float temperature;
    String temp1String(Temp1,10);
    temperature = temp1String.toFloat();
    u8g2.print(temperature,0);
    // u8g2.drawStr(1,1,temp1String.c_str());
    // Serial.print("Screen temperature: ");
    // Serial.println(temp1String);
    // Serial.print(Temp1);
    u8g2.setFont(u8g2_font_6x13_mf);
    u8g2.setCursor(115, 63);
    u8g2.print("°C");		// requires enableUTF8Print()
    u8g2.sendBuffer();
}

void ReadTemperatureSensors() {
  counter2++;
  tempsum1 = tempsum1 + readTempData1();
  if (counter2 == 20) {
    tempavg = tempsum1 / counter2;
    counter2 = 0;
    tempsum1 = 0;
    dtostrf(tempavg, 2, 0, Temp1);  // dtostrf(float_value, min_width, num_digits_after_decimal, where_to_store_string)
    if (tempavg > FridgeMax) {  // Fridge temperature max/min
      FridgeMax = tempavg;
      FridgeMaxTime = readableTime;
    }
    if (tempavg < FridgeMin) {
      FridgeMin = tempavg;
      FridgeMinTime = readableTime;
    }
  }
}

void readings() {
  interval15s = millis() / const15minute;  // getting the number of 15 minutes cycles since powered on
  interval24s = (millis() / const24hr);
  getServerUpTime();
  GraphTime(); // Returns GraphTimeCommon, the actual epoch time more for live graphing
  convertToHumanReadableTime(GraphTimeCommon); // Returns ReadableTime for stats
  convertToHumanReadableShortTime(GraphTimeCommon); // Returns ReadableTime for LCD display
  GraphTimeHistory(); //Returns GraphTimeStarted, the actual epoch time the ESP started
  previousMillis = millis();
  solAmpsSum = 0, chargerAmpsSum = 0; int tempAmpsOutSum = 0; int tempvoltsSum = 0; counter = 0;
  previousMillis = millis();
  Cycle15minCounter++;

  while ((millis() - previousMillis) < interval) {
    counter++;
    tempvoltsSum += analogRead(voltsPin);
    chargerAmpsSum += analogRead(chargerPin);
    solAmpsSum += analogRead(solarPin);
    tempAmpsOutSum += analogRead(ampsOutPin);
  }
  ReadTemperatureSensors();
  voltsADC = (tempvoltsSum / counter);
  chargerADC = chargerAmpsSum / counter;
  solarADC = solAmpsSum / counter;
  ampsOutADC = tempAmpsOutSum / counter;

  volts = (voltsADC - 181.388) / 196.804 ;
  chargerAmps = (chargerADC - 3110.4131) / -71.3508;

  solarAmps = (solarADC -  3080.323) / -233.438 ;
  ampsOut = abs((ampsOutADC - 3155.1656) / -115.8277);
  if (solarAmps < cutOffLimit) {solarAmps = 0;}
  if (chargerAmps < cutOffLimit) {chargerAmps = 0;} // ampsOut cutoff is not required

  ampsIn = solarAmps + chargerAmps;
  ampsInSum += ampsIn; // the following variables are used for 15 minute accounting
  ampsOutSum += ampsOut;
  voltsSum += volts;

  // *** DETERMINE THE MIN/MAX VOLTS AND THEIR chargerAmps RESPECTIVE TIME***
  if (volts < MinVolts) {
    MinVolts = volts;
    MinVoltsTime = readableTime;
  }
  if (volts > MaxVolts) {
    MaxVolts = volts;
    MaxVoltsTime = readableTime;
  }

  amphIn15 += ampsIn * (CycleTime / 3600000.00000000);  // 3,600,000 is 1 hr
  amphIn = amphIn + ampsIn * (CycleTime / 3600000.00000000); // RETURNS THE TOTAL AH
  amphOut15 += ampsOut * (CycleTime / 3600000.00000000);
  amphOut = amphOut + ampsOut * (CycleTime / 3600000.00000000);
  ampsIn15log[0] = ampsInSum/Cycle15minCounter;  //  Updates the ampsin graph
  ampsOut15log[0] = ampsOutSum / Cycle15minCounter;  //  Updates the ampsout graph
  // Serial.print("Ampsin15log: "); Serial.println(ampsIn15log[0]);
  volts15log[0] = voltsSum / Cycle15minCounter;   //  Updates the volts graph
  amphIn15log[0] = amphIn15;
  amphOut15Log[0] = amphOut15;

  if (ampsIn > MaxampIn) {
    MaxampIn = ampsIn;
    MaxampInTime = readableTime;
  }

  if (ampsOut > MaxampOut) {
    MaxampOut = ampsOut;
    MaxampOutTime = readableTime;
  }

  CycleTime = millis() - VoidCycleTime;
  VoidCycleTime = millis();

  //15 minute averages for use in graphs
  if (millis() - minute15 > const15minute) {  // minute15 is the millis at startup and after each 15 minute cycle
    interval15s = millis() / const15minute;  // getting the number of 15 minutes cycles since powered on
    // Serial.println("Shifting array positions...");
    for (int i = interval15s; i > 0 && i < 1344; i--) {  // go through the array and shift along
      ampsIn15log[i] = ampsIn15log[i-1];
      ampsOut15log[i] = ampsOut15log[i-1];
      amphIn15log[i] = amphIn15log[i-1];
      amphOut15Log[i] = amphOut15Log[i-1];
      volts15log[i] = volts15log[i-1];
    }
    
    if (ampsOut15log[0] > ampsIn15log[0]) {
      Message = "DISCHARGING";
    } else if (ampsOut15log[0] < ampsIn15log[0]) {
      Message = "CHARGING";
    } else if (ampsOut15log[0] == ampsIn15log[0]) {
      Message = "STATIC";
    }
    if (millis() - millis2hr >= const2hr) {
      volts2hr = 0;
      for (int i = 1; i <= 8; i++) {
        volts2hr += volts15log[i];
      }
      volts2hr = volts2hr / 8;  //  Average voltage over 2 hours
      millis2hr = millis();
    }
    

    if (SOC < 90 && volts15log[0] < floatVoltage) {
      Status = "BOOST...";
    } else if (SOC > 90 && volts15log[1] > floatVoltage && volts2hr < floatVoltage) {
      Status = "ABSORPTION...";
    } else if (SOC > 90 && volts2hr > floatVoltage) {
      Status = "FLOAT...";
      amphIn = BatCapacity;
      amphOut = 0.0;
    } else {
      Status = "INITIALISING...";
    }
  amphIn15 = 0;
  amphOut15 = 0;
  ampsOutSum = 0;  // this resets the average amps
  ampsInSum = 0;   // this resets the average amps
  voltsSum = 0;    // this resets the average voltage
  // Serial.println("15 minute routine complete...");
  minute15 = millis();
  Cycle15minCounter = 0;
  }
  SOC = (BatCapacity + (amphIn * ChargeFactor / 100.00) - amphOut) / BatCapacity * 100;

  if (SOC < 0) {
    SOC = 0;
    amphIn = 0;
    amphOut = BatCapacity;
  } else if (SOC > 100) {
    amphIn = 0;
    amphOut = 0;
    SOC = 100;
  }

  if (SOC > MaxSOC) {
    MaxSOC = SOC;
    MaxSOCTime = readableTime;
  } else if (SOC < MinSOC) {
    MinSOC = SOC;
    MinSOCTime = readableTime;
  }
}


void writeLongArrayIntoEEPROM(int address, long numbers[], int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++)
  {
    EEPROM.write(addressIndex, (numbers[i] >> 24) & 0xFF);
    EEPROM.write(addressIndex + 1, (numbers[i] >> 16) & 0xFF);
    EEPROM.write(addressIndex + 2, (numbers[i] >> 8) & 0xFF);
    EEPROM.write(addressIndex + 3, numbers[i] & 0xFF);
    addressIndex += 4;
  }
}
void readLongArrayFromEEPROM(int address, long numbers[], int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++)
  {
    numbers[i] = ((long)EEPROM.read(addressIndex) << 24) +
                 ((long)EEPROM.read(addressIndex + 1) << 16) +
                 ((long)EEPROM.read(addressIndex + 2) << 8) +
                 (long)EEPROM.read(addressIndex + 3);
    addressIndex += 4;
  }
}


void setup() {
    u8g2.begin();
    u8g2.enableUTF8Print();
    Wire.begin();
    pinMode(LED, OUTPUT);
    // Serial.begin(115200);
        // Get the free flash memory space
    uint32_t freeSpace = ESP.getFreeSketchSpace();
    // Serial.print("Free flash memory: ");
    // Serial.print(freeSpace);
    // Serial.println(" bytes");

    minute15 = millis();
    VoidCycleTime = millis();
    delay(300);  //These are so the serial window has time to display at startup.
    bool success = SPIFFS.begin();
    if (!success) {
        // Serial.println("Error mounting the file system");
        return;
    }
    BatCapacity = EEPROM.read(0);
    ChargeFactor = EEPROM.read(1);

    server.onNotFound([]() {                               // If the client requests any URI
        if (!handleFileRead(server.uri()))                   // send it if it exists
        server.send(404, "text/plain", "404: Not Found");  // otherwise, respond with a 404 (Not Found) error
    });
    EEPROM.begin(EEPROM_SIZE);
    BatCapacity = EEPROM.read(0);

    // Serial.print("Configuring access point...");
    WiFi.softAP(ssid);
    // Serial.println(WiFi.softAPIP() ? "Ready" : "Failed!");
    // IPAddress myIP = WiFi.softAPIP();
    // Serial.print("AP IP address: ");
    // Serial.println(myIP);
    MDNS.begin("power");
    server.on("/", HTTP_GET,handleRoot);
    server.on("/data", HTTP_GET,getData);
    server.on("/config", getConfig);
    server.on("/stats", getStats);
    server.on("/test", getTest);
    server.on("/graphdata", getGraphData);  // this is the live data graph
    server.on("/graphdata2", getGraphData2);  // this is the second data graph
    // server.on("/graphdataAmpsIn", getGraphAmpsIn);
    // server.on("/graphdataAmpsOut", getGraphAmpsOut);
    // server.on("/graphdataVolts", getGraphvolts);
    server.on("/graphdataAh", getGraphAh);
    server.on("/CONFIG_page", handlePage2);
    server.on("/STATS_page", handlePage3);
    server.on("/GRAPH_page", handlePage4);
    server.on("/TEST_page", handleTestPage);
    server.on("/SaveConfigButton", SaveConFig);
    server.on("/BUTTON_RST", ProcessStatsRst);
    server.on("/RebootButton", SystemReboot);
    server.on("/SetClockButton", SetClock);
    // server.on("/UPDATE_page", ampsOutUpdatePage);
    ElegantOTA.begin(&server);    // Start ElegantOTA
    // server.on("/update", HTTP_POST, handleUpampsOut);
    server.begin();
    Serial.println("HTTP server started");
    //  listDir(SPIFFS, "/", 0);  // this works, just don't want to see it anymore...
}

void loop() {
    unsigned long TempMillis = millis();
    // server.handleClient();
    while (millis() - TempMillis < 700) {
      server.handleClient();
      delay(2);//allow the cpu to switch to other tasks
      digitalWrite(LED, HIGH);
    }
    if (!client.connected()) {
        // Serial.println("Client disconnected");
        // Release resources associated with the client
        client.stop();
    }
    digitalWrite(LED, LOW);
    ReadTemperatureSensors();
    readings();
    GraphTime();
    GraphTimeHistory();
    draw_m0_h_with_extra_blank();

    // Serial.print("GraphtimeHistory: ");
    // Serial.print(GraphTimeHistory());
    // Serial.print("GraphTimeStarted: ");
    // Serial.print(GraphTimeStarted);
    // Serial.print("  GraphTimeCommon: ");
    // Serial.println(GraphTimeCommon);
    // Serial.print("Readable Time ");
    // Serial.println(readableTime);
}