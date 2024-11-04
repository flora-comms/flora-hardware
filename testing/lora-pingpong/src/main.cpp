#include <Arduino.h>

/*
  RadioLib SX126x Ping-Pong Example

  This example is intended to run on two SX126x radios,
  and send packets between the two.

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// uncomment the following only on one
// of the nodes to initiate the pings
#define INITIATING_NODE

//#define HELTEC
#define PETAL

// SX1262 Setup
#define LORA_FREQ 915.0 // MHz
#define LORA_BW 250.0   // kHz
#define LORA_SF 11
#define LORA_CR 5
#define LORA_SYNC 0x34
#define LORA_POWER 17  // dBm
#define LORA_PREAMB 16 // symbols

#ifdef PETAL
  #define NSS   34
  #define IRQ   39
  #define NRST  48
  #define BUSY  33
  #define MOSI  35
  #define MISO  37
  #define SCK   36
  SPIClass spi(HSPI);
  SPISettings spiSettings(2000000, MSBFIRST, SPI_MODE0);
  SX1262 radio = new Module(NSS, IRQ, NRST, BUSY, spi, spiSettings);
#endif

#ifdef HELTEC
  #define HEL_NSS 8
  #define HEL_DIO1 14
  #define HEL_NRST 12
  #define HEL_BUSY 13
  SX1262 radio = new Module(HEL_NSS, HEL_DIO1, HEL_NRST, HEL_BUSY);
#endif

  // or detect the pinout automatically using RadioBoards
  // https://github.com/radiolib-org/RadioBoards
  /*
  #define RADIO_BOARD_AUTO
  #include <RadioBoards.h>
  Radio radio = new RadioModule();
  */

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
  void setFlag(void)
  {
    // we sent or received a packet, set the flag
    operationDone = true;
}


void setup()
{
  delay(5000);
  #ifdef PETAL
    Serial.println("Beginning....");
    spi.begin(SCK, MISO, MOSI, NSS);
  #endif
  Serial.begin(9600);

  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin(LORA_FREQ, LORA_BW, LORA_SF, LORA_CR, LORA_SYNC, LORA_POWER, LORA_PREAMB);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("success!"));
  }
  else
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
    {
      delay(10);
    }
  }
  radio.setCurrentLimit(60.0);
  radio.setDio2AsRfSwitch(true);
  radio.explicitHeader();
  radio.setCRC(2);

  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(setFlag);

#if defined(INITIATING_NODE)
  // send the first packet on this node
  Serial.print(F("[SX1262] Sending first packet ... "));
  transmissionState = radio.startTransmit("Hello World!");
#else

  // start listening for LoRa packets on this node
  Serial.print(F("[SX1262] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("success!"));
  }
  else
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
    {
      delay(10);
    }
  }
#endif
}

void loop()
{
  // check if the previous operation finished
  if (operationDone)
  {
    // reset flag
    operationDone = false;

    if (transmitFlag)
    {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == 0)
      {

        // packet was successfully sent
        Serial.println(F("transmission finished!"));
      }
      else
      {
        Serial.print(F("failed, code "));
        Serial.println(transmissionState);
      }

      // listen for response
      radio.startReceive();
      transmitFlag = false;
    }
    else
    {
      // the previous operation was reception
      // print data and send another packet
      String str;
      int state = radio.readData(str);

      if (state == RADIOLIB_ERR_NONE)
      {
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
      delay(1000);

      // send another one
      Serial.print(F("[SX1262] Sending another packet ... "));
      transmissionState = radio.startTransmit("Hello World!");
      transmitFlag = true;
    }
  }
}