/*
    LilyGo Ink Screen Series Test
        - Created by Lewis he

Depend ESP8266Audio and ESP8266_Spiram libraries
    cd ~/Arduino/libraries
    git clone https://github.com/earlephilhower/ESP8266Audio
    git clone https://github.com/Gianbacchio/ESP8266_Spiram
*/


// According to the board, cancel the corresponding macro definition
// #define LILYGO_T5_V213   Not support
// #define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28


#include <boards.h>
#include <GxEPD.h>          //https://github.com/lewisxhe/GxEPD
#include <SD.h>
#include <FS.h>

// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54" b/w/r 200x200
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w
// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.51" b/w newer panel

// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r
// #include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7" b/w

// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13" b/w/r

// old panel
// #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w old panel
// #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w old panel

//! The current LilyGo uses the ink screen version DEPG0213BN
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w newer panel


// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r
// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w
// #include <GxQYEG0290BN/GxQYEG0290BN.h>    // 2.9" b/w new panel
// #include <GxDEPG0290B/GxDEPG0290B.h>      // 2.9" b/w new panel


#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

// https://github.com/earlephilhower/ESP8266Audio
#include <HTTPClient.h> //Remove ESP8266 warning
#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceID3.h"
#include "image.h"

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);


#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
SPIClass SDSPI(VSPI);
#endif

#if defined(_GxGDEW0213Z16_H_) || defined(_GxGDEW029Z10_H_) || defined(_GxGDEW027C44_H_) ||defined(_GxGDEW0154Z17_H_) || defined(_GxGDEW0154Z04_H_)
#define _HAS_COLOR_
#endif


bool setupSDCard(void)
{
#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
    SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);
    return SD.begin(SDCARD_CS, SDSPI);
#elif defined(_HAS_SDCARD_)
    return SD.begin(SDCARD_CS);
#endif
}

void setup()
{
    bool rlst = false;
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);

    display.init();
    display.setTextColor(GxEPD_BLACK);

    rlst = setupSDCard();

    display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);

#if defined(_HAS_COLOR_)
    display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_RED);
#else
    display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
#endif

#if defined(_HAS_SDCARD_)
    display.setRotation(1);
    display.setCursor(20, display.height() - 15);
    String sizeString = "SD:" + String(SD.cardSize() / 1024.0 / 1024.0 / 1024.0) + "G";
    display.println(rlst ? sizeString : "SD:N/A");

    int16_t x1, x2;
    uint16_t w, h;
    String str = GxEPD_BitmapExamplesQ;
    str = str.substring(2, str.lastIndexOf("/"));
    display.getTextBounds(str, 0, 0, &x1, &x2, &w, &h);
    display.setCursor(display.width() - w - 5, display.height() - 15);
    display.println(str);
#endif

    display.update();

#ifdef _HAS_PWR_CTRL_
    pinMode(SPK_POWER_EN, OUTPUT);
    digitalWrite(SPK_POWER_EN, HIGH);
#endif

    AudioGeneratorMP3 *mp3;
    AudioFileSourcePROGMEM *file;
    AudioOutputI2S *out;
    AudioFileSourceID3 *id3;

    //MP3 Decode
    file = new AudioFileSourcePROGMEM(image, sizeof(image));
    id3 = new AudioFileSourceID3(file);
#if defined(_BUILTIN_DAC_)
    out = new AudioOutputI2S(0, 1);
#else
    out = new AudioOutputI2S();
    out->SetPinout(IIS_BCK, IIS_WS, IIS_DOUT);
#endif
    mp3 = new AudioGeneratorMP3();
    mp3->begin(id3, out);
    while (1) {
        if (mp3->isRunning()) {
            if (!mp3->loop()) {
                mp3->stop();
                break;
            }
        } else {
            Serial.printf("MP3 done\n");
            break;
        }
    }
    delay(10000);
}

void loop()
{
    drawCornerTest();

    int i = 0;

    while (i < 4) {
        display.setRotation(i);
        showFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
        //showFont("FreeMonoBold18pt7b", &FreeMonoBold18pt7b);
        //showFont("FreeMonoBold24pt7b", &FreeMonoBold24pt7b);
        i++;
    }

    display.fillScreen(GxEPD_WHITE);

    display.update();

    display.powerDown();

    esp_sleep_enable_ext1_wakeup(((uint64_t)(((uint64_t)1) << BUTTON_1)), ESP_EXT1_WAKEUP_ALL_LOW);

    esp_deep_sleep_start();

}

void showFont(const char name[], const GFXfont *f)
{
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
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
    display.update();
    delay(5000);
}

void drawCornerTest()
{
    display.drawCornerTest();
    delay(5000);
    uint8_t rotation = display.getRotation();
    for (uint16_t r = 0; r < 4; r++) {
        display.setRotation(r);
        display.fillScreen(GxEPD_WHITE);
        display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
        display.fillRect(display.width() - 18, 0, 16, 16, GxEPD_BLACK);
        display.fillRect(display.width() - 25, display.height() - 25, 24, 24, GxEPD_BLACK);
        display.fillRect(0, display.height() - 33, 32, 32, GxEPD_BLACK);
        display.update();
        delay(5000);
    }
    display.setRotation(rotation); // restore
}

