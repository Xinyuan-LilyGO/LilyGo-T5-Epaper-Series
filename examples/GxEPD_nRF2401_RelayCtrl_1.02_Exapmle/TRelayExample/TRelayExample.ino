/**
 * @file      main.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  深圳市芯元电子科技有限公司
 * @date      2022-06-13
 * @note      The sample program uses T-Relay with nRF2401 module
 */

#include <Arduino.h>
#include <RadioLib.h>


#define RADIO_MOSI                  (15)
#define RADIO_MISO                  (4)
#define RADIO_SCLK                  (14)
#define RADIO_CS                    (12)
#define RADIO_IRQ                   (32)
#define RADIO_CE                    (2)

#define RELAY_PIN_1                 (21)
#define RELAY_PIN_2                 (19)
#define RELAY_PIN_3                 (18)
#define RELAY_PIN_4                 (5)
#define LED_PIN                     (25)

nRF24 radio = new Module(RADIO_CS, RADIO_IRQ, RADIO_CE);


const uint8_t relayPins[] = {
    RELAY_PIN_1,
    RELAY_PIN_2,
    RELAY_PIN_3,
    RELAY_PIN_4,
    LED_PIN,
};

const uint8_t relayPinsCount = sizeof(relayPins) / sizeof(relayPins[0]);

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
ICACHE_RAM_ATTR void setFlag(void)
{
    // check if the interrupt is enabled
    if (!enableInterrupt) {
        return;
    }

    // we got a packet, set the flag
    receivedFlag = true;
}

bool setupRadio()
{
    SPI.begin(RADIO_SCLK, RADIO_MISO, RADIO_MOSI, RADIO_CS);

    // initialize nRF24 with default settings
    Serial.print(F("[nRF24] Initializing ... "));
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        return false;
    }

    attachInterrupt(RADIO_IRQ, setFlag, FALLING);

    // set receive pipe 0 address
    // NOTE: address width in bytes MUST be equal to the
    //       width set in begin() or setAddressWidth()
    //       methods (5 by default)
    Serial.print(F("[nRF24] Setting address for receive pipe  "));
    uint8_t addr[] = {0x00, 0x23, 0x45, 0x67, 0x89};
    state = radio.setReceivePipe(0, addr);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print(F("0 failed, code "));
        Serial.println(state);
    }

    radio.startReceive();

    return true;
}


void loopRadio()
{
    // check if the flag is set
    if (receivedFlag) {
        // disable the interrupt service routine while
        // processing the data
        enableInterrupt = false;

        // reset flag
        receivedFlag = false;

        uint8_t data[3] = {0};
        int state = radio.readData(data, 3);

        if (state == RADIOLIB_ERR_NONE) {
            // packet was successfully received
            Serial.println(F("[nRF24] Received packet!"));
            // print data of the packet
            Serial.print(F("[nRF24] Data:\t\t"));
            Serial.print("Addr:"); Serial.print(data[0]);
            Serial.print(" Num:"); Serial.print(data[1]);
            Serial.print(" Level:"); Serial.println(data[2]);

            uint8_t index = data[1];
            bool level = data[2];

            if (index <= relayPinsCount) {
                digitalWrite(relayPins[index], level);
            }

        } else {
            // some other error occurred
            Serial.print(F("[nRF24] Failed, code "));
            Serial.println(state);

        }

        // put module back to listen mode
        radio.startReceive();

        // we're ready to receive more packets,
        // enable interrupt service routine
        enableInterrupt = true;
    }

}

void setup()
{
    Serial.begin(115200);

    setupRadio();

    for (int i = 0; i < relayPinsCount; ++i) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], LOW);
    }
}

void loop()
{
    loopRadio();
}
