// SX1262 sleep tester
// Test the functionality of sleeping the esp32 module and waking from a
// recieved messasge fromt he semtech sx1662 module. Press USER_BTN to send a
// packet, The receving node will wake from sleep and blink the LED green three
// times

#include <Arduino.h>
#include <RadioLib.h>
#include <esp_sleep.h>

#define NSS 34
#define IRQ GPIO_NUM_38
#define NRST 48
#define BUSY 33
#define MOSI 35
#define MISO 37
#define SCK 36
#define USER_BTN GPIO_NUM_14

// Set up the RGB LED pins
#define R_PIN 17
#define G_PIN 18
#define B_PIN 16
#define R_CHANNEL 0
#define G_CHANNEL 1
#define B_CHANNEL 2
#define PWM_FREQ 12000
#define PWM_RES 8

#define TRANSMIT_DELAY 1000

SPIClass spi(HSPI);
SPISettings spiSettings(2000000, MSBFIRST, SPI_MODE0);
SX1262 radio = new Module(NSS, IRQ, NRST, BUSY, spi, spiSettings);

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

// this function is called when a complete packet
// is transmitted or received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif

void setFlag(void) {
  // we sent or received a packet, set the flag
  operationDone = true;
}

// SX1262 Setup
#define LORA_FREQ 915.0 // MHz
#define LORA_SYNC 0x34  // sync word
#define LORA_POWER 17   // tx power in dBm
#define LORA_PREAMB 16  // # of symbols in preamble
// long fast
#define LORA_BW 250.0 // kHz
#define LORA_SF 11
#define LORA_CR 5

// debounce variables
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // milliseconds
bool buttonState = false;
bool lastButtonState = false;

void sleep();
void ledController(int led_CHANNEL, int brightness, int blinks, int onTime_ms,
                   int offTime_ms);

void setup() {
  Serial.begin(115200);
  spi.begin(SCK, MISO, MOSI, NSS);

  // Setup RGB
  ledcSetup(R_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(R_PIN, R_CHANNEL);
  ledcSetup(G_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(G_PIN, G_CHANNEL);
  ledcSetup(B_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(B_PIN, B_CHANNEL);

  // pinMode(USER_BTN, INPUT_PULLUP);
  // gpio_wakeup_enable(USER_BTN, GPIO_INTR_LOW_LEVEL);

  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  // gpio_reset_pin((gpio_num_t)IRQ);
  int state =
      radio.begin(LORA_FREQ, LORA_BW, LORA_SF, LORA_CR, LORA_SYNC, LORA_POWER);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) {
      delay(10);
    }
  }

  // set the function that will be called
  // when new packet is received
  // radio.setDio1Action(setFlag);

  // start listening for LoRa packets on this node
  Serial.print(F("[SX1262] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) {
      delay(10);
    }
  }
}

void loop() {

  sleep();

  // Transmit on button press
  bool reading = digitalRead(USER_BTN) == LOW;
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState) {
        // Send a message
        Serial.print(F("[SX1262] Button pressed! Sending packet ... "));
        uint8_t bytes[] = {'i', 't', 's', ' ', 'c', 'a', 'm', '\0'};
        transmissionState = radio.startTransmit(bytes, 11);
        transmitFlag = true;
      }
    }
  }
  lastButtonState = reading;

  if (operationDone) {
    operationDone = false;
    if (transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == 0) {

        // packet was successfully sent
        Serial.println(F("transmission finished!"));
      } else {
        Serial.print(F("failed, code "));
        Serial.println(transmissionState);
      }

      // listen for response
      radio.startReceive();
      transmitFlag = false;
    } else {
      // the previous operation was reception
      // print data and send another packet
      String str;

      int state = radio.readData(str);

      if (state == RADIOLIB_ERR_NONE) {
        // packet was successfully received
        Serial.println(F("[SX1262] Received packet!"));

        // print data of the packet
        Serial.print(F("[SX1262] Data:\t\t"));
        Serial.println(str);

        // print RSSI (Received Signal Strength Indicator)
        Serial.print(F("[SX1262] RSSI:\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print SNR (Signal-to-Noise Ratio)
        Serial.print(F("[SX1262] SNR:\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));
      }

      // wait a second before transmitting again
      delay(TRANSMIT_DELAY);

      // send another one
      // Serial.print(F("[SX1262] Sending another packet ... "));
      // uint8_t bytes[] = {0xFF, 0x02, 0x00, 0x00, 'l', 'i',
      //                    'l',  'y',  'g',  'o',  '\0'};
      // transmissionState = radio.startTransmit(bytes, 11);
      transmitFlag = true;
    }
  }
}

void sleep() {
  radio.clearDio1Action();
  pinMode(IRQ, INPUT);
  gpio_wakeup_enable(IRQ, GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  Serial.println("GOING TO SLEEP");

  esp_light_sleep_start();

  Serial.println("WAKE UP");
  ledController(G_CHANNEL, 80, 3, 400, 250);
  operationDone = true;
  return;
}

void ledController(int led_CHANNEL, int brightness, int blinks, int onTime_ms,
                   int offTime_ms) {
  for (int i = 0; i < blinks; i++) {
    ledcWrite(led_CHANNEL, brightness);
    delay(onTime_ms);
    ledcWrite(led_CHANNEL, 0);
    delay(offTime_ms);
  }
}