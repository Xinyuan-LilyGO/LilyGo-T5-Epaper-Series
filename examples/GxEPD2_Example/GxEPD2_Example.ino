// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Display ePaper for Arduino: https://forum.arduino.cc/index.php?topic=436411.0

// see GxEPD2_wiring_examples.h for wiring suggestions and examples

// NOTE for use with Waveshare ESP32 Driver Board:
// **** also need to select the constructor with the parameters for this board in GxEPD2_display_selection_new_style.h ****
//
// The Wavehare ESP32 Driver Board uses uncommon SPI pins for the FPC connector. It uses HSPI pins, but SCK and MOSI are swapped.
// To use HW SPI with the ESP32 Driver Board, HW SPI pins need be re-mapped in any case. Can be done using either HSPI or VSPI.
// Other SPI clients can either be connected to the same SPI bus as the e-paper, or to the other HW SPI bus, or through SW SPI.
// The logical configuration would be to use the e-paper connection on HSPI with re-mapped pins, and use VSPI for other SPI clients.
// VSPI with standard VSPI pins is used by the global SPI instance of the Arduino IDE ESP32 package.

// uncomment next line to use HSPI for EPD (and e.g VSPI for SD), e.g. with Waveshare ESP32 Driver Board
//#define USE_HSPI_FOR_EPD

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>
// Note: if you use this with ENABLE_GxEPD2_GFX 1:
//       uncomment it in GxEPD2_GFX.h too, or add #include <GFX.h> before any #include <GxEPD2_GFX.h>


// https://github.com/ZinggJM/GxEPD2.git
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "BitmapExamples.h"

#define EPD_MOSI                (23)
#define EPD_MISO                (-1) //elink no use
#define EPD_SCLK                (18)

#define EPD_BUSY                (4)
#define EPD_RSET                (16)
#define EPD_DC                  (17)
#define EPD_CS                  (5)

#define SDCARD_CS               (13)
#define SDCARD_MOSI             (15)
#define SDCARD_MISO             (2)
#define SDCARD_SCLK             (14)

#define BUTTON_1                (37)
#define BUTTON_2                (38)
#define BUTTON_3                (39)

#define SPK_POWER_EN            (19)

#define BUTTONS                 {37,38,39}
#define BUTTON_COUNT            (3)

#define LED_PIN                 (26)
#define LED_ON                  (LOW)

#define ADC_PIN                 (35)

#define SPERKER_PIN             (25)

#define LEDC_CHANNEL_0          (0)
#define _HAS_ADC_DETECTED_
#define _HAS_LED_
#define _HAS_SPEAKER_
#define _BUILTIN_DAC_
#define _HAS_SDCARD_
#define _HAS_PWR_CTRL_

// select the display constructor line in one of the following files (old style):
// #include "GxEPD2_display_selection.h"
// #include "GxEPD2_display_selection_added.h"

// // or select the display class and display driver class in the following file (new style):
// #include "GxEPD2_display_selection_new_style.h"
GxEPD2_BW<GxEPD2_290_GDEY029T94, GxEPD2_290_GDEY029T94::HEIGHT>
display(GxEPD2_290_GDEY029T94(/*CS=5*/ EPD_CS, /*DC=*/ EPD_DC, /*RST=*/ EPD_RSET, /*BUSY=*/ EPD_BUSY)); // GDEY029T94  128x296, SSD1680, (FPC-A005 20.06.15)


#include <SD.h>
#include <FS.h>
#include <WiFi.h>


#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
SPIClass SDSPI(VSPI);
#endif

bool setupSDCard(void)
{
#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
    SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);
    return SD.begin(SDCARD_CS, SDSPI);
#elif defined(_HAS_SDCARD_)
    return SD.begin(SDCARD_CS);
#endif
    return false;
}


void testSpeaker()
{
#if defined(_HAS_SPEAKER_)
#ifdef _HAS_PWR_CTRL_
    pinMode(SPK_POWER_EN, OUTPUT);
    digitalWrite(SPK_POWER_EN, HIGH);
#endif
    ledcSetup(LEDC_CHANNEL_0, 1000, 8);
    ledcAttachPin(SPERKER_PIN, LEDC_CHANNEL_0);
    int i = 3;
    while (i--) {
        ledcWriteTone(LEDC_CHANNEL_0, 1000);
        delay(200);
        ledcWriteTone(LEDC_CHANNEL_0, 0);
    }
#ifdef _HAS_PWR_CTRL_
    pinMode(SPK_POWER_EN, INPUT);
#endif
    ledcDetachPin(SPERKER_PIN);
#endif
}


void testWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();

    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("");
}


void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");
    delay(100);

    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);

    //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
    display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    //display.init(115200, true, 10, false, SPI0, SPISettings(4000000, MSBFIRST, SPI_MODE0)); // extended init method with SPI channel and/or settings selection
    // first update should be full refresh
    testSpeaker();

    if (setupSDCard()) {
        uint8_t cardType = SD.cardType();

        if (cardType == CARD_NONE) {
            Serial.println("No SD card attached");
            return;
        }

        Serial.print("SD Card Type: ");
        if (cardType == CARD_MMC) {
            Serial.println("MMC");
        } else if (cardType == CARD_SD) {
            Serial.println("SDSC");
        } else if (cardType == CARD_SDHC) {
            Serial.println("SDHC");
        } else {
            Serial.println("UNKNOWN");
        }

        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.printf("SD Card Size: %lluMB\n", cardSize);
    } else {
        Serial.println("Card mount failed!");
    }


    testWiFi();

    helloWorld();
    delay(1000);
    // partial refresh mode can be used to full screen,
    // effective if display panel hasFastPartialUpdate
    helloFullScreenPartialMode();
    delay(1000);
    //stripeTest(); return; // GDEH029Z13 issue
    helloArduino();
    delay(1000);
    helloEpaper();
    delay(1000);
    //helloValue(123.9, 1);
    //delay(1000);
    showFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
    delay(1000);
    if (display.epd2.WIDTH < 104) {
        showFont("glcdfont", 0);
        delay(1000);
    }
    drawGraphics();

    drawBitmaps();

    if (display.epd2.hasPartialUpdate) {
        showPartialUpdate();
        delay(1000);
    } // else // on GDEW0154Z04 only full update available, doesn't look nice
    //drawCornerTest();
    //showBox(16, 16, 48, 32, false);
    //showBox(16, 56, 48, 32, true);
    display.powerOff();
    deepSleepTest();
    Serial.println("setup done");
}

void loop()
{
}

// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

const char HelloWorld[] = "Hello World!";
const char HelloArduino[] = "Hello Arduino!";
const char HelloEpaper[] = "Hello E-Paper!";

void helloWorld()
{
    //Serial.println("helloWorld");
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center bounding box by transposition of origin:
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(HelloWorld);
    } while (display.nextPage());
    //Serial.println("helloWorld done");
}

void helloWorldForDummies()
{
    //Serial.println("helloWorld");
    const char text[] = "Hello World!";
    // most e-papers have width < height (portrait) as native orientation, especially the small ones
    // in GxEPD2 rotation 0 is used for native orientation (most TFT libraries use 0 fix for portrait orientation)
    // set rotation to 1 (rotate right 90 degrees) to have enough space on small displays (landscape)
    display.setRotation(1);
    // select a suitable font in Adafruit_GFX
    display.setFont(&FreeMonoBold9pt7b);
    // on e-papers black on white is more pleasant to read
    display.setTextColor(GxEPD_BLACK);
    // Adafruit_GFX has a handy method getTextBounds() to determine the boundary box for a text for the actual font
    int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
    display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh); // it works for origin 0, 0, fortunately (negative tby!)
    // center bounding box by transposition of origin:
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    // full window mode is the initial mode, set it anyway
    display.setFullWindow();
    // here we use paged drawing, even if the processor has enough RAM for full buffer
    // so this can be used with any supported processor board.
    // the cost in code overhead and execution time penalty is marginal
    // tell the graphics class to use paged drawing mode
    display.firstPage();
    do {
        // this part of code is executed multiple times, as many as needed,
        // in case of full buffer it is executed once
        // IMPORTANT: each iteration needs to draw the same, to avoid strange effects
        // use a copy of values that might change, don't read e.g. from analog or pins in the loop!
        display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
        display.setCursor(x, y); // set the postition to start printing text
        display.print(text); // print some text
        // end of part executed multiple times
    }
    // tell the graphics class to transfer the buffer content (page) to the controller buffer
    // the graphics class will command the controller to refresh to the screen when the last page has been transferred
    // returns true if more pages need be drawn and transferred
    // returns false if the last page has been transferred and the screen refreshed for panels without fast partial update
    // returns false for panels with fast partial update when the controller buffer has been written once more, to make the differential buffers equal
    // (for full buffered with fast partial update the (full) buffer is just transferred again, and false returned)
    while (display.nextPage());
    //Serial.println("helloWorld done");
}

void helloFullScreenPartialMode()
{
    //Serial.println("helloFullScreenPartialMode");
    const char fullscreen[] = "full screen update";
    const char fpm[] = "fast partial mode";
    const char spm[] = "slow partial mode";
    const char npm[] = "no partial mode";
    display.setPartialWindow(0, 0, display.width(), display.height());
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(GxEPD_BLACK);
    const char *updatemode;
    if (display.epd2.hasFastPartialUpdate) {
        updatemode = fpm;
    } else if (display.epd2.hasPartialUpdate) {
        updatemode = spm;
    } else {
        updatemode = npm;
    }
    // do this outside of the loop
    int16_t tbx, tby; uint16_t tbw, tbh;
    // center update text
    display.getTextBounds(fullscreen, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t utx = ((display.width() - tbw) / 2) - tbx;
    uint16_t uty = ((display.height() / 4) - tbh / 2) - tby;
    // center update mode
    display.getTextBounds(updatemode, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t umx = ((display.width() - tbw) / 2) - tbx;
    uint16_t umy = ((display.height() * 3 / 4) - tbh / 2) - tby;
    // center HelloWorld
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t hwx = ((display.width() - tbw) / 2) - tbx;
    uint16_t hwy = ((display.height() - tbh) / 2) - tby;
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(hwx, hwy);
        display.print(HelloWorld);
        display.setCursor(utx, uty);
        display.print(fullscreen);
        display.setCursor(umx, umy);
        display.print(updatemode);
    } while (display.nextPage());
    //Serial.println("helloFullScreenPartialMode done");
}

void helloArduino()
{
    //Serial.println("helloArduino");
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    // align with centered HelloWorld
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    // height might be different
    display.getTextBounds(HelloArduino, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t y = ((display.height() / 4) - tbh / 2) - tby; // y is base line!
    // make the window big enough to cover (overwrite) descenders of previous text
    uint16_t wh = FreeMonoBold9pt7b.yAdvance;
    uint16_t wy = (display.height() / 4) - wh / 2;
    display.setPartialWindow(0, wy, display.width(), wh);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        //display.drawRect(x, y - tbh, tbw, tbh, GxEPD_BLACK);
        display.setCursor(x, y);
        display.print(HelloArduino);
    } while (display.nextPage());
    delay(1000);
    //Serial.println("helloArduino done");
}

void helloEpaper()
{
    //Serial.println("helloEpaper");
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    // align with centered HelloWorld
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    // height might be different
    display.getTextBounds(HelloEpaper, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t y = ((display.height() * 3 / 4) - tbh / 2) - tby; // y is base line!
    // make the window big enough to cover (overwrite) descenders of previous text
    uint16_t wh = FreeMonoBold9pt7b.yAdvance;
    uint16_t wy = (display.height() * 3 / 4) - wh / 2;
    display.setPartialWindow(0, wy, display.width(), wh);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(HelloEpaper);
    } while (display.nextPage());
    //Serial.println("helloEpaper done");
}

// test partial window issue on GDEW0213Z19 and GDEH029Z13
void stripeTest()
{
    helloStripe(104);
    delay(2000);
    helloStripe(96);
}

const char HelloStripe[] = "Hello Stripe!";

void helloStripe(uint16_t pw_xe) // end of partial window in physcal x direction
{
    //Serial.print("HelloStripe("); Serial.print(pw_xe); Serial.println(")");
    display.setRotation(3);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(HelloStripe, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t wh = FreeMonoBold9pt7b.yAdvance;
    uint16_t wy = pw_xe - wh;
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = wy - tby;
    display.setPartialWindow(0, wy, display.width(), wh);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(HelloStripe);
    } while (display.nextPage());
    //Serial.println("HelloStripe done");
}

#if defined(ESP8266) || defined(ESP32)
#include <StreamString.h>
#define PrintString StreamString
#else
class PrintString : public Print, public String
{
public:
    size_t write(uint8_t data) override
    {
        return concat(char(data));
    };
};
#endif

void helloValue(double v, int digits)
{
    //Serial.println("helloValue");
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    PrintString valueString;
    valueString.print(v, digits);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(valueString, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() * 3 / 4) - tbh / 2) - tby; // y is base line!
    // show what happens, if we use the bounding box for partial window
    uint16_t wx = (display.width() - tbw) / 2;
    uint16_t wy = ((display.height() * 3 / 4) - tbh / 2);
    display.setPartialWindow(wx, wy, tbw, tbh);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(valueString);
    } while (display.nextPage());
    delay(2000);
    // make the partial window big enough to cover the previous text
    uint16_t ww = tbw; // remember window width
    display.getTextBounds(HelloEpaper, 0, 0, &tbx, &tby, &tbw, &tbh);
    // adjust, because HelloEpaper was aligned, not centered (could calculate this to be precise)
    ww = max(ww, uint16_t(tbw + 12)); // 12 seems ok
    wx = (display.width() - tbw) / 2;
    // make the window big enough to cover (overwrite) descenders of previous text
    uint16_t wh = FreeMonoBold9pt7b.yAdvance;
    wy = (display.height() * 3 / 4) - wh / 2;
    display.setPartialWindow(wx, wy, ww, wh);
    // alternately use the whole width for partial window
    //display.setPartialWindow(0, wy, display.width(), wh);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(valueString);
    } while (display.nextPage());
    //Serial.println("helloValue done");
}

void deepSleepTest()
{
    //Serial.println("deepSleepTest");
    const char hibernating[] = "hibernating ...";
    const char wokeup[] = "woke up";
    const char from[] = "from deep sleep";
    const char again[] = "again";
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    // center text
    display.getTextBounds(hibernating, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(hibernating);
    } while (display.nextPage());
    display.hibernate();
    delay(5000);
    display.getTextBounds(wokeup, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t wx = (display.width() - tbw) / 2;
    uint16_t wy = ((display.height() / 3) - tbh / 2) - tby; // y is base line!
    display.getTextBounds(from, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t fx = (display.width() - tbw) / 2;
    uint16_t fy = ((display.height() * 2 / 3) - tbh / 2) - tby; // y is base line!
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(wx, wy);
        display.print(wokeup);
        display.setCursor(fx, fy);
        display.print(from);
    } while (display.nextPage());
    delay(5000);
    display.getTextBounds(hibernating, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t hx = (display.width() - tbw) / 2;
    uint16_t hy = ((display.height() / 3) - tbh / 2) - tby; // y is base line!
    display.getTextBounds(again, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t ax = (display.width() - tbw) / 2;
    uint16_t ay = ((display.height() * 2 / 3) - tbh / 2) - tby; // y is base line!
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(hx, hy);
        display.print(hibernating);
        display.setCursor(ax, ay);
        display.print(again);
    } while (display.nextPage());
    display.hibernate();
    //Serial.println("deepSleepTest done");
}

void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial)
{
    //Serial.println("showBox");
    display.setRotation(1);
    if (partial) {
        display.setPartialWindow(x, y, w, h);
    } else {
        display.setFullWindow();
    }
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.fillRect(x, y, w, h, GxEPD_BLACK);
    } while (display.nextPage());
    //Serial.println("showBox done");
}

void drawCornerTest()
{
    display.setFullWindow();
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    for (uint16_t r = 0; r <= 4; r++) {
        display.setRotation(r);
        display.firstPage();
        do {
            display.fillScreen(GxEPD_WHITE);
            display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
            display.fillRect(display.width() - 18, 0, 16, 16, GxEPD_BLACK);
            display.fillRect(display.width() - 25, display.height() - 25, 24, 24, GxEPD_BLACK);
            display.fillRect(0, display.height() - 33, 32, 32, GxEPD_BLACK);
            display.setCursor(display.width() / 2, display.height() / 2);
            display.print(display.getRotation());
        } while (display.nextPage());
        delay(2000);
    }
}

void showFont(const char name[], const GFXfont *f)
{
    display.setFullWindow();
    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);
    display.firstPage();
    do {
        drawFont(name, f);
    } while (display.nextPage());
}

void drawFont(const char name[], const GFXfont *f)
{
    //display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(f);
    display.setCursor(0, 0);
    display.println();
    display.println(name);
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
    if (display.epd2.hasColor) {
        display.setTextColor(GxEPD_RED);
    }
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
}

// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()
// showPartialUpdate() purposely uses values that are not multiples of 8 to test this

void showPartialUpdate()
{
    // some useful background
    helloWorld();
    // use asymmetric values for test
    uint16_t box_x = 10;
    uint16_t box_y = 15;
    uint16_t box_w = 70;
    uint16_t box_h = 20;
    uint16_t cursor_y = box_y + box_h - 6;
    if (display.epd2.WIDTH < 104) cursor_y = box_y + 6;
    float value = 13.95;
    uint16_t incr = display.epd2.hasFastPartialUpdate ? 1 : 3;
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(GxEPD_BLACK);
    // show where the update box is
    for (uint16_t r = 0; r < 4; r++) {
        display.setRotation(r);
        display.setPartialWindow(box_x, box_y, box_w, box_h);
        display.firstPage();
        do {
            display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
            //display.fillScreen(GxEPD_BLACK);
        } while (display.nextPage());
        delay(2000);
        display.firstPage();
        do {
            display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        } while (display.nextPage());
        delay(1000);
    }
    //return;
    // show updates in the update box
    for (uint16_t r = 0; r < 4; r++) {
        display.setRotation(r);
        display.setPartialWindow(box_x, box_y, box_w, box_h);
        for (uint16_t i = 1; i <= 10; i += incr) {
            display.firstPage();
            do {
                display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
                display.setCursor(box_x, cursor_y);
                display.print(value * i, 2);
            } while (display.nextPage());
            delay(500);
        }
        delay(1000);
        display.firstPage();
        do {
            display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        } while (display.nextPage());
        delay(1000);
    }
}


void drawBitmaps()
{
    const unsigned char *bitmaps[] = {
        BitmapExample1
    };
    if ((display.epd2.WIDTH == 128) && (display.epd2.HEIGHT == 296) && !display.epd2.hasColor) {
        bool m = display.mirror(false);
        for (uint16_t i = 0; i < sizeof(bitmaps) / sizeof(char *); i++) {
            display.firstPage();
            do {
                display.fillScreen(GxEPD_WHITE);
                display.drawInvertedBitmap(0, 0, bitmaps[i], 128, 296, GxEPD_BLACK);
            } while (display.nextPage());
            delay(2000);
        }
        display.mirror(m);
    }
}


void drawGraphics()
{
    display.setRotation(0);
    display.firstPage();
    do {
        display.drawRect(display.width() / 8, display.height() / 8, display.width() * 3 / 4, display.height() * 3 / 4, GxEPD_BLACK);
        display.drawLine(display.width() / 8, display.height() / 8, display.width() * 7 / 8, display.height() * 7 / 8, GxEPD_BLACK);
        display.drawLine(display.width() / 8, display.height() * 7 / 8, display.width() * 7 / 8, display.height() / 8, GxEPD_BLACK);
        display.drawCircle(display.width() / 2, display.height() / 2, display.height() / 4, GxEPD_BLACK);
        display.drawPixel(display.width() / 4, display.height() / 2, GxEPD_BLACK);
        display.drawPixel(display.width() * 3 / 4, display.height() / 2, GxEPD_BLACK);
    } while (display.nextPage());
}
