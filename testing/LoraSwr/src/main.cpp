#include <Arduino.h>
#include <RadioLib.h>
/* LoraSwr 
Used to test the match between the SX1262 and its antenna.
*/

// ----- DEFINES ---- //

#define PRINTF(F, args...)  Serial.printf(F, args); Serial.println();
#define PRINT(MSG)          Serial.print(MSG);
#define PRINTLN(MSG)        Serial.println(MSG);

// petal lora pins
#define LORA_NSS 34
#define LORA_IRQ 38
#define LORA_NRST 48
#define LORA_BUSY 33
#define LORA_MOSI 35
#define LORA_MISO 37
#define LORA_SCK 36

// lora settings
#define LORA_START_FREQ 900.0 // MHz
#define LORA_BW   62.5        // kHz
#define LORA_SF   12          // 2^SF bits/symbol  
#define LORA_CR   8           // 4/8
#define LORA_PWR  10          // dBm
#define LORA_mA_LIMIT 60.0   // mA

#define BUTTON  14

// ----- GLOBALS --- //

SPIClass loraSPI(HSPI);
SX1262 radio = new Module( // lora radio
    LORA_NSS,
    LORA_IRQ,
    LORA_NRST,
    LORA_BUSY,
    loraSPI,
    SPISettings(2000000, MSBFIRST, SPI_MODE0));

volatile bool txDone;

// --- PROTOS ---- //
void onIrq(void);

// main
void setup() {
  delay(3000);  
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
  loraSPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  int status = radio.begin(LORA_START_FREQ, LORA_BW, LORA_SF, LORA_CR, 0x10, LORA_PWR);

  if (status != RADIOLIB_ERR_NONE) {
    PRINTF("Lora init failed! Code: %i", status);
    while(1);
  }
  radio.setCurrentLimit(LORA_mA_LIMIT);
  radio.setDio1Action(onIrq);
  radio.setDio2AsRfSwitch(true);
  radio.explicitHeader();
  radio.setCRC(2);
  PRINTLN("Radio configured successfully.");
  PRINTF("Starting Frequency: %.1f", LORA_START_FREQ);
  txDone = false;
}

void loop() {
  float freq = LORA_START_FREQ;
  String testBytes = "Hello";
  int txStatus = RADIOLIB_ERR_NONE;
  bool buttonEnable = true;
  while(true) {
    if (txDone) {
      txDone = false;
      PRINTLN("TX complete.")
      buttonEnable = true;  // re-enable the button
    }
    if (Serial.available() > 0 ) {
      String cmd = Serial.readString();
      if (cmd.equals("n")) {
        if (freq >= 928.0) {
          PRINTLN("Max frequency reached!");
        } else {
          freq += 1.0;
          int status = radio.setFrequency(freq);
          if (status == RADIOLIB_ERR_NONE)
          {
            PRINTF("Frequency changed to: %.1f MHz.", freq);
          }
          else
          {
            PRINTF("Frequency change to %.1f MHz failed; code: %i.", freq, status);
            freq -= 1.0;
            PRINTF("Reverting to %.1f MHz.", freq);
          }
        }
      }
      else if (cmd.equals("p")) {
        if (freq <= 900.0) {
          PRINTLN("Min frequency reached!");
        } else {
          freq -= 1.0;
          int status = radio.setFrequency(freq);
          if (status == RADIOLIB_ERR_NONE) {
            PRINTF("Frequency changed to: %.1f MHz.", freq);
          } else {
            PRINTF("Frequency change to %.1f MHz failed; code: %i.", freq, status);
            freq += 1.0;
            PRINTF("Reverting to %.1f MHz.", freq);
          }
        }
      }
      else {
        PRINTLN("Enter a valid command.");
        PRINTLN("Use 'p' to go to the previous frequency (-1.0 MHz).");
        PRINTLN("Use 'n' to go to the next frequency (+1.0 MHz).");
      }
    }
    if (!digitalRead(BUTTON) & buttonEnable) {
      delay(200); // debounce
      while (!digitalRead(BUTTON)); // wait for release
      delay(200);
      buttonEnable = false;
      txStatus = radio.startTransmit(testBytes);
      if (txStatus == RADIOLIB_ERR_NONE)
      {
        PRINTF("TX at %.1f MHz started...", freq);
      }
      else
      {
        PRINTF("TX at %.1f MHz failed; code: %i", freq, txStatus);
        buttonEnable = true;
      }
    }
  }
}

void onIrq(void) {
  txDone = true;
}