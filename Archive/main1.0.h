/* This project lends off Trailer level v10
  Designed for Bremners caravan
    */
// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <ESP8266WebServer.h>
// #include <ESP8266mDNS.h>

#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
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
#include "chart.h"
// #include "updateESP.h"
#include <OneWire.h>  //Library for temperature sensor DS18B20
#include <Wire.h>     // Must include Wire library for I2C
int eprompreburn;

OneWire ds(33);                          //Temperature sensor setup
unsigned long lastMeasureTime = 0;       // Storing the temperature time shift
char Temp1[10];                          // Storing the temperature string result for parsing to client
char Temp2[10];                          // Storing the temperature string result for parsing to client
float tempsum1 = 0.0;                    // Storing the sum of the temperatures read over a given time period
float tempsum2 = 0.0;                    // Storing the sum of the temperatures read over a given time period
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

#define Ampspin 35  // current monitoring input. Note for ESP32 these can only be ADC1 as the ADC2 pins aren't accesiblw while WiFi on
int BatCapacity;
int ChargeFactor;
int MCUADC;
float Currentvoltage;  //battery voltage
// const float QOV = 2.468;
const float VCC = 3.3;     // Maximum volatage on the input pin meauring the voltage from the sensor.
float cutOffLimit = 0.02;  // set the current which below that value is zero
int count = 0;
unsigned long days;
const int voltsPin = 34;  // CharBattery voltage
const int voltsMCUPin = 32;
const int ampsPin = 35;  // Note that the ESP32 cannot read ADC2 pins while WiFi is in use
unsigned int freeMem;
int LedSwitch = 0;
int LED = 2;
const char* ssid = "Bremner Mate";
const char* password = "";

WebServer server(80);
bool updating = false;
// This section deals with the Amp sampling and graph array.
unsigned long previousMillis = 0;
unsigned long VoidCycleTime = 0;
unsigned long currentMillis = 0;
unsigned long minute15 = 0;
unsigned long CycleTime;
const unsigned long minute15const = 1000 * 60 * 15;
unsigned long hr2 = 1000 * 60 * 60 * 2;
unsigned long minutes;
unsigned long hours;
int ampsSum;  //sums the ADC measurements for averaging
float avg1;   // Temperature: average storer
float avg2;
float volts;  // measures the voltage on the charging current sensor
float volts15;
float volts15log[288];  // 72 hrs x 4
float volts2hr = 0.00;
float voltsMCU;
int SOC = 100;
float ampCounter = 0;
float Amps = 0.00;
float ampsIn = 0.00;
float ampsIn15 = 0.00;
float ampsIn15log[288];  // 72 hrs x 4
float ampsOut = 0;
float ampsOut15 = 0;
float ampsOut15log[288];  // 72 hrs x 4
int interval15s;
float amphIn = 0.00;
float amphOut = 0.00;
float volthOut = 0.00;
float voltsSum = 0.00;
float voltsMCUSum = 0;
float voltsCounter = 0;
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
String Message;
String Status;
String json;
// JSONVar SensorReadings;
JsonVariant SensorReadings;
int hrAdj = 0;
int minAdj = 0;

unsigned long interval = 300;  // time to spend taking a sample
String readableTime;           // to track the server time
String ServerUpTime;           // to track the server time

void handleRoot() {  // Evoked when somebody accesses the root of the server.
  // The next line will execute function handleNotFound() if somebody would try to access anything else other then the root of the server.

  server.send(200, "text/html", MAIN_page);  //Send web page
}
void handlePage2() {  //  CONFIGURATION PAGE
  // Serial.println("Settings page");
  if (server.method() == HTTP_POST) {
    String tempy3 = server.arg("ChargeEfficiency");
    String tempy4 = server.arg("battery");
    String tempy5 = server.arg("clockID");
    String tempy6, tempy7;
    String tempy8 = server.arg("Reset");
    // String message;
    // message += server.args();
    // message += "\n";
    // for (uint8_t i = 0; i < server.args(); i++) {
    //   message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    // }
    // Serial.println(message);

    tempy5.trim();
    int strLength = tempy5.length();
    int colonPosition = tempy5.indexOf(':');
    for (int i = 0; i < colonPosition; i++) {
      tempy6 += tempy5.charAt(i);
    }
    for (int i = colonPosition + 1; i < strLength; i++) {
      tempy7 += tempy5.charAt(i);
    }
    hrAdj = tempy6.toInt();
    minAdj = tempy7.toInt();
    if (hrAdj - hours < 24) {
      hrAdj = hrAdj - hours;
    } else {
      hrAdj = hrAdj - hours + 24;
    }

    if (minAdj - minutes < 60) {
      minAdj = minAdj - minutes;
    } else {
      minAdj = minAdj - minutes + 60;
    }

    unsigned long TempMillis = millis();
    Serial.println("Running EEPROM burn...");

    if (TempMillis >= 40) {
      EEPROM.write(0, tempy4.toInt());  //Battery capacity
    }
    if (TempMillis >= 60) {
      EEPROM.write(1, tempy3.toInt());  // Charge Efficiency
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


void handlePage4() {  //LOAD GRAPH PAGE
  server.send(200, "text/html", GRAPH_page);
}
// void LoadUpdatePage() {                        // Evoked when somebody accesses the root of the server.
//   server.send(200, "text/html", UPDATE_page);  //Send web page
// }
void handleUpload() {
  updating = true;  // Set updating flag to true
  HTTPUpload& upload = server.upload();
 // Serial.print("Uploading ");
  // Serial.print(upload.filename.c_str());
  // Serial.print(" (");
  // Serial.print(upload.totalSize);
  // Serial.println(" bytes)");

// Serial.println("Serial write...");
// Serial.write(upload.buf, upload.currentSize);
int totalBytesWritten = 0;
int totalBytesToWrite = upload.currentSize;

while (totalBytesWritten < totalBytesToWrite) {
  int bytesWritten = Serial.write(upload.buf + totalBytesWritten, totalBytesToWrite - totalBytesWritten);
  if (bytesWritten < 0) {
    Serial.println("Error writing to serial port");
    break;
  }
  totalBytesWritten += bytesWritten;

  // Calculate progress percentage and print to serial monitor
  float progress = (float)totalBytesWritten / totalBytesToWrite * 100;
  Serial.printf("Progress: %.2f%%\n", progress);
}


  // Serial.print("Upload status: ");
  // Serial.println(upload.status);
  // if (upload.status == UPLOAD_FILE_START) {   //  UPLOAD_FILE_START (value of 0)
  //   Serial.println("Ready to Upload!!");
  //   Serial.printf("Update file: %s\n", upload.filename.c_str());
  //   if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
  //     Serial.println("Error 1");
  //     Update.printError(Serial);
  //   }
    
  // } else if (upload.status == UPLOAD_FILE_WRITE) {    //  UPLOAD_FILE_WRITE (value of 1)
  //   Serial.println("Status is write...");
  //   if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
  //     Serial.write(upload.buf, upload.currentSize);
  //     Serial.print("Error 2");
  //     Update.printError(Serial);
  //   }
  // } else if (upload.status == UPLOAD_FILE_END) {    //    UPLOAD_FILE_END (value of 2)
  //   Serial.println("Status is complete...");
  //   if (Update.end(true)) {
  //     Serial.println("Update complete");
  //     Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
  //   } else {
  //     Serial.print("Error 3: ");
  //     Update.printError(Serial);
  //   }

  // if (updating) {
  //   while (Update.end() == false) {
  //     // delay(100);
  //   }
  // }

 


    updating = false;  // Set updating flag back to false
  // }
  if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
    Serial.println("Error: write failed");
    // Serial.write(upload.buf, upload.currentSize);  // This will print the data being received from the web page to the Serial Monitor

  }

  if (upload.totalSize == upload.currentSize) {
    if (Update.end()) {
      Serial.println("OTA update successful");
    } else {
      Serial.println("OTA update failed");
    }
  }
}

void getConfig() {
  BatCapacity = EEPROM.read(0);
  ChargeFactor = EEPROM.read(1);

  //  Serial.print("Batt size read is ");
  //  Serial.println(BatCapacity);
  String text1 = "{\"data\":[";

  text1 += "{\"dataValue\":\"";
  text1 += SOC;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += BatCapacity;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += ChargeFactor;
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
  int tmpVolts = analogRead(voltsPin);
  int tmpAmps = analogRead(Ampspin);
  String text1 = "{\"data\":[";

  text1 += "{\"dataValue\":\"";
  text1 += tmpVolts;
  text1 += "\"},";

  text1 += "{\"dataValue\":\"";
  text1 += tmpAmps;
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
  text1 += "\"}";  //Note this end stopper is different

  text1 += "]}";  //
  server.send(200, "text/html", text1);
}



void demoMode() {
  volts = 10 * voltsCounter;
  ampsOut = 1 * voltsCounter;
  ampsIn = 4 * voltsCounter;
}
void getData() {  // Content for main page
  // readings();
  // char temp[200];

  String text2 = "{\"data\":[";
  text2 += "{\"dataValue\":\"";
  text2 += volts;
  // text2 += counter2;
  // text2 += MCUADC;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += ampsIn;
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
  text2 += Temp2;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += voltsMCU;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += amphIn;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += amphOut;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += readableTime;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += Message;
  text2 += "\"},";

  text2 += "{\"dataValue\":\"";
  text2 += Status;
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
  Serial.print("HandleFileRead: File Not Found. Can not find ");
  Serial.println(path);
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
    Serial.println("Button 1 pressed");
  }
}

void SetClock() {
  if (server.method() == HTTP_POST) {
    Serial.println("Button 1 pressed");
  }
}

void ProcessStatsRst() {
  Serial.println("Stats resetting...");
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
  byte addr[8] = { 0x28, 0x5A, 0x2E, 0x79, 0x97, 0x18, 0x03, 0xB4};

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

float readTempData2() {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8] = {0x28, 0x88, 0x2D, 0x79, 0x97, 0x18, 0x03, 0x50};

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
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

unsigned long GraphTime() {  // the hrAdj and minAdj comes from the clock via the config page
  int temphr = hrAdj * 60 * 60 * 1000;
  int tempmin = minAdj * 60 * 1000;
  return (millis() + temphr + tempmin);
}
void getGraphData() {
  String text1 = "{\"graph\":[";
  text1 += GraphTime();
  text1 += ",";
  text1 += ampsIn;
  text1 += ",";
  text1 += ampsOut;
  text1 += ",";
  text1 += volts;
  text1 += "]}";  //Note this end stopper is different

  server.send(200, "text/html", text1);
  // {{"time":"01:24"},{"In":"9.60"},{"Out":"-8.60"}}
  Serial.print("Current readings: ");
  Serial.print("\t");
  Serial.println(text1);
}
unsigned long GraphTimeHistory() {
  return (GraphTime() - millis());
}

void sendGraphAmpsOut() {
  String text1 = "{\"ampout\":[";
  for (int i = 1; i <= interval15s; i++) {
    text1 += "[";
    text1 += GraphTimeHistory() + (i * 15 * 60 * 1000);
    text1 += ",";
    text1 += ampsOut15log[i - 1];
    if (i == interval15s) {
      text1 += "]";
    } else {
      text1 += "], ";
    }
  }
  text1 += "]}";

  server.send(200, "text/html", text1.c_str());
  Serial.print("\t");
  Serial.println(text1);
}
void sendGraphvolts() {
  String text1 = "{\"volts\":[";
  for (int i = 1; i <= interval15s; i++) {
    text1 += "[";
    text1 += GraphTimeHistory() + (i * 15 * 60 * 1000);
    text1 += ",";
    text1 += volts15log[i - 1];
    if (i == interval15s) {
      text1 += "]";
    } else {
      text1 += "], ";
    }
  }
  text1 += "]}";

  server.send(200, "text/html", text1.c_str());
  // Serial.print("\t");
  // Serial.println(text1);
}
void sendGraphAmpsOutCummulative() {
  String text1 = "{\"ampoutcum\":[";
  for (int i = 1; i <= interval15s; i++) {
    text1 += "[";
    text1 += GraphTimeHistory() + (i * 15 * 60 * 1000);
    text1 += ",";
    float cumsum = 0;
    for (int x = 1; x <= i; x++) {
      cumsum += ampsOut15log[x - 1];
    }
    text1 += cumsum;
    if (i == interval15s) {
      text1 += "]";
    } else {
      text1 += "], ";
    }
  }
  text1 += "]}";

  server.send(200, "text/html", text1.c_str());
  // Serial.print("\t");
  // Serial.print("sendGraphAmpsOutCummulative: ");
  // Serial.println(text1);
}

void getGraphAmpsInCummulative() {
  // Serial.print("Intervals: ");
  // Serial.println(interval15s);
  String text1 = "{\"ampincum\":[";
  for (int i = 1; i <= interval15s; i++) {
    text1 += "[";
    text1 += GraphTimeHistory() + (i * 15 * 60 * 1000);
    text1 += ",";
    float cumsum = 0;
    for (int x = 1; x <= i; x++) {
      cumsum += ampsIn15log[x - 1];
    }
    text1 += cumsum;
    if (i == interval15s) {
      text1 += "]";
    } else {
      text1 += "], ";
    }
  }

  text1 += "]}";

  server.send(200, "text/html", text1.c_str());
  // Serial.print("Historic data Amps In: ");
  // Serial.print(text1);
  // Serial.print("\t");
}

void getGraphAmpsIn() {
  // Serial.print("Intervals: ");
  // Serial.println(interval15s);
  String text1 = "{\"ampin\":[";
  for (int i = 1; i <= interval15s; i++) {
    text1 += "[";
    text1 += (GraphTimeHistory() + i * 15 * 60 * 1000);
    text1 += ",";
    text1 += (ampsIn15log[i - 1]);
    if (i == interval15s) {
      text1 += "]";
    } else {
      text1 += "], ";
    }
  }
  text1 += "]}";

  server.send(200, "text/html", text1.c_str());
  Serial.print("Historic data Amps In: ");
  Serial.print("\t");
  Serial.print(text1);
}

void getReadableTime() {
  unsigned long currentMillis;
  unsigned long seconds;
  unsigned long days;
  readableTime ="";
  currentMillis = GraphTime();
  seconds = currentMillis / 1000;
  minutes = (seconds / 60);
  hours = (minutes / 60);
  days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  
  if (days > 0) {
    readableTime = String(days) + " ";
  }
  if (hours > 0) {
    readableTime += String(hours) + ":";
  }
  if (minutes < 10) {
    readableTime += "0";
  }
  readableTime += String(minutes) + ":";
  if (seconds < 10) {
    readableTime += "0";
  }
  readableTime += String(seconds);
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

void readings() {
  BatCapacity = EEPROM.read(0);
  ChargeFactor = EEPROM.read(1);
  getReadableTime(), getServerUpTime();
  previousMillis = millis();
  ampsSum = 0, voltsMCUSum = 0, voltsSum = 0, counter = 0;
  previousMillis = millis();
  voltsCounter++;

  while ((millis() - previousMillis) < interval) {
    counter++;
    ampsSum += analogRead(Ampspin);
    voltsMCUSum += analogRead(voltsMCUPin);
    voltsSum += analogRead(voltsPin);
  }
  voltsMCU = ((voltsMCUSum / counter) * 0.0021 + 0.421);
  volts = (voltsSum / counter);
  Amps = ampsSum / counter;
  // Serial.print("Batt volts ADC = ");
  // Serial.print(volts);
  // Serial.print("  Volts MCU ADC= ");
  // Serial.print(voltsMCUSum / counter);
  // Serial.print("  Amps ADC= ");
  // Serial.println(Amps);
  volts = (volts * 0.0025 + 3.224) * 4.8;

  if (Amps > 4095 / 2) {
    ampsIn = Amps * 0.0346 - 63.214;
    ampsOut = 0;
  } else {
    ampsOut = abs(Amps * 0.0346 - 63.214);
    ampsIn = 0;
  }

  // demoMode();

  // *** DETERMINE THE MIN/MAX VOLTS AND THEIR RESPECTIVE TIME***
  if (volts < MinVolts) {
    MinVolts = volts;
    MinVoltsTime = readableTime;
  }
  if (volts > MaxVolts) {
    MaxVolts = volts;
    MaxVoltsTime = readableTime;
  }
  CycleTime = millis() - VoidCycleTime;
  VoidCycleTime = millis();
  volthOut = volthOut + volts;

  if (ampsIn >= cutOffLimit) {
    amphIn = amphIn + (ampsIn * (CycleTime / 3600000.00000000));  // 3,600,000 is 1 hr
    if (ampsIn > MaxampIn) {
      MaxampIn = ampsIn;
      MaxampInTime = readableTime;
    }
  } else {
    ampsIn = 0;
  }

  if (ampsOut >= cutOffLimit) {
    amphOut = amphOut + (ampsOut * (CycleTime / 3600000.00000000));
    if (ampsOut > MaxampOut) {
      MaxampOut = ampsOut;
      MaxampOutTime = readableTime;
    }
  } else if (ampsOut <= -cutOffLimit) {
    // Serial.print("Charging: ");
    ampsIn = ampsIn + fabs(ampsOut);
    // Serial.print(ampsIn);
    //  Serial.print("\t");
    amphIn = amphIn + (ampsIn * (CycleTime / 3600000.00000000));  // if a charger is attached, this is where it is accounted for
    // Serial.print(amphIn);
    // Serial.print("\t");
    // Serial.println(CycleTime);

    if (ampsIn > MaxampIn) {
      MaxampIn = ampsIn;
      MaxampInTime = readableTime;
    }
    ampsOut = 0.00;  //fundamentally, there is no amps coming in when it is negative.
  } else if (ampsOut > -cutOffLimit && ampsOut < cutOffLimit) {
    ampsOut = 0.00;
  }
  SOC = (BatCapacity + (amphIn * ChargeFactor / 100.00) - amphOut) / BatCapacity * 100;

  if (SOC > 115) {
    SOC = 115;
  }

  if (SOC > MaxSOC) {
    MaxSOC = SOC;
    MaxSOCTime = readableTime;
  } else if (SOC < MinSOC) {
    MinSOC = SOC;
    MinSOCTime = readableTime;
  }

  //15 minute totals
  if (millis() - minute15 > minute15const) {  // minute15 is the millis at startup and after each 15 minute cycle
    volthOut = volthOut / voltsCounter;
    interval15s = millis() / minute15const;  // getting the number of 15 minutes cycles since powered on
    float previousAmphIn = 0;
    float previousAmphOut = 0;
    float previousvolts = 0;
    if (interval15s > 288) {  //if there are 4 days worth of array data then prevent the array from overflowing
      interval15s = 288;
      for (int i = interval15s; i > 1; i--) {  // go through the array and shift along
        int x = i - 1;
        int y = i - 2;
        ampsIn15log[x] = ampsIn15log[y];
        ampsOut15log[x] = ampsOut15log[y];
        volts15log[x] = volts15log[y];
      }
    }
    if (interval15s > 1) {
      for (int i = 1; i <= interval15s; i++) {
        previousAmphIn = previousAmphIn + ampsIn15log[i - 1];
        previousAmphOut = previousAmphOut + ampsOut15log[i - 1];
      }
      ampsIn15log[interval15s - 1] = amphIn - previousAmphIn;
      ampsOut15log[interval15s - 1] = amphOut - previousAmphOut;
      volts15log[interval15s - 1] = volthOut;
    } else {
      ampsIn15log[interval15s - 1] = amphIn;
      ampsOut15log[interval15s - 1] = amphOut;
      volts15log[interval15s - 1] = volthOut;
    }
    Serial.println("AmpsIn  AmpsOut");
    for (int i = 1; i <= interval15s; i++) {
      Serial.print(ampsIn15log[i - 1]);
      Serial.print("\t");
      Serial.print(ampsOut15log[i - 1]);
      // Serial.print("\t");
      // Serial.println(volts15log[i-1]);
    }
    if (ampsOut15log[0] > ampsIn15log[0]) {
      Message = "DISCHARGING";
    } else if (ampsOut15log[0] < ampsIn15log[0]) {
      Message = "CHARGING";
    } else if (ampsOut15log[0] == ampsIn15log[0]) {
      Message = "STATIC";
    }
    minute15 = millis();
    volthOut = 0;
    voltsCounter = 0;
    if (millis() / hr2 >= 2) {
      previousAmphIn = 0;
      previousAmphOut = 0;
      for (int i = 1; i <= interval15s; i++) {
        previousAmphIn += ampsIn15log[i];
        previousAmphOut += ampsOut15log[i];
        volts2hr += volts15log[i];
      }
    }

    if (SOC < 90 && volts15 < 14.8) {
      Status = "BOOST...";
    } else if (SOC > 90 && volts15log[0] / hr2 > 14.8 && volts2hr / hr2 < 14.8) {
      Status = "ADSORPTION...";
    } else if (SOC > 90 && volts2hr / hr2 > 14.8) {
      Status = "FLOAT...";
    } else {
      Status = "INITIALISING...";
    }
  }

  counter2++;
  tempsum1 = tempsum1 + readTempData2();
  tempsum2 = tempsum2 + readTempData1();
  if (counter2 == 20) {
    avg1 = tempsum1 / counter2;
    avg2 = tempsum2 / counter2;
    counter2 = 0;
    tempsum1 = 0;
    tempsum2 = 0;
    dtostrf(avg1, 2, 0, Temp1);  // dtostrf(float_value, min_width, num_digits_after_decimal, where_to_store_string)
    dtostrf(avg2, 2, 0, Temp2);
    if (avg1 > FridgeMax) {  // Fridge temperature max/min
      FridgeMax = avg1;
      FridgeMaxTime = readableTime;
    }
    if (avg1 < FridgeMin) {
      FridgeMin = avg1;
      FridgeMinTime = readableTime;
    }

    if (avg2 > RadMax) {  // This is for the fridge radiator max/min
      RadMax = avg2;
      RadMaxTime = readableTime;
    }
    if (avg2 < RadMin) {
      RadMin = avg2;
      RadMinTime = readableTime;
    }
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
  Wire.begin();
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
    // Get the free flash memory space
  uint32_t freeSpace = ESP.getFreeSketchSpace();
  Serial.print("Free flash memory: ");
  Serial.print(freeSpace);
  Serial.println(" bytes");

  minute15 = millis();
  VoidCycleTime = millis();
  delay(300);  //These are so the serial window has time to display at startup.
  bool success = SPIFFS.begin();
  if (!success) {
    Serial.println("Error mounting the file system");
    return;
  }

  server.onNotFound([]() {                               // If the client requests any URI
    if (!handleFileRead(server.uri()))                   // send it if it exists
      server.send(404, "text/plain", "404: Not Found");  // otherwise, respond with a 404 (Not Found) error
  });
  EEPROM.begin(EEPROM_SIZE);
  BatCapacity = EEPROM.read(0);

  Serial.print("Configuring access point...");
  WiFi.softAP(ssid);
  Serial.println(WiFi.softAPIP() ? "Ready" : "Failed!");
  // IPAddress myIP = WiFi.softAPIP();
  // Serial.print("AP IP address: ");
  // Serial.println(myIP);
  MDNS.begin("power");
  server.on("/", handleRoot);
  server.on("/data", getData);
  server.on("/config", getConfig);
  server.on("/stats", getStats);
  server.on("/test", getTest);
  server.on("/graphdata", getGraphData);
  server.on("/graphdataAmpsIn", getGraphAmpsIn);
  server.on("/graphdataAmpsInCum", getGraphAmpsInCummulative);
  server.on("/graphdataAmpsOut", sendGraphAmpsOut);
  server.on("/graphdatavolts", sendGraphvolts);
  server.on("/graphdataAmpsOutCum", sendGraphAmpsOutCummulative);
  server.on("/CONFIG_page", handlePage2);
  server.on("/TEST_page", handleTestPage);
  server.on("/STATS_page", handlePage3);
  server.on("/GRAPH_page", handlePage4);
  server.on("/SaveConfigButton", SaveConFig);
  server.on("/BUTTON_RST", ProcessStatsRst);
  server.on("/RebootButton", SystemReboot);
  server.on("/SetClockButton", SetClock);
  // server.on("/UPDATE_page", LoadUpdatePage);
  ElegantOTA.begin(&server);    // Start ElegantOTA
  // server.on("/update", HTTP_POST, handleUpload);
  server.begin();
  Serial.println("HTTP server started");

  //  listDir(SPIFFS, "/", 0);  // this works, just don't want to see it anymore...
}

void loop() {
  // Serial.println("PPPOOOOOO!");
  unsigned long TempMillis = millis();
  server.handleClient();
  while (millis() - TempMillis < 200) {
    digitalWrite(LED, HIGH);  // turn the LED on (HIGH is the voltage level)
  }
  // server.handleClient();
  digitalWrite(LED, LOW);
  readings();
}

