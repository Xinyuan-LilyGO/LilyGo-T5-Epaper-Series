/**
 * @file      main.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  深圳市芯元电子科技有限公司
 * @date      2022-07-21
 * @note      Example using nRF2401 to send data to control T-Relay
 */


#define LILYGO_T5_V102                                  //Using TTGO MINI E-Paper 1.02" epaper

#define TIME_TO_SLEPP_MINUTE        10                  //Sleep time unit : minutes


#include <boards.h>
#include <GxEPD.h>                      //https://github.com/lewisxhe/GxEPD
#include <U8g2_for_Adafruit_GFX.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <RadioLib.h>                   //https://github.com/jgromes/RadioLib
#include <AceButton.h>                  //https://github.com/bxparks/AceButton
#include <SD.h>
#include <FS.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w 


using namespace ace_button;

void setupDisplay();
void updateItem(bool fullScreen, uint8_t item, uint8_t select);

AceButton buttons[BUTTON_COUNT];
const uint8_t button_pin[BUTTON_COUNT] = BUTTONS;       //{36,39,0}
SPIClass SPI2(HSPI);                                    //Sdcard using hspi


// Radio shiled pins
#define RADIO_MOSI                (23)
#define RADIO_MISO                (38)
#define RADIO_SCLK                (26)
#define RADIO_CS                  (25)
#define RADIO_IRQ                 (37)
#define RADIO_CE                  (12)

#define BOARD_SDA                 (18)
#define BOARD_SCL                 (33)

// symbol
#define RELAY_SYMOBL_CONNECT            "+"
#define RELAY_SYMOBL_DISCONNECT         "-"

typedef struct {
    const char *name;
    uint16_t x;
    uint16_t y;
} MyMenu_t;

MyMenu_t menu[4] = {
    {"Num 0", 0, 0},
    {"Num 1", 0, 0},
    {"Num 2", 0, 0},
    {"Num 3", 0, 0},
} ;


nRF24           radio = new Module(RADIO_CS, RADIO_IRQ, RADIO_CE);
GxIO_Class      io(EPD_SCLK, EPD_MISO, EPD_MOSI, EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class     display(io, EPD_RSET, EPD_BUSY);


const uint8_t menuSize          = sizeof(menu) / sizeof(menu[0]);
static uint8_t addr[]           = {0x00, 0x23, 0x45, 0x67, 0x89};         //RF configuration address
static uint8_t cItem            = 0;
static uint8_t selectItem       = 0;


bool setupRadio()
{
    SPI.begin(RADIO_SCLK, RADIO_MISO, RADIO_MOSI);

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
    // set transmit address
    // NOTE: address width in bytes MUST be equal to the
    //       width set in begin() or setAddressWidth()
    //       methods (5 by default)
    // byte addr[] = {0x01, 0x23, 0x45, 0x67, 0x89};
    Serial.print(F("[nRF24] Setting transmit pipe ... "));
    state = radio.setTransmitPipe(addr);
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        return false;
    }

    return true;
}

void transmit(uint8_t item, uint8_t on)
{
    Serial.printf("[nRF24] Transmitting packet ... Item:%u On:%u\n", item, on);

    const uint8_t transmitBytes = 3;

    uint8_t pipeAddress = 0;

    uint8_t buffer[transmitBytes] = {addr[0], item, on};

    int state = radio.startTransmit(buffer, transmitBytes, pipeAddress);
    if (state == RADIOLIB_ERR_NONE) {
        // the packet was successfully transmitted
        Serial.println(F("success!"));

    } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
        // the supplied packet was longer than 32 bytes
        Serial.println(F("too long!"));

    } else if (state == RADIOLIB_ERR_ACK_NOT_RECEIVED) {
        // acknowledge from destination module
        // was not received within 15 retries
        Serial.println(F("ACK not received!"));

    } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
        // timed out while transmitting
        Serial.println(F("timeout!"));
    } else {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }
}

// The event handler for the button.
void buttonHandleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
    uint8_t id = button->getId();
    Serial.print(F("handleEvent(): eventType: "));
    Serial.print(eventType);
    Serial.print(F("; buttonState: "));
    Serial.print(buttonState);
    Serial.print(" ID:");
    Serial.println(id);
    switch (id) {
    case 0:
        if (AceButton::kEventReleased == eventType ) {
            if (cItem < menuSize) {
                cItem++;
            }
            updateItem(0, cItem, selectItem);
        }
        break;
    case 1:
        if (AceButton::kEventReleased == eventType ) {
            if (cItem) {
                cItem--;
            }
            updateItem(0, cItem, selectItem);
        }
        break;
    case 2:
        if (AceButton::kEventReleased == eventType ) {
            Serial.printf("update ...:%d\n", cItem);
            if (selectItem & _BV(cItem)) {
                selectItem &= (~_BV(cItem));
                Serial.println("--");
            } else {
                selectItem |= _BV(cItem);
                Serial.println("++");
            }
            updateItem(0, cItem, selectItem);
            transmit(cItem, selectItem & _BV(cItem));
        }
        break;
    default:
        break;
    }
}

void loopButton()
{
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].check();
    }
}

void setupButton()
{
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        // initialize built-in LED as an output
        pinMode(button_pin[i], OUTPUT);
        // Button uses the built-in pull up register.
        pinMode(button_pin[i], INPUT_PULLUP);
        // initialize the corresponding AceButton
        buttons[i].init(button_pin[i], HIGH, i);
    }

    // Configure the ButtonConfig with the event handler, and enable all higher
    // level events.
    ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(buttonHandleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
}

void setupDisplay()
{
#if defined(LILYGO_T5_V102)  || defined(LILYGO_EPD_DISPLAY_102)
    pinMode(EPD_POWER_ENABLE, OUTPUT);
    digitalWrite(EPD_POWER_ENABLE, HIGH);
#endif /*LILYGO_EPD_DISPLAY_102*/

    display.init(); // enable diagnostic output on Serial
    display.setRotation(1);
}


void sleep()
{
    /**
    *  Operating power consumption is about ~10mA,
    *  sleep current is about ~22uA
    */

    radio.sleep();

    SPI.end();

    Wire.end();

#if defined(LILYGO_T5_V102)  || defined(LILYGO_EPD_DISPLAY_102)
    digitalWrite(EPD_POWER_ENABLE, LOW);
    pinMode(EPD_POWER_ENABLE, INPUT);
#endif /*LILYGO_EPD_DISPLAY_102*/

    // esp_sleep_enable_ext1_wakeup(BUTTON_3, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEPP_MINUTE * 60000000);
    esp_deep_sleep_start();
}

bool setupSDCard()
{
    pinMode(SDCARD_MISO, INPUT_PULLUP);
    SPI2.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);
    if (!SD.begin(SDCARD_CS, SPI2)) {
        Serial.println("SD begin failed!");
        return false;
    }
    return true;
}

void setup()
{
    /**
     *  Update the screen and transmit wireless, do not need such a high frequency,
     *  reducing the frequency can significantly reduce power consumption
     */
    setCpuFrequencyMhz(20);

    Serial.begin(115200);

    setupButton();

    setupDisplay();

    bool rRes =  setupRadio();

    display.setRotation(0);

    updateItem(1, 0, 0);
}

void loop()
{
    loopButton();
}

void updateItem(bool fullScreen, uint8_t item, uint8_t select)
{
    static uint16_t prevLasty, prevLastx;
    static uint8_t prevItem = 0;
    int16_t tbx, tby; uint16_t tbw, tbh;
    uint16_t x  = 0;
    uint16_t y  = 1;
    const uint16_t box_w = 80;
    const uint16_t box_h = 20;
    uint16_t box_x =  ((display.width() - box_w) / 2);
    uint16_t box_y = 0;
    String text;

    if (item >= menuSize) {
        item = 0;
    }
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
    if (fullScreen) {
        do {
            display.fillScreen(GxEPD_WHITE);
            for (int i = 0; i < menuSize; i++) {
                if (select & _BV(i)) {
                    text = RELAY_SYMOBL_CONNECT;
                } else {
                    text = RELAY_SYMOBL_DISCONNECT;
                }
                text.concat(menu[i].name);
                display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
                x = ((box_w - tbw) / 2) - tbx;
                y = ((box_h - tbh) / 2) - tby ;
                x += box_x;
                y += box_y;

                display.setCursor(x, y);
                display.print(text);
                menu[i].x = box_x;
                menu[i].y = box_y;
                display.drawRoundRect(box_x, box_y, box_w, box_h, 3, GxEPD_BLACK);
                box_y += box_h + 6;
            }
        } while (0);

        prevLasty = menu[0].y;
        prevLastx = menu[0].x;

        display.println();
        display.setFont(&FreeSerifBold12pt7b);
        text  = "LilyGo";
        display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((box_w - tbw) / 2) - tbx;
        display.setCursor(x, display.getCursorY() + 5);
        display.print(text);
        display.update();

    } else {
        display.setTextColor(GxEPD_BLACK);
        do {
            display.fillRoundRect(menu[prevItem].x, menu[prevItem].y, box_w, box_h, 3, GxEPD_WHITE);
            if (select & _BV(prevItem)) {
                text = RELAY_SYMOBL_CONNECT;
            } else {
                text = RELAY_SYMOBL_DISCONNECT;
            }
            text.concat(menu[prevItem].name);
            display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
            x = ((box_w - tbw) / 2) - tbx;
            y = ((box_h - tbh) / 2) - tby ;
            x += menu[prevItem].x;
            y += menu[prevItem].y;

            display.setCursor(x, y);
            display.print(text);
            display.drawRoundRect(menu[prevItem].x, menu[prevItem].y, box_w, box_h, 3, GxEPD_BLACK);
            display.updateWindow(menu[prevItem].x, menu[prevItem].y, box_w, box_h, true);
        } while (0);


        display.setTextColor(GxEPD_WHITE);
        do {
            if (select & _BV(item)) {
                text = RELAY_SYMOBL_CONNECT;
            } else {
                text = RELAY_SYMOBL_DISCONNECT;
            }
            text.concat(menu[item].name);
            display.fillRoundRect(menu[item].x, menu[item].y, box_w, box_h, 3, GxEPD_BLACK);
            display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
            x = ((box_w - tbw) / 2) - tbx;
            y = ((box_h - tbh) / 2) - tby ;
            x += menu[item].x;
            y += menu[item].y;
            display.setCursor(x, y);
            display.print(text);

            display.updateWindow(menu[item].x, menu[item].y, box_w, box_h, true);
        } while (0);

        prevLasty = menu[item].y;
        prevLastx = menu[item].x;
        prevItem = item;

    }
}