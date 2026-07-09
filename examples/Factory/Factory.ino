/**
 * @file      Factory.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2026  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2026-07-09
 *
 */
#define EPD_PWR_EN              (12)    // Only V2.4 Version , v2.3.1 version not have this pin
#define BUTTON_1                (39)
#define ADC_PIN                 (35)

#include <SD.h>
#include <FS.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <WiFi.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>

// GxEPD2 display selection (uncomment ONE and set its PANEL_NAME)

// DEPG0213BN 122x250, SSD1680, TTGO T5 2.13 V2.4 (FPC-7528B)
#define PANEL_NAME "DEPG0213BN"
GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

// GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)
// #define PANEL_NAME "GDEM0213B74"
// GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEM0213B74 122x250, SSD1680

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

SPIClass SDSPI(VSPI);

int16_t g_voltageY = 0;
uint32_t interval = 0;
uint32_t loopCount = 0;

/*
 * Scan WiFi networks and return the count.
 * Results are also printed to Serial.
 */
int scanWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Scanning WiFi...");
    int n = WiFi.scanNetworks();

    Serial.println("Scan done");
    if (n == 0) {
        Serial.println("No networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
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
    WiFi.mode(WIFI_OFF);
    return n;
}

/*
 * Draw a WiFi signal icon at (x, y) with given bar count (1-3 bars).
 * bars: number of signal bars to fill (1=weak, 2=medium, 3=strong)
 */
void drawWiFiIcon(int16_t x, int16_t y, int8_t bars)
{
    /* Base arc */
    display.drawPixel(x + 4, y, GxEPD_BLACK);
    display.drawPixel(x + 3, y + 1, GxEPD_BLACK);
    display.drawPixel(x + 5, y + 1, GxEPD_BLACK);
    display.drawPixel(x + 2, y + 2, GxEPD_BLACK);
    display.drawPixel(x + 6, y + 2, GxEPD_BLACK);

    /* Bar 1 (weakest) */
    if (bars >= 1) {
        display.fillRect(x + 3, y + 4, 3, 2, GxEPD_BLACK);
    }
    /* Bar 2 (medium) */
    if (bars >= 2) {
        display.fillRect(x + 1, y + 6, 7, 2, GxEPD_BLACK);
    }
    /* Bar 3 (strong) */
    if (bars >= 3) {
        display.fillRect(x - 1, y + 8, 11, 2, GxEPD_BLACK);
    }
}

/*
 * Draw a horizontal divider line.
 */
void drawDivider(int16_t y, int16_t width)
{
    int16_t cx = display.width() / 2;
    int16_t startX = cx - width / 2;
    int16_t endX = cx + width / 2;
    display.drawLine(startX, y, endX, y, GxEPD_BLACK);
    display.drawPixel(startX - 1, y, GxEPD_BLACK);
    display.drawPixel(endX + 1, y, GxEPD_BLACK);
}

/*
 * Show a "scanning" screen so the user knows WiFi scan is in progress.
 */
void showScanningScreen()
{
    display.setRotation(0);

    uint16_t screenW = display.width();
    uint16_t screenH = display.height();

    u8g2Fonts.begin(display);
    u8g2Fonts.setFontMode(1);
    u8g2Fonts.setFontDirection(0);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        /* Title */
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        const char *title = "LilyGo E-Paper";
        int16_t titleW = u8g2Fonts.getUTF8Width(title);
        u8g2Fonts.setCursor((screenW - titleW) / 2, 20);
        u8g2Fonts.print(title);

        /* Panel model */
        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        const char *panel = "Panel: " PANEL_NAME;
        int16_t panelW = u8g2Fonts.getUTF8Width(panel);
        u8g2Fonts.setCursor((screenW - panelW) / 2, 32);
        u8g2Fonts.print(panel);

        /* Scanning indicator - centered */
        int16_t midY = screenH / 2 + 5;

        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        const char *scanText = "Scanning WiFi";
        int16_t scanW = u8g2Fonts.getUTF8Width(scanText);
        u8g2Fonts.setCursor((screenW - scanW) / 2, midY);
        u8g2Fonts.print(scanText);

        /* Animated dots */
        u8g2Fonts.setCursor((screenW - scanW) / 2 + scanW + 2, midY);
        u8g2Fonts.print("...");

        /* Bottom hint */
        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        const char *hint = "Please wait";
        int16_t hintW = u8g2Fonts.getUTF8Width(hint);
        u8g2Fonts.setCursor((screenW - hintW) / 2, screenH - 10);
        u8g2Fonts.print(hint);
    } while (display.nextPage());
}

/*
 * Read battery voltage from ADC, average 16 samples for stability.
 */
uint16_t readBatteryVoltage()
{
    uint32_t sum = 0;
    for (int i = 0; i < 16; i++) {
        sum += analogReadMilliVolts(ADC_PIN);
    }
    uint16_t voltage = (sum / 16) * 2;   // voltage divider ratio = 2
    if (voltage > 4200) voltage = 4200;  // clamp to max LiPo voltage
    return voltage;
}

/*
 * Draw the initial full-screen dashboard.
 * Returns the Y position where the battery section starts.
 */
int16_t drawDashboardFull(int wifiCount, bool sdReady, uint16_t batteryMv)
{
    display.setRotation(0);

    uint16_t screenW = display.width();
    uint16_t screenH = display.height();

    u8g2Fonts.begin(display);
    u8g2Fonts.setFontMode(1);
    u8g2Fonts.setFontDirection(0);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        int16_t curY = 4;

        /* ---- Title bar ---- */
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        u8g2Fonts.setCursor(4, curY + 10);
        u8g2Fonts.print("LilyGo E-Paper");
        curY += 14;
        drawDivider(curY, screenW - 8);
        curY += 4;

        /* ---- WiFi Section ---- */
        curY += 2;
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        u8g2Fonts.setCursor(4, curY + 10);
        u8g2Fonts.print("WiFi Scan");
        drawWiFiIcon(screenW - 16, curY, wifiCount >= 3 ? 3 : (wifiCount >= 1 ? 2 : 1));
        curY += 14;

        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        if (wifiCount == 0) {
            u8g2Fonts.setCursor(8, curY + 6);
            u8g2Fonts.print("No networks found");
            curY += 10;
        } else {
            String countStr = "Found " + String(wifiCount) + " network" + (wifiCount > 1 ? "s" : "");
            u8g2Fonts.setCursor(8, curY + 6);
            u8g2Fonts.print(countStr);
            curY += 10;

            int showCount = min(wifiCount, 4);
            for (int i = 0; i < showCount; i++) {
                String ssid = WiFi.SSID(i);
                int rssi = WiFi.RSSI(i);
                bool encrypted = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;

                int8_t sigBars = 1;
                if (rssi > -50) sigBars = 3;
                else if (rssi > -70) sigBars = 2;

                int16_t barX = 8;
                for (int b = 0; b < 3; b++) {
                    int16_t barH = 2 + b;
                    int16_t barY = curY + 8 - barH;
                    if (b < sigBars) {
                        display.fillRect(barX + b * 3, barY, 2, barH, GxEPD_BLACK);
                    } else {
                        display.drawRect(barX + b * 3, barY, 2, barH, GxEPD_BLACK);
                    }
                }

                u8g2Fonts.setCursor(20, curY + 6);
                if (ssid.length() > 18) ssid = ssid.substring(0, 17) + "~";
                u8g2Fonts.print(ssid);
                u8g2Fonts.setCursor(screenW - 14, curY + 6);
                u8g2Fonts.print(encrypted ? "L" : " ");
                u8g2Fonts.setCursor(screenW - 30, curY + 6);
                u8g2Fonts.print(String(rssi) + "dB");
                curY += 10;
            }
            if (wifiCount > 4) {
                u8g2Fonts.setCursor(8, curY + 6);
                u8g2Fonts.print("... and " + String(wifiCount - 4) + " more");
                curY += 10;
            }
        }

        curY += 2;
        drawDivider(curY, screenW - 8);
        curY += 4;

        /* ---- SD Card Section ---- */
        curY += 2;
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        u8g2Fonts.setCursor(4, curY + 10);
        u8g2Fonts.print("SD Card");
        curY += 14;
        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        if (sdReady) {
            float sizeGB = SD.cardSize() / 1024.0 / 1024.0 / 1024.0;
            u8g2Fonts.setCursor(8, curY + 6);
            u8g2Fonts.print("Ready  " + String(sizeGB, 1) + " GB");
        } else {
            u8g2Fonts.setCursor(8, curY + 6);
            u8g2Fonts.print("Not detected");
        }
        curY += 12;

        drawDivider(curY, screenW - 8);
        curY += 4;

        /* ---- Display Info Section ---- */
        curY += 2;
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        u8g2Fonts.setCursor(4, curY + 10);
        u8g2Fonts.print("Display");
        curY += 14;
        u8g2Fonts.setFont(u8g2_font_5x7_tf);
        u8g2Fonts.setCursor(8, curY + 6);
        u8g2Fonts.print(String(display.width()) + " x " + String(display.height()) + " px");
        curY += 10;
        u8g2Fonts.setCursor(8, curY + 6);
        u8g2Fonts.print("Model: " PANEL_NAME);
        curY += 12;

        drawDivider(curY, screenW - 8);
        curY += 4;

        /* ---- Battery Voltage Section (static frame) ---- */
        int16_t batY = curY + 2;
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        u8g2Fonts.setCursor(4, batY + 10);
        u8g2Fonts.print("Battery");
        batY += 14;
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        u8g2Fonts.setCursor(8, batY + 10);
        u8g2Fonts.print("--.-- V");
        batY += 14;

        drawDivider(batY, screenW - 8);
        batY += 4;

        /* ---- Footer ---- */
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        const char *footer = "Sampling 30s...";
        int16_t footW = u8g2Fonts.getUTF8Width(footer);
        u8g2Fonts.setCursor((screenW - footW) / 2, screenH - 8);
        u8g2Fonts.print(footer);
    } while (display.nextPage());

    /* Battery section starts at y=170 (after Display section ends at ~166).
     * Use a single partial window covering battery + footer to avoid
     * position miscalculation. */
    return 170;
}

/*
 * Partial-update the battery voltage value and the countdown footer
 * in a single partial window to avoid position miscalculation.
 * regionY: top of the battery section (from drawDashboardFull return).
 */
void updateBatteryPartial(uint16_t voltageMv, int remainingSec, int16_t regionY)
{
    uint16_t screenW = display.width();
    uint16_t screenH = display.height();

    /* Single partial window covering battery + footer */
    int16_t regionH = screenH - regionY;
    display.setPartialWindow(0, regionY, screenW, regionH);
    display.firstPage();
    do {
        display.fillRect(0, regionY, screenW, regionH, GxEPD_WHITE);

        /* ---- Battery label ---- */
        int16_t y = regionY + 2;
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        u8g2Fonts.setCursor(4, y + 10);
        u8g2Fonts.print("Battery");
        y += 14;

        /* ---- Battery voltage value ---- */
        float volts = voltageMv / 1000.0;
        String voltStr = String(volts, 2) + " V  (" + String(voltageMv) + " mV)";
        int16_t vw = u8g2Fonts.getUTF8Width(voltStr.c_str());
        u8g2Fonts.setCursor((screenW - vw) / 2, y + 10);
        u8g2Fonts.print(voltStr);
        y += 14;

        /* ---- Divider ---- */
        drawDivider(y, screenW - 8);
        y += 4;

        /* ---- Progress bar ---- */
        int16_t barY = y + 1;
        int16_t barH = 3;
        int16_t barMargin = 10;
        int16_t barW = screenW - barMargin * 2;
        int16_t barFillW = (barW * remainingSec) / 30;
        display.drawRect(barMargin, barY, barW, barH, GxEPD_BLACK);
        if (barFillW > 0) {
            display.fillRect(barMargin + 1, barY + 1, barFillW - 1, barH - 1, GxEPD_BLACK);
        }

        /* ---- Countdown text ---- */
        u8g2Fonts.setFont(u8g2_font_6x12_tf);
        String countStr = "Sleep in " + String(remainingSec) + "s...";
        int16_t cw = u8g2Fonts.getUTF8Width(countStr.c_str());
        u8g2Fonts.setCursor((screenW - cw) / 2, screenH - 6);
        u8g2Fonts.print(countStr);
    } while (display.nextPage());
    display.nextPageToPrevious();
}


void setup()
{
    bool rlst = false;
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

    // Power on the EPD
    pinMode(EPD_PWR_EN, OUTPUT);
    digitalWrite(EPD_PWR_EN, HIGH);

    SPI.begin(/*SCK*/ 18, /*MISO*/ 19, /*MOSI*/ 23);
    display.init(115200, true, 2, false);
    display.setTextColor(GxEPD_BLACK);

    /* Show scanning screen before WiFi scan starts */
    showScanningScreen();

    /* Scan WiFi and get count */
    int wifiCount = scanWiFi();

    /* Setup SD card */
    SDSPI.begin(/*SCLK*/ 14, /*MISO*/ 2, /*MOSI*/ 15);
    rlst = SD.begin(/*CS*/ 13, SDSPI);

    /* Read initial battery voltage */
    uint16_t batteryMv = readBatteryVoltage();
    Serial.printf("Battery voltage: %d mV\n", batteryMv);

    /* Draw the full dashboard and get the battery voltage Y position */
    int16_t voltageY = drawDashboardFull(wifiCount, rlst, batteryMv);

    /* Store voltageY for use in loop() */
    extern int16_t g_voltageY;
    g_voltageY = voltageY;
}

void loop()
{
    if (loopCount >= 30) {
        /* Clear screen before sleep */
        display.setFullWindow();
        display.firstPage();
        do {
            display.fillScreen(GxEPD_WHITE);
        } while (display.nextPage());

        display.powerOff();

        /* Deep sleep for 30 seconds, then wake up and run again */
        esp_sleep_enable_timer_wakeup(30ULL * 1000000ULL);
        esp_deep_sleep_start();
    }

    if (millis() - interval > 1000) {
        loopCount++;
        interval = millis();

        uint16_t batteryMv = readBatteryVoltage();
        int remainingSec = 30 - loopCount;
        Serial.printf("[%ds] Battery: %d mV\n", remainingSec, batteryMv);

        updateBatteryPartial(batteryMv, remainingSec, g_voltageY);
    }
}

