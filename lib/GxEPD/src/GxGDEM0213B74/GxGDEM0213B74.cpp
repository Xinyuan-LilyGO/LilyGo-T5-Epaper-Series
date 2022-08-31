// class GxGDEM0213B74 : Display class for GDEH0213B73 e-Paper from Dalian Good Display Co., Ltd.: http://www.e-paper-display.com/products_detail/productId=458.html
//
// based on Demo Example from Good Display: http://www.e-paper-display.com/download_detail/downloadsId=810.html
// Controller: SSD1675B : http://www.e-paper-display.com/download_detail/downloadsId=820.html
//
// Author : J-M Zingg
//
// Version : see library.properties
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// Library: https://github.com/ZinggJM/GxEPD

#include "GxGDEM0213B74.h"

//#define DISABLE_DIAGNOSTIC_OUTPUT

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// Partial Update Delay, may have an influence on degradation
#define GxGDEM0213B74_PU_DELAY 300

GxGDEM0213B74::GxGDEM0213B74(GxIO &io, int8_t rst, int8_t busy) :
    GxEPD(GxGDEM0213B74_VISIBLE_WIDTH, GxGDEM0213B74_HEIGHT), IO(io),
    _current_page(-1), _using_partial_mode(false), _diag_enabled(false),
    _rst(rst), _busy(busy)
{
}

void GxGDEM0213B74::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

    // check rotation, move pixel around if necessary
    switch (getRotation()) {
    case 1:
        swap(x, y);
        x = GxGDEM0213B74_VISIBLE_WIDTH - x - 1;
        break;
    case 2:
        x = GxGDEM0213B74_VISIBLE_WIDTH - x - 1;
        y = GxGDEM0213B74_HEIGHT - y - 1;
        break;
    case 3:
        swap(x, y);
        y = GxGDEM0213B74_HEIGHT - y - 1;
        break;
    }
    uint16_t i = x / 8 + y * GxGDEM0213B74_WIDTH / 8;
    if (_current_page < 1) {
        if (i >= sizeof(_buffer)) return;
    } else {
        y -= _current_page * GxGDEM0213B74_PAGE_HEIGHT;
        if ((y < 0) || (y >= GxGDEM0213B74_PAGE_HEIGHT)) return;
        i = x / 8 + y * GxGDEM0213B74_WIDTH / 8;
    }

    if (!color)
        _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
    else
        _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
}

void GxGDEM0213B74::init(uint32_t serial_diag_bitrate)
{
    if (serial_diag_bitrate > 0) {
        Serial.begin(serial_diag_bitrate);
        _diag_enabled = true;
    }
    IO.init();
    IO.setFrequency(4000000); // 4MHz
    if (_rst >= 0) {
        digitalWrite(_rst, HIGH);
        pinMode(_rst, OUTPUT);
    }
    pinMode(_busy, INPUT);
    fillScreen(GxEPD_WHITE);
    _current_page = -1;
    _using_partial_mode = false;
}

void GxGDEM0213B74::fillScreen(uint16_t color)
{
    uint8_t data = (color == GxEPD_BLACK) ? 0xFF : 0x00;
    for (uint16_t x = 0; x < sizeof(_buffer); x++) {
        _buffer[x] = data;
    }
}

void GxGDEM0213B74::update(void)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    _writeCommand(0x24);
    for (uint16_t y = 0; y < GxGDEM0213B74_HEIGHT; y++) {
        for (uint16_t x = 0; x < GxGDEM0213B74_WIDTH / 8; x++) {
            uint16_t idx = y * (GxGDEM0213B74_WIDTH / 8) + x;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _writeCommand(0x26);
    for (uint16_t y = 0; y < GxGDEM0213B74_HEIGHT; y++) {
        for (uint16_t x = 0; x < GxGDEM0213B74_WIDTH / 8; x++) {
            uint16_t idx = y * (GxGDEM0213B74_WIDTH / 8) + x;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Full();
    _PowerOff();
}

void  GxGDEM0213B74::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode)
{
    if (mode & bm_default) mode |= bm_flip_x | bm_invert;
    drawBitmapBM(bitmap, x, y, w, h, color, mode);
}

void GxGDEM0213B74::drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode)
{
    if (_current_page != -1) return;
    // example bitmaps are made for y-decrement, x-increment, for origin on opposite corner
    // bm_flip_y for normal display is rotated but avoids black boarder
    if (mode & bm_default) mode |= bm_flip_y;
    uint8_t ram_entry_mode = 0x03; // y-increment, x-increment : normal mode
    if ((mode & bm_flip_y) && (mode & bm_flip_x)) ram_entry_mode = 0x00; // y-decrement, x-decrement
    else if (mode & bm_flip_y) ram_entry_mode = 0x01; // y-decrement, x-increment
    else if (mode & bm_flip_x) ram_entry_mode = 0x02; // y-increment, x-decrement
    if (mode & bm_partial_update) {
        _using_partial_mode = true; // remember
        _Init_Part(ram_entry_mode);
        for (uint8_t command = 0x24; true; command = 0x26) {
            // leave both controller buffers equal
            _writeCommand(0x24);
            for (uint32_t i = 0; i < GxGDEM0213B74_BUFFER_SIZE; i++) {
                uint8_t data = 0xFF; // white is 0xFF on device
                if (i < size) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                    data = pgm_read_byte(&bitmap[i]);
#else
                    data = bitmap[i];
#endif
                    if (mode & bm_invert) data = ~data;
                }
                _writeData(data);
            }
            if (command == 0x26) break;
            _Update_Part();
        }
        delay(GxGDEM0213B74_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(ram_entry_mode);
        for (uint8_t command = 0x24; true; command = 0x26) {
            // leave both controller buffers equal
            _writeCommand(command);
            for (uint32_t i = 0; i < GxGDEM0213B74_BUFFER_SIZE; i++) {
                uint8_t data = 0xFF; // white is 0xFF on device
                if (i < size) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                    data = pgm_read_byte(&bitmap[i]);
#else
                    data = bitmap[i];
#endif
                    if (mode & bm_invert) data = ~data;
                }
                _writeData(data);
            }
            if (command == 0x26) break;
        }
        _Update_Full();
        _PowerOff();
    }
}

void GxGDEM0213B74::eraseDisplay(bool using_partial_update)
{
    if (_current_page != -1) return;
    if (using_partial_update) {
        _using_partial_mode = true; // remember
        _Init_Part(0x01);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDEM0213B74_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Part();
        delay(GxGDEM0213B74_PU_DELAY);
        // update erase buffer
        _writeCommand(0x26);
        for (uint32_t i = 0; i < GxGDEM0213B74_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        delay(GxGDEM0213B74_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(0x01);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDEM0213B74_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        // update erase buffer
        _writeCommand(0x26);
        for (uint32_t i = 0; i < GxGDEM0213B74_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Full();
        _PowerOff();
    }
}

void GxGDEM0213B74::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
    if (_current_page != -1) return;
    if (using_rotation) _rotate(x, y, w, h);
    if (x >= GxGDEM0213B74_WIDTH) return;
    if (y >= GxGDEM0213B74_HEIGHT) return;
    uint16_t xe = gx_uint16_min(GxGDEM0213B74_WIDTH, x + w) - 1;
    uint16_t ye = gx_uint16_min(GxGDEM0213B74_HEIGHT, y + h) - 1;
    uint16_t xs_d8 = x / 8;
    uint16_t xe_d8 = xe / 8;
    _Init_Part(0x03);
    _SetRamArea(xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, y % 256, y / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x24);
    for (int16_t y1 = y; y1 <= ye; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxGDEM0213B74_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Part();
    delay(GxGDEM0213B74_PU_DELAY);
    // update erase buffer
    _SetRamArea(xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, y % 256, y / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x26);
    for (int16_t y1 = y; y1 <= ye; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxGDEM0213B74_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Part();
    delay(GxGDEM0213B74_PU_DELAY);
}

void GxGDEM0213B74::_writeToWindow(uint8_t command, uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h)
{
    //Serial.printf("_writeToWindow(%d, %d, %d, %d, %d, %d)\n", xs, ys, xd, yd, w, h);
    // the screen limits are the hard limits
    if (xs >= GxGDEM0213B74_WIDTH) return;
    if (ys >= GxGDEM0213B74_HEIGHT) return;
    if (xd >= GxGDEM0213B74_WIDTH) return;
    if (yd >= GxGDEM0213B74_HEIGHT) return;
    w = gx_uint16_min(w, GxGDEM0213B74_WIDTH - xs);
    w = gx_uint16_min(w, GxGDEM0213B74_WIDTH - xd);
    h = gx_uint16_min(h, GxGDEM0213B74_HEIGHT - ys);
    h = gx_uint16_min(h, GxGDEM0213B74_HEIGHT - yd);
    uint16_t xds_d8 = xd / 8;
    uint16_t xde_d8 = (xd + w - 1) / 8;
    uint16_t yde = yd + h - 1;
    // soft limits, must send as many bytes as set by _SetRamArea
    uint16_t xse_d8 = xs / 8 + xde_d8 - xds_d8;
    uint16_t yse = ys + h - 1;
    _SetRamArea(xds_d8, xde_d8, yd % 256, yd / 256, yde % 256, yde / 256); // X-source area,Y-gate area
    _SetRamPointer(xds_d8, yd % 256, yd / 256); // set ram
    _waitWhileBusy();
    _writeCommand(command);
    for (int16_t y1 = ys; y1 <= yse; y1++) {
        for (int16_t x1 = xs / 8; x1 <= xse_d8; x1++) {
            uint16_t idx = y1 * (GxGDEM0213B74_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
}

void GxGDEM0213B74::updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{
    if (using_rotation) {
        switch (getRotation()) {
        case 1:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            xs = GxGDEM0213B74_WIDTH - xs - w - 1;
            xd = GxGDEM0213B74_WIDTH - xd - w - 1;
            break;
        case 2:
            xs = GxGDEM0213B74_WIDTH - xs - w - 1;
            ys = GxGDEM0213B74_HEIGHT - ys - h - 1;
            xd = GxGDEM0213B74_WIDTH - xd - w - 1;
            yd = GxGDEM0213B74_HEIGHT - yd - h - 1;
            break;
        case 3:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            ys = GxGDEM0213B74_HEIGHT - ys  - h - 1;
            yd = GxGDEM0213B74_HEIGHT - yd  - h - 1;
            break;
        }
    }
    _Init_Part(0x03);
    _writeToWindow(0x24, xs, ys, xd, yd, w, h);
    _Update_Part();
    delay(GxGDEM0213B74_PU_DELAY);
    // update erase buffer
    _writeToWindow(0x26, xs, ys, xd, yd, w, h);
    delay(GxGDEM0213B74_PU_DELAY);
}

void GxGDEM0213B74::powerDown()
{
    _using_partial_mode = false;
    _PowerOff();
}

void GxGDEM0213B74::_writeCommand(uint8_t command)
{
    if (digitalRead(_busy)) {
        String str = String("command 0x") + String(command, HEX);
        _waitWhileBusy(str.c_str());
    }
    IO.writeCommandTransaction(command);
}

void GxGDEM0213B74::_writeData(uint8_t data)
{
    IO.writeDataTransaction(data);
}

void GxGDEM0213B74::_writeData(const uint8_t *data, uint16_t n)
{
    IO.startTransaction();
    for (uint16_t i = 0; i < n; i++) {
        IO.writeData(*data++);
    }
    IO.endTransaction();
}

void GxGDEM0213B74::_waitWhileBusy(const char *comment)
{
    unsigned long start = micros();
    while (1) {
        if (!digitalRead(_busy)) break;
        delay(1);
        if (micros() - start > 10000000) {
            if (_diag_enabled) Serial.println("Busy Timeout!");
            break;
        }
    }
    if (comment) {
#if !defined(DISABLE_DIAGNOSTIC_OUTPUT)
        if (_diag_enabled) {
            unsigned long elapsed = micros() - start;
            Serial.print(comment);
            Serial.print(" : ");
            Serial.println(elapsed);
        }
#endif
    }
    (void) start;
}

void GxGDEM0213B74::_setRamDataEntryMode(uint8_t em)
{
    const uint16_t xPixelsPar = GxGDEM0213B74_X_PIXELS - 1;
    const uint16_t yPixelsPar = GxGDEM0213B74_Y_PIXELS - 1;
    em = gx_uint16_min(em, 0x03);
    _writeCommand(0x11);
    _writeData(em);
    switch (em) {
    case 0x00: // x decrease, y decrease
        _SetRamArea(xPixelsPar / 8, 0x00, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
        _SetRamPointer(xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256); // set ram
        break;
    case 0x01: // x increase, y decrease : as in demo code
        _SetRamArea(0x00, xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
        _SetRamPointer(0x00, yPixelsPar % 256, yPixelsPar / 256); // set ram
        break;
    case 0x02: // x decrease, y increase
        _SetRamArea(xPixelsPar / 8, 0x00, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
        _SetRamPointer(xPixelsPar / 8, 0x00, 0x00); // set ram
        break;
    case 0x03: // x increase, y increase : normal mode
        _SetRamArea(0x00, xPixelsPar / 8, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
        _SetRamPointer(0x00, 0x00, 0x00); // set ram
        break;
    }
}

void GxGDEM0213B74::_SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
    _writeCommand(0x44);
    _writeData(Xstart);
    _writeData(Xend);
    _writeCommand(0x45);
    _writeData(Ystart);
    _writeData(Ystart1);
    _writeData(Yend);
    _writeData(Yend1);
}

void GxGDEM0213B74::_SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1)
{
    _writeCommand(0x4e);
    _writeData(addrX);
    _writeCommand(0x4f);
    _writeData(addrY);
    _writeData(addrY1);
}

void GxGDEM0213B74::_PowerOn(void)
{
    // _writeCommand(0x22);
    // _writeData(0xc0);
    // _writeCommand(0x20);
    // _waitWhileBusy("_PowerOn");
}

void GxGDEM0213B74::_PowerOff(void)
{
    _writeCommand(0x10);
    _writeData(0x01);
    // _writeCommand(0x20);
    // _waitWhileBusy("_PowerOff");
}

void GxGDEM0213B74::_InitDisplay(uint8_t em)
{
    digitalWrite(_rst, 0);
    delay(10);
    digitalWrite(_rst, 1);
    delay(10);
    _waitWhileBusy();

    _writeCommand(0x12);  //SWRESET
    _waitWhileBusy();

    _writeCommand(0x01); //Driver output control
    _writeData(0xF9);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x11); //data entry mode
    _writeData(0x01);

    _writeCommand(0x44); //set Ram-X address start/end position
    _writeData(0x00);
    _writeData(0x0F);    //0x0F-->(15+1)*8=128

    _writeCommand(0x45); //set Ram-Y address start/end position
    _writeData(0xF9);   //0xF9-->(249+1)=250
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x3C); //BorderWavefrom
    _writeData(0x05);

    _writeCommand(0x21); //  Display update control
    _writeData(0x00);
    _writeData(0x80);

    _writeCommand(0x18); //Read built-in temperature sensor
    _writeData(0x80);

    _writeCommand(0x4E);   // set RAM x address count to 0;
    _writeData(0x00);
    _writeCommand(0x4F);   // set RAM y address count to 0X199;
    _writeData(0xF9);
    _writeData(0x00);
    _waitWhileBusy();

    _setRamDataEntryMode(em);
}

void GxGDEM0213B74::_Init_Full(uint8_t em)
{
    _InitDisplay(em);
    _PowerOn();
}

void GxGDEM0213B74::_Init_Part(uint8_t em)
{
    _InitDisplay(em);
    _writeCommand(0x3C);
    _writeData(0x80);
    _PowerOn();
}

void GxGDEM0213B74::_Update_Full(void)
{
    _writeCommand(0x22);
    _writeData(0xF7);
    _writeCommand(0x20);
    _waitWhileBusy("_Update_Full");
}

void GxGDEM0213B74::_Update_Part(void)
{
    _writeCommand(0x22);
    _writeData(0xFF);
    _writeCommand(0x20);
    _waitWhileBusy("_Update_Part");
}

void GxGDEM0213B74::drawPaged(void (*drawCallback)(void))
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        _writeCommand(command);
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            for (int16_t y1 = 0; y1 < GxGDEM0213B74_PAGE_HEIGHT; y1++) {
                for (int16_t x1 = 0; x1 < GxGDEM0213B74_WIDTH / 8; x1++) {
                    uint16_t idx = y1 * (GxGDEM0213B74_WIDTH / 8) + x1;
                    uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                    _writeData(~data);
                }
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Full();
    }
    _PowerOff();
}

void GxGDEM0213B74::drawPaged(void (*drawCallback)(uint32_t), uint32_t p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        _writeCommand(command);
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            for (int16_t y1 = 0; y1 < GxGDEM0213B74_PAGE_HEIGHT; y1++) {
                for (int16_t x1 = 0; x1 < GxGDEM0213B74_WIDTH / 8; x1++) {
                    uint16_t idx = y1 * (GxGDEM0213B74_WIDTH / 8) + x1;
                    uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                    _writeData(~data);
                }
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Full();
    }
    _PowerOff();
}

void GxGDEM0213B74::drawPaged(void (*drawCallback)(const void *), const void *p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        _writeCommand(command);
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            for (int16_t y1 = 0; y1 < GxGDEM0213B74_PAGE_HEIGHT; y1++) {
                for (int16_t x1 = 0; x1 < GxGDEM0213B74_WIDTH / 8; x1++) {
                    uint16_t idx = y1 * (GxGDEM0213B74_WIDTH / 8) + x1;
                    uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                    _writeData(~data);
                }
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Full();
    }
    _PowerOff();
}

void GxGDEM0213B74::drawPaged(void (*drawCallback)(const void *, const void *), const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        _writeCommand(command);
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            for (int16_t y1 = 0; y1 < GxGDEM0213B74_PAGE_HEIGHT; y1++) {
                for (int16_t x1 = 0; x1 < GxGDEM0213B74_WIDTH / 8; x1++) {
                    uint16_t idx = y1 * (GxGDEM0213B74_WIDTH / 8) + x1;
                    uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                    _writeData(~data);
                }
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Full();
    }
    _PowerOff();
}

void GxGDEM0213B74::_rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
{
    switch (getRotation()) {
    case 1:
        swap(x, y);
        swap(w, h);
        x = GxGDEM0213B74_WIDTH - x - w - 1;
        break;
    case 2:
        x = GxGDEM0213B74_WIDTH - x - w - 1;
        y = GxGDEM0213B74_HEIGHT - y - h - 1;
        break;
    case 3:
        swap(x, y);
        swap(w, h);
        y = GxGDEM0213B74_HEIGHT - y - h - 1;
        break;
    }
}

void GxGDEM0213B74::drawPagedToWindow(void (*drawCallback)(void), uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            uint16_t yds = gx_uint16_max(y, _current_page * GxGDEM0213B74_PAGE_HEIGHT);
            uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxGDEM0213B74_PAGE_HEIGHT);
            if (yde > yds) {
                fillScreen(GxEPD_WHITE);
                drawCallback();
                uint16_t ys = yds % GxGDEM0213B74_PAGE_HEIGHT;
                _writeToWindow(command, x, ys, x, yds, w, yde - yds);
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Part();
    }
}

void GxGDEM0213B74::drawPagedToWindow(void (*drawCallback)(uint32_t), uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            uint16_t yds = gx_uint16_max(y, _current_page * GxGDEM0213B74_PAGE_HEIGHT);
            uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxGDEM0213B74_PAGE_HEIGHT);
            if (yde > yds) {
                fillScreen(GxEPD_WHITE);
                drawCallback(p);
                uint16_t ys = yds % GxGDEM0213B74_PAGE_HEIGHT;
                _writeToWindow(command, x, ys, x, yds, w, yde - yds);
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Part();
    }
}

void GxGDEM0213B74::drawPagedToWindow(void (*drawCallback)(const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            uint16_t yds = gx_uint16_max(y, _current_page * GxGDEM0213B74_PAGE_HEIGHT);
            uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxGDEM0213B74_PAGE_HEIGHT);
            if (yde > yds) {
                fillScreen(GxEPD_WHITE);
                drawCallback(p);
                uint16_t ys = yds % GxGDEM0213B74_PAGE_HEIGHT;
                _writeToWindow(command, x, ys, x, yds, w, yde - yds);
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Part();
    }
}

void GxGDEM0213B74::drawPagedToWindow(void (*drawCallback)(const void *, const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (uint8_t command = 0x24; true; command = 0x26) {
        // leave both controller buffers equal
        for (_current_page = 0; _current_page < GxGDEM0213B74_PAGES; _current_page++) {
            uint16_t yds = gx_uint16_max(y, _current_page * GxGDEM0213B74_PAGE_HEIGHT);
            uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxGDEM0213B74_PAGE_HEIGHT);
            if (yde > yds) {
                fillScreen(GxEPD_WHITE);
                drawCallback(p1, p2);
                uint16_t ys = yds % GxGDEM0213B74_PAGE_HEIGHT;
                _writeToWindow(command, x, ys, x, yds, w, yde - yds);
            }
        }
        _current_page = -1;
        if (command == 0x26) break;
        _Update_Part();
    }
}

void GxGDEM0213B74::drawCornerTest(uint8_t em)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(em);
    _writeCommand(0x24);
    for (uint32_t y = 0; y < GxGDEM0213B74_HEIGHT; y++) {
        for (uint32_t x = 0; x < GxGDEM0213B74_WIDTH / 8; x++) {
            uint8_t data = 0xFF;
            if ((x < 1) && (y < 8)) data = 0x00;
            if ((x > GxGDEM0213B74_WIDTH / 8 - 3) && (y < 16)) data = 0x00;
            if ((x > GxGDEM0213B74_WIDTH / 8 - 4) && (y > GxGDEM0213B74_HEIGHT - 25)) data = 0x00;
            if ((x < 4) && (y > GxGDEM0213B74_HEIGHT - 33)) data = 0x00;
            _writeData(data);
        }
    }
    _Update_Full();
    _PowerOff();
}
