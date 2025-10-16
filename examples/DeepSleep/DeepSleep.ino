/*
    LilyGo Ink Screen Series Test
        - Created by Lewis he
*/

// According to the board, cancel the corresponding macro definition
#define LILYGO_T5_V213
// #define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28
// #define LILYGO_T5_V102
// #define LILYGO_T5_V266
// #define LILYGO_EPD_DISPLAY         //T-Display 1.02 inch epaper   //Depend  https://github.com/adafruit/Adafruit_NeoPixel
// #define LILYGO_EPD_DISPLAY_154

#include <boards.h>
#include <GxEPD.h>

#if defined(LILYGO_T5_V102) || defined(LILYGO_EPD_DISPLAY)
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
#include <Adafruit_NeoPixel.h>             //Depend  https://github.com/adafruit/Adafruit_NeoPixel
#elif defined(LILYGO_T5_V266)
#include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
#elif defined(LILYGO_T5_V213)
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
#include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13" b/w  form GoodDisplay 4-color
#else
// #include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54" b/w/r 200x200
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w
// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.51" b/w   form DKE GROUP
// #include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
// #include <GxDEPG0290R/GxDEPG0290R.h>      // 2.9" b/w/r  form DKE GROUP
// #include <GxDEPG0290B/GxDEPG0290B.h>      // 2.9" b/w    form DKE GROUP
// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r  form GoodDisplay
// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13" b/w/r form GoodDisplay
// #include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13" b/w  form GoodDisplay 4-color
// #include <GxGDEW0213M21/GxGDEW0213M21.h>  // 2.13"  b/w Ultra wide temperature , form GoodDisplay
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
// #include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7" b/w   form GoodDisplay
// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r form GoodDisplay
// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w   form GoodDisplay
// #include <GxDEPG0750BN/GxDEPG0750BN.h>    // 7.5" b/w   form DKE GROUP
#endif


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

void setup()
{
    WiFi.mode(WIFI_OFF);

    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
    display.init();
    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);

    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 0);
    display.println("Deep Sleep Test");
    display.update();

    delay(1000);

    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(0, 0);
    display.println("Power off");
    display.update();
    display.powerDown();

    delay(1000);

    SPI.end();

    pinMode(EPD_CS, INPUT);
    pinMode(EPD_DC, INPUT);
    pinMode(EPD_RSET, INPUT);
    pinMode(EPD_BUSY, INPUT);
    pinMode(EPD_SCLK, INPUT);
    pinMode(EPD_MISO, INPUT);
    pinMode(EPD_MOSI, INPUT);

#if defined(_HAS_SDCARD_)
    pinMode(SDCARD_SCLK, INPUT);
    pinMode(SDCARD_MISO, INPUT);
    pinMode(SDCARD_MOSI, INPUT);
    pinMode(SDCARD_CS, INPUT);
#endif

    // T5 2.13 DEPG0213BN model deepsleep current ~= 166uA (ext1 waekup)
    // T5 2.13 GDEM0213B74 model deepsleep current ~= 70uA (ext1 waekup)
    // esp_sleep_enable_ext1_wakeup(((uint64_t)(((uint64_t)1) << BUTTON_1)), ESP_EXT1_WAKEUP_ALL_LOW);

    // T5 2.13 DEPG0213BN model deepsleep current ~= 166uA (timer waekup)
    // T5 2.13 GDEM0213B74 model deepsleep current ~= 70uA (timer waekup)
    const uint32_t uS_TO_S_FACTOR =  1000000ULL;  /* Conversion factor for micro seconds to seconds */
    const uint32_t TIME_TO_SLEEP  =  5 ;       /* Time ESP32 will go to sleep (in seconds) */
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    esp_deep_sleep_start();
}

void loop()
{
}
