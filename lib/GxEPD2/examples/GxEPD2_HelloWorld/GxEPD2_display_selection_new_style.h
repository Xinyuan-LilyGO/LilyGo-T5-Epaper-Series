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

// NOTE: you may need to adapt or select for your wiring in the processor specific conditional compile sections below

// select the display class (only one), matching the kind of display panel
#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
//#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
//#define GxEPD2_DISPLAY_CLASS GxEPD2_4C
//#define GxEPD2_DISPLAY_CLASS GxEPD2_7C

// select the display driver class (only one) for your  panel
//#define GxEPD2_DRIVER_CLASS GxEPD2_102     // GDEW0102T4   80x128, UC8175, (WFT0102CZA2)
//#define GxEPD2_DRIVER_CLASS GxEPD2_150_BN  // DEPG0150BN 200x200, SSD1681, (FPC8101), TTGO T5 V2.4.1
//#define GxEPD2_DRIVER_CLASS GxEPD2_154     // GDEP015OC1  200x200, IL3829, (WFC0000CZ07), no longer available
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_D67 // GDEH0154D67 200x200, SSD1681, (HINK-E154A07-A1)
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_T8  // GDEW0154T8  152x152, UC8151 (IL0373), (WFT0154CZ17)
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_M09 // GDEW0154M09 200x200, JD79653A, (WFT0154CZB3)
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_M10 // GDEW0154M10 152x152, UC8151D, (WFT0154CZ17)
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_GDEY0154D67 // GDEY0154D67 200x200, SSD1681, (FPC-B001 20.05.21)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213     // GDE0213B1   122x250, IL3895, (HINK-E0213-G01), phased out
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_B72 // GDEH0213B72 122x250, SSD1675A (IL3897), (HINK-E0213A22-A0 SLH1852)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_B73 // GDEH0213B73 122x250, SSD1675B, (HINK-E0213A22-A0 SLH1914)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_B74 // GDEM0213B74 122x250, SSD1680, FPC-7528B)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_flex // GDEW0213I5F 104x212, UC8151 (IL0373), (WFT0213CZ16)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_M21 // GDEW0213M21 104x212, UC8151 (IL0373), (WFT0213CZ16)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_T5D // GDEW0213T5D 104x212, UC8151D, (WFT0213CZ16)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_BN // DEPG0213BN  122x250, SSD1680, (FPC-7528B), TTGO T5 V2.4.1, V2.3.1
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_GDEY0213B74 // GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)
//#define GxEPD2_DRIVER_CLASS GxEPD2_260     // GDEW026T0   152x296, UC8151 (IL0373), (WFT0154CZ17)
//#define GxEPD2_DRIVER_CLASS GxEPD2_260_M01 // GDEW026M01  152x296, UC8151 (IL0373), (WFT0260CZB2)
//#define GxEPD2_DRIVER_CLASS GxEPD2_266_BN // DEPG0266BN   152x296, SSD1680, (FPC7510), TTGO T5 V2.66, TTGO T5 V2.4.1
//#define GxEPD2_DRIVER_CLASS GxEPD2_266_GDEY0266T90 // GDEY0266T90 152x296, SSD1680, (FPC-A003 HB)
//#define GxEPD2_DRIVER_CLASS GxEPD2_270     // GDEW027W3   176x264, EK79652 (IL91874), (WFI0190CZ22)
//#define GxEPD2_DRIVER_CLASS GxEPD2_270_GDEY027T91 // GDEY027T91 176x264, SSD1680, (FB)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290     // GDEH029A1   128x296, SSD1608 (IL3820), (E029A01-FPC-A1 SYX1553)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T5  // GDEW029T5   128x296, UC8151 (IL0373), (WFT0290CZ10)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T5D // GDEW029T5D  128x296, UC8151D, (WFT0290CZ10)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_I6FD // GDEW029I6FD  128x296, UC8151D, (WFT0290CZ10)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T94 // GDEM029T94  128x296, SSD1680, (FPC-7519 rev.b)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_T94_V2 // GDEM029T94  128x296, SSD1680, (FPC-7519 rev.b), Waveshare 2.9" V2 variant
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_BS // DEPG0290BS  128x296, SSD1680, (FPC-7519 rev.b)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_M06 // GDEW029M06  128x296, UC8151D, (WFT0290CZ10)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_GDEY029T94 // GDEY029T94 128x296, SSD1680, (FPC-A005 20.06.15)
//#define GxEPD2_DRIVER_CLASS GxEPD2_371     // GDEW0371W7  240x416, UC8171 (IL0324), (missing)
//#define GxEPD2_DRIVER_CLASS GxEPD2_370_TC1 // ED037TC1  280x480, SSD1677, (ICA-FU-20 ichia 2029), Waveshare 3.7"
//#define GxEPD2_DRIVER_CLASS GxEPD2_420     // GDEW042T2   400x300, UC8176 (IL0398), (WFT042CZ15)
//#define GxEPD2_DRIVER_CLASS GxEPD2_420_M01 // GDEW042M01  400x300, UC8176 (IL0398), (WFT042CZ15)
//#define GxEPD2_DRIVER_CLASS GxEPD2_420_GDEY042T91 // GDEY042T91 400x300, SSD1683 (no inking)
//#define GxEPD2_DRIVER_CLASS GxEPD2_583     // GDEW0583T7  600x448, UC8159c (IL0371), (missing)
//#define GxEPD2_DRIVER_CLASS GxEPD2_583_T8  // GDEW0583T8  648x480, EK79655 (GD7965), (WFT0583CZ61)
//#define GxEPD2_DRIVER_CLASS GxEPD2_583_GDEQ0583T31 // GDEQ0583T31  648x480, UC8179, (P583010-MF1-B)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750     // GDEW075T8   640x384, UC8159c (IL0371), (WF0583CZ09)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750_T7  // GDEW075T7   800x480, EK79655 (GD7965), (WFT0583CZ61)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750_YT7  // GDEY075T7  800x480, UC8179 (GD7965), (FPC-C001 20.8.20)
//#define GxEPD2_DRIVER_CLASS GxEPD2_1160_T91 // GDEH116T91 960x640, SSD1677, (none or hidden)
//#define GxEPD2_DRIVER_CLASS GxEPD2_1248     // GDEW1248T3 1304x984, UC8179, (WFT1248BZ23,WFT1248BZ24)
// 3-color e-papers
//#define GxEPD2_DRIVER_CLASS GxEPD2_154c     // GDEW0154Z04 200x200, IL0376F, (WFT0000CZ04), no longer available
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_Z90c // GDEH0154Z90 200x200, SSD1681, (HINK-E154A07-A1)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213c     // GDEW0213Z16 104x212, UC8151 (IL0373), (WFT0213CZ16)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_Z19c // GDEH0213Z19 104x212, UC8151D, (HINK-E0213A20-A2 2020-11-19)
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_Z98c // GDEY0213Z98 122x250, SSD1680, (FPC-A002 20.04.08)
//#define GxEPD2_DRIVER_CLASS GxEPD2_266c     // GDEY0266Z90 152x296, SSD1680, (FPC-7510)
//#define GxEPD2_DRIVER_CLASS GxEPD2_270c     // GDEW027C44  176x264, IL91874, (WFI0190CZ22)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290c     // GDEW029Z10  128x296, UC8151 (IL0373), (WFT0290CZ10)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_Z13c // GDEH029Z13  128x296, UC8151D, (HINK-E029A10-A3 20160809)
//#define GxEPD2_DRIVER_CLASS GxEPD2_290_C90c // GDEM029C90  128x296, SSD1680, (FPC-7519 rev.b)
//#define GxEPD2_DRIVER_CLASS GxEPD2_420c     // GDEW042Z15  400x300, UC8176 (IL0398), (WFT0420CZ15)
//#define GxEPD2_DRIVER_CLASS GxEPD2_420c_Z21 // GDEQ042Z21  400x300, UC8276, (hidden)
//#define GxEPD2_DRIVER_CLASS GxEPD2_583c     // GDEW0583Z21 600x448, UC8159c (IL0371), (missing)
//#define GxEPD2_DRIVER_CLASS GxEPD2_583c_Z83 // GDEW0583Z83 648x480, EK79655 (GD7965), (WFT0583CZ61)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c     // GDEW075Z09  640x384, UC8159c (IL0371), (WF0583CZ09)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c_Z08 // GDEW075Z08  800x480, EK79655 (GD7965), (WFT0583CZ61)
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c_Z90 // GDEH075Z90  880x528, SSD1677, (HINK-E075A07-A0)
//#define GxEPD2_DRIVER_CLASS GxEPD2_1248c    // GDEY1248Z51 1304x984, UC8179, (WFT1248BZ23,WFT1248BZ24)
// 4-color e-paper
//#define GxEPD2_DRIVER_CLASS GxEPD2_437c     // Waveshare 4.37" 4-color
// 7-color e-paper
//#define GxEPD2_DRIVER_CLASS GxEPD2_565c // Waveshare 5.65" 7-color
//#define GxEPD2_DRIVER_CLASS GxEPD2_730c_GDEY073D46 // GDEY073D46 800x480 7-color, (N-FPC-001 2021.11.26)
// grey levels parallel IF e-papers on Waveshare e-Paper IT8951 Driver HAT
//#define GxEPD2_DRIVER_CLASS GxEPD2_it60           // ED060SCT 800x600
//#define GxEPD2_DRIVER_CLASS GxEPD2_it60_1448x1072 // ED060KC1 1448x1072
//#define GxEPD2_DRIVER_CLASS GxEPD2_it78_1872x1404 // ED078KC2 1872x1404
//#define GxEPD2_DRIVER_CLASS GxEPD2_it103_1872x1404 // ES103TC1 1872x1404

// SS is usually used for CS. define here for easy change
#ifndef EPD_CS
#define EPD_CS SS
#endif

#if defined(GxEPD2_DISPLAY_CLASS) && defined(GxEPD2_DRIVER_CLASS)

// somehow there should be an easier way to do this
#define GxEPD2_BW_IS_GxEPD2_BW true
#define GxEPD2_3C_IS_GxEPD2_3C true
#define GxEPD2_4C_IS_GxEPD2_4C true
#define GxEPD2_7C_IS_GxEPD2_7C true
#define GxEPD2_1248_IS_GxEPD2_1248 true
#define GxEPD2_1248c_IS_GxEPD2_1248c true
#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_BW(x) IS_GxEPD(GxEPD2_BW_IS_, x)
#define IS_GxEPD2_3C(x) IS_GxEPD(GxEPD2_3C_IS_, x)
#define IS_GxEPD2_4C(x) IS_GxEPD(GxEPD2_4C_IS_, x)
#define IS_GxEPD2_7C(x) IS_GxEPD(GxEPD2_7C_IS_, x)
#define IS_GxEPD2_1248(x) IS_GxEPD(GxEPD2_1248_IS_, x)
#define IS_GxEPD2_1248c(x) IS_GxEPD(GxEPD2_1248c_IS_, x)

#include "GxEPD2_selection_check.h"

#if defined (ESP8266)
#define MAX_DISPLAY_BUFFER_SIZE (81920ul-34000ul-5000ul) // ~34000 base use, change 5000 to your application use
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));
// mapping of Waveshare e-Paper ESP8266 Driver Board, new version
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=15*/ EPD_CS, /*DC=4*/ 4, /*RST=2*/ 2, /*BUSY=5*/ 5));
// mapping of Waveshare e-Paper ESP8266 Driver Board, old version
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=15*/ EPD_CS, /*DC=4*/ 4, /*RST=5*/ 5, /*BUSY=16*/ 16));
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

#if defined(ESP32)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
#if !IS_GxEPD2_1248(GxEPD2_DRIVER_CLASS) && !IS_GxEPD2_1248c(GxEPD2_DRIVER_CLASS)
#if defined(ARDUINO_LOLIN_D32_PRO)
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 0, /*RST=*/ 2, /*BUSY=*/ 15)); // my LOLIN_D32_PRO proto board
#else
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // my suggested wiring and proto board
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // LILYGO_T5_V2.4.1
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 19, /*RST=*/ 4, /*BUSY=*/ 34)); // LILYGO® TTGO T5 2.66
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 2, /*RST=*/ 0, /*BUSY=*/ 4)); // e.g. TTGO T8 ESP32-WROVER
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // Waveshare ESP32 Driver Board
#endif
#else // GxEPD2_1248 or GxEPD2_1248c
// Waveshare 12.48 b/w or b/w/r SPI display board and frame or Good Display 12.48 b/w panel GDEW1248T3 or b/w/r panel GDEY1248Z51
// general constructor for use with all parameters, e.g. for Waveshare ESP32 driver board mounted on connection board
GxEPD2_DISPLAY_CLASS < GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS) > display(GxEPD2_DRIVER_CLASS(/*sck=*/ 13, /*miso=*/ 12, /*mosi=*/ 14,
    /*cs_m1=*/ 23, /*cs_s1=*/ 22, /*cs_m2=*/ 16, /*cs_s2=*/ 19,
    /*dc1=*/ 25, /*dc2=*/ 17, /*rst1=*/ 33, /*rst2=*/ 5,
    /*busy_m1=*/ 32, /*busy_s1=*/ 26, /*busy_m2=*/ 18, /*busy_s2=*/ 4));
#endif
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

// can't use package "STMF1 Boards (STM32Duino.com)" (Roger Clark) anymore with Adafruit_GFX, use "STM32 Boards (selected from submenu)" (STMicroelectronics)
#if defined(ARDUINO_ARCH_STM32)
#define MAX_DISPLAY_BUFFER_SIZE 15000ul // ~15k is a good compromise
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=PA4*/ EPD_CS, /*DC=*/ PA3, /*RST=*/ PA2, /*BUSY=*/ PA1));
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

#if defined(__AVR)
#if defined (ARDUINO_AVR_MEGA2560) // Note: SS is on 53 on MEGA
#define MAX_DISPLAY_BUFFER_SIZE 5000 // e.g. full height for 200x200
#else // Note: SS is on 10 on UNO, NANO
#define MAX_DISPLAY_BUFFER_SIZE 800 // 
#endif
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));
// for Arduino Micro or Arduino Leonardo with CS on 10 on my proto boards (SS would be 17) uncomment instead:
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 10, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));
#endif

#if defined(ARDUINO_ARCH_SAM)
#define MAX_DISPLAY_BUFFER_SIZE 32768ul // e.g., up to 96k
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=10*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

#if defined(ARDUINO_ARCH_SAMD)
#define MAX_DISPLAY_BUFFER_SIZE 15000ul // ~15k is a good compromise
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=4*/ 4, /*DC=*/ 7, /*RST=*/ 6, /*BUSY=*/ 5));
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=4*/ 4, /*DC=*/ 3, /*RST=*/ 2, /*BUSY=*/ 1)); // my Seed XIOA0
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=4*/ 3, /*DC=*/ 2, /*RST=*/ 1, /*BUSY=*/ 0)); // my other Seed XIOA0
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

#if defined(ARDUINO_ARCH_RP2040)
#define MAX_DISPLAY_BUFFER_SIZE 131072ul // e.g. half of available ram
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
#if defined(ARDUINO_NANO_RP2040_CONNECT)
// adapt the constructor parameters to your wiring
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));
#endif
#if defined(ARDUINO_RASPBERRY_PI_PICO)
// adapt the constructor parameters to your wiring
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7)); // my proto board
// mapping of GoodDisplay DESPI-PICO. NOTE: uses alternate HW SPI pins!
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 3, /*DC=*/ 2, /*RST=*/ 1, /*BUSY=*/ 0)); // DESPI-PICO
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 3, /*DC=*/ 2, /*RST=*/ 11, /*BUSY=*/ 10)); // DESPI-PICO modified
#endif
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

#endif
