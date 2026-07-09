/**
 * @file      DeepSleep.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2026  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2026-07-08
 *
 * Deep sleep example using GxEPD2 paged drawing API.
 * Shows "Good Morning" on wake, then "Good Night" before sleeping.
 */

#define EPD_PWR_EN              (12)    // Only V2.4 Version , v2.3.1 version not have this pin
#define BUTTON_1                (39)

#include <SPI.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <WiFi.h>

// GxEPD2 display selection (uncomment ONE and set its PANEL_NAME)
#define PANEL_NAME "DEPG0213BN"
GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // DEPG0213BN 122x250, SSD1680, TTGO T5 V2.4.1, V2.3.1

// #define PANEL_NAME "GDEM0213B74"
// GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEM0213B74 122x250, SSD1680

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

/* Sleep duration configuration */
const uint32_t uS_TO_S_FACTOR = 1000000ULL;   /* Conversion factor for micro seconds to seconds */
const uint32_t TIME_TO_SLEEP = 30;             /* Time ESP32 will go to sleep (in seconds) */

/* Boot counter stored in RTC memory to track wake-up count */
RTC_DATA_ATTR static uint32_t bootCount = 0;

/*
 * Draw a crescent moon icon at (cx, cy) with given radius.
 * The moon is formed by two overlapping circles.
 */
void drawMoon(int16_t cx, int16_t cy, int16_t r)
{
    display.fillCircle(cx, cy, r, GxEPD_BLACK);
    display.fillCircle(cx + r / 2, cy - r / 3, r * 7 / 10, GxEPD_WHITE);
}

/*
 * Draw a small star (4-point) at (cx, cy) with given size.
 */
void drawStar(int16_t cx, int16_t cy, int16_t size)
{
    display.drawFastVLine(cx, cy - size, size * 2 + 1, GxEPD_BLACK);
    display.drawFastHLine(cx - size, cy, size * 2 + 1, GxEPD_BLACK);
}

/*
 * Draw a sun icon at (cx, cy) with given radius.
 * Circle center + 8 rays around it.
 */
void drawSun(int16_t cx, int16_t cy, int16_t r)
{
    display.fillCircle(cx, cy, r, GxEPD_BLACK);

    int16_t rayLen = r / 2;
    int16_t rayGap = r + 2;

    /* 8 rays: up, down, left, right, and 4 diagonals */
    int16_t dx[] = {0, 0, -1, 1, -1, 1, -1, 1};
    int16_t dy[] = {-1, 1, 0, 0, -1, -1, 1, 1};

    for (int i = 0; i < 8; i++) {
        int16_t x1 = cx + dx[i] * rayGap;
        int16_t y1 = cy + dy[i] * rayGap;
        int16_t x2 = cx + dx[i] * (rayGap + rayLen);
        int16_t y2 = cy + dy[i] * (rayGap + rayLen);
        display.drawLine(x1, y1, x2, y2, GxEPD_BLACK);
        display.drawLine(x1 + 1, y1, x2 + 1, y2, GxEPD_BLACK);
    }
}

/*
 * Draw a horizontal decorative divider line with a diamond in the center.
 */
void drawDivider(int16_t y, int16_t width)
{
    int16_t cx = display.width() / 2;
    int16_t startX = cx - width / 2;
    int16_t endX = cx + width / 2;

    display.drawLine(startX, y, cx - 6, y, GxEPD_BLACK);
    display.drawLine(cx + 6, y, endX, y, GxEPD_BLACK);

    /* diamond shape */
    int16_t d = 4;
    display.fillTriangle(cx, y - d, cx + d, y, cx, y + d, GxEPD_BLACK);
    display.fillTriangle(cx, y - d, cx - d, y, cx, y + d, GxEPD_BLACK);
}

/*
 * Draw a rounded rectangle outline.
 */
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
    display.drawRoundRect(x, y, w, h, r, GxEPD_BLACK);
}

/*
 * Show the wake-up screen: "Good Morning" with sun icon,
 * boot count, and sleep info.
 */
void showWakeScreen()
{
    uint16_t screenW = display.width();
    uint16_t screenH = display.height();

    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        /* --- Sun icon at top center --- */
        drawSun(screenW / 2, 35, 14);

        /* --- Main title --- */
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        u8g2Fonts.setFont(u8g2_font_helvR14_tf);
        const char *title = "Good Morning";
        int16_t titleW = u8g2Fonts.getUTF8Width(title);
        u8g2Fonts.setCursor((screenW - titleW) / 2, 68);
        u8g2Fonts.print(title);

        /* --- Decorative divider --- */
        drawDivider(78, screenW - 40);

        /* --- Status info box --- */
        int16_t boxX = 10;
        int16_t boxY = 90;
        int16_t boxW = screenW - 20;
        int16_t boxH = 42;
        drawRoundRect(boxX, boxY, boxW, boxH, 6);

        /* wake-up count */
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        String wakeStr = "Wake-up #" + String(bootCount);
        int16_t wakeW = u8g2Fonts.getUTF8Width(wakeStr.c_str());
        u8g2Fonts.setCursor((screenW - wakeW) / 2, boxY + 16);
        u8g2Fonts.print(wakeStr);

        /* sleep duration info */
        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        String sleepStr = "Slept for " + String(TIME_TO_SLEEP) + "s";
        int16_t sleepW = u8g2Fonts.getUTF8Width(sleepStr.c_str());
        u8g2Fonts.setCursor((screenW - sleepW) / 2, boxY + 32);
        u8g2Fonts.print(sleepStr);

        /* --- Bottom hint --- */
        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        const char *hint = "Going to sleep in a moment...";
        int16_t hintW = u8g2Fonts.getUTF8Width(hint);
        u8g2Fonts.setCursor((screenW - hintW) / 2, screenH - 10);
        u8g2Fonts.print(hint);
    } while (display.nextPage());
}

/*
 * Show the sleep screen: "Good Night" with moon & stars,
 * countdown info, and power-off message.
 */
void showSleepScreen()
{
    uint16_t screenW = display.width();
    uint16_t screenH = display.height();

    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        /* --- Moon and stars at top --- */
        drawMoon(screenW / 2 - 5, 30, 16);
        drawStar(screenW / 2 + 25, 18, 3);
        drawStar(screenW / 2 - 28, 22, 2);
        drawStar(screenW / 2 + 18, 40, 2);

        /* --- Main title --- */
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        u8g2Fonts.setFont(u8g2_font_helvR14_tf);
        const char *title = "Good Night";
        int16_t titleW = u8g2Fonts.getUTF8Width(title);
        u8g2Fonts.setCursor((screenW - titleW) / 2, 65);
        u8g2Fonts.print(title);

        /* --- Decorative divider --- */
        drawDivider(75, screenW - 40);

        /* --- Status info box --- */
        int16_t boxX = 10;
        int16_t boxY = 87;
        int16_t boxW = screenW - 20;
        int16_t boxH = 42;
        drawRoundRect(boxX, boxY, boxW, boxH, 6);

        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        String sleepInfo = "Sleeping for " + String(TIME_TO_SLEEP) + "s";
        int16_t infoW = u8g2Fonts.getUTF8Width(sleepInfo.c_str());
        u8g2Fonts.setCursor((screenW - infoW) / 2, boxY + 16);
        u8g2Fonts.print(sleepInfo);

        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        String bootInfo = "Boot count: " + String(bootCount);
        int16_t bootW = u8g2Fonts.getUTF8Width(bootInfo.c_str());
        u8g2Fonts.setCursor((screenW - bootW) / 2, boxY + 32);
        u8g2Fonts.print(bootInfo);

        /* --- Bottom hint --- */
        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        const char *hint = "Auto wake-up in 20 seconds";
        int16_t hintW = u8g2Fonts.getUTF8Width(hint);
        u8g2Fonts.setCursor((screenW - hintW) / 2, screenH - 10);
        u8g2Fonts.print(hint);
    } while (display.nextPage());
}


void setup()
{
    Serial.begin(115200);
    Serial.println("Start ...");

    // Power on the EPD
    pinMode(EPD_PWR_EN, OUTPUT);
    digitalWrite(EPD_PWR_EN, HIGH);

    WiFi.mode(WIFI_OFF);

    SPI.begin(/*SCK*/ 18, /*MISO*/ 19, /*MOSI*/ 23);
    display.init(115200, true, 2, false);
    display.setRotation(0);

    u8g2Fonts.begin(display);

    bootCount++;
    Serial.print("Boot count: ");
    Serial.println(bootCount);

    /* --- Show wake-up screen --- */
    showWakeScreen();
    delay(3000);

    /* --- Show sleep screen --- */
    showSleepScreen();
    delay(2000);

    /* --- Prepare for deep sleep --- */
    display.powerOff();

    /* Enable timer wake-up */
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    /* Also enable button wake-up (BUTTON_1) */
    // esp_sleep_enable_ext1_wakeup(((uint64_t)(((uint64_t)1) << BUTTON_1)), ESP_EXT1_WAKEUP_ALL_LOW);

    esp_deep_sleep_start();
}

void loop()
{
}
