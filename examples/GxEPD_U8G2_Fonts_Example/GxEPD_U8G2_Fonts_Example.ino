/*
    LilyGo Ink Screen Series u8g2Fonts Test
        - Created by Lewis he
*/

// According to the board, cancel the corresponding macro definition
// #define LILYGO_T5_V213
// #define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28


#include <boards.h>
#include <GxEPD.h>                          //https://github.com/lewisxhe/GxEPD

// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w
#include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.54" b/w newer panel

// #include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7" b/w

// #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w new panel
// #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w newer panel
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w newer panel

// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w
// #include <GxQYEG0290BN/GxQYEG0290BN.h>    // 2.9" b/w new panel


// The following screens are not supported
// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13" b/w/r
// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54" b/w/r 200x200
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r
// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r


#include <U8g2_for_Adafruit_GFX.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;


void setup(void)
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");
    display.init(); // enable diagnostic output on Serial
    u8g2Fonts.begin(display);
}


void loop()
{
    u8g2Fonts.setFontMode(1);                           // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(0);                      // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);          // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);          // apply Adafruit GFX color

    u8g2Fonts.setFont(u8g2_font_helvR14_tf);            // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

    uint16_t x = display.width() / 2 - 60 ;
    uint16_t y = display.height() / 2;

    display.fillScreen(GxEPD_WHITE);

    u8g2Fonts.setCursor(x, y);                          // start writing at this position
    u8g2Fonts.print("Hello World!");

    u8g2Fonts.setFont(u8g2_font_unifont_t_chinese2);    // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2Fonts.setCursor(x + 25, y + 32);
    u8g2Fonts.print("你好世界");

    display.update();
    delay(10000);
}
