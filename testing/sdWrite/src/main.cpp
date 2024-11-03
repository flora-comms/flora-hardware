#include <Arduino.h>
#include <ArduinoJSON.h>
#include <SD.h>

/*
DEBUGGING

Comment out for debugging
*/
#define DEBUG

#ifdef DEBUG
#define SERIAL_BAUD 115200
#define DBG_PRINTLN(MSG) Serial.println(MSG)
#define DBG_PRINT(MSG) Serial.print(MSG)
#else
#define DBG_PRINTLN(MSG)
#endif

#define SD_CS 10   // SD chip select pin
#define SD_SCK 12  // SD sck pin
#define SD_MOSI 11 // SD mosi pin
#define SD_MISO 13 // SD miso

// objects
File file;
JsonDocument doc;

// Function prototypes
String JSONtoSTRING(String json);
void appendHistory(String fileName, String *Message);
void printFileContent(String fileName);

void setup() {
#ifdef DEBUG
  Serial.begin(SERIAL_BAUD);
  delay(1000); // Needed to add delay to give Serial Monitor time to connect,
               // can remove when not debugging
#endif

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  SPI.setDataMode(SPI_MODE0);

  DBG_PRINTLN("~~~~~~~~~~~~~~~~~~~DEGUB start~~~~~~~~~~~~~~~~~~~");

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    DBG_PRINTLN("Failed to mount SD card!");
    DBG_PRINTLN("");
    return;
  } else {
    DBG_PRINTLN("SD card mounted successfully.");
    DBG_PRINTLN("");
  }

  // create sample JSON object identical to what will recieved from client
  JsonObject history = doc.to<JsonObject>();
  history["payload"] = "Hello World!";
  history["nodeID"] = 5;
  history["SOS"] = 0;
  String mockMessage;
  serializeJson(history, mockMessage);
  DBG_PRINT("Unparsed JSON object: ");
  DBG_PRINTLN(mockMessage);
  DBG_PRINTLN("");
  // Convert JSON to "payload,nodeID,SOS"
  String parsedJSON = JSONtoSTRING(mockMessage);
  // Wrire to a new line in history.csv, pass the address of the parsed string
  appendHistory("/history.csv", &parsedJSON);
  // print out the entire file to serial monito
  printFileContent("/history.csv");
}

void loop() {}

//~~~~~~Function DEFS~~~~~~

String JSONtoSTRING(String json) {
  deserializeJson(doc, json);

  String payload = doc["payload"];
  int nodeID = doc["nodeID"];
  int SOS = doc["SOS"];

  String combinedString = payload + "," + String(nodeID) + "," + String(SOS);
  DBG_PRINT("Parsed to String: ");
  DBG_PRINTLN(combinedString);
  DBG_PRINTLN("");

  return combinedString;
}

void appendHistory(String fileName, String *message) // payload, nodeID, SOS
{
  // Write to file
  file = SD.open(fileName, FILE_APPEND);

  if (!file) {
    DBG_PRINTLN("Failed to open file for writing!");
    return;
  } else {
    DBG_PRINTLN("Writing to SD card...");
    DBG_PRINTLN("");
    file.println(*message);
    file.close();
  }
}

void printFileContent(String fileName) {
  file = SD.open(fileName, FILE_READ);
  // buffer to store the file content
  String fileContent = "";
  // stores conetent char by char into the buffer
  while (file.available()) {
    fileContent += (char)file.read();
  }
  // Print the entire buffer
  DBG_PRINT("contents of ");
  DBG_PRINT(fileName);
  DBG_PRINTLN(":");
  DBG_PRINTLN(fileContent);
  file.close();
}