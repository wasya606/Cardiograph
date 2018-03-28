#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H
#include <list>
#include <map>

#include "internal_types.h"
#include "timer.h"
#include "gpio.h"
#include "spi.h"

#define OLED_WIDTH    96  // DO NOT CHANGE THIS
#define OLED_HEIGHT   64  // DO NOT CHANGE THIS

#define COLOR565(R,G,B)  (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))

class OledDriver
{
public:
    enum Color {
      BLACK     = COLOR565(  0,  0,  0), // black
      GREY      = COLOR565(100, 100, 100), // grey
      DARKGREY  = COLOR565(40,  40, 40), // darkgrey
      LIGHTGREY = COLOR565(200, 200, 200), // lightgrey
      GREEN     = COLOR565(  0, 255,  0), // green
      DARKGREEN = COLOR565(  0, 100,  0), // darkgreen
      RED       = COLOR565(255,  0,  0), // red
      DARKRED   = COLOR565(100,  0,  0), // darkred
      YELLOW    = COLOR565(255, 255,  0), // yellow
      DARKYELLOW = COLOR565(100, 100,  0), // darkyellow
      WHITE     = COLOR565(255, 255, 255), // white
      BROWN     = COLOR565(128, 42, 42), // brown
      BLUE      = COLOR565(  0,  0, 255), // blue
      DARKBLUE  = COLOR565(  0,  0, 100), // darkblue
      ROYALBLUE = COLOR565( 65, 105, 225), // royalblue
      DODGERBLUE = COLOR565( 30, 144, 255), // dorgerblue
      NAVY      = COLOR565(  0,  0, 128), // navyblue
      SKYBLUE   = COLOR565(135, 206, 250), // skyblue
      BLUEVIOLET = COLOR565(138, 43, 226), // blueviolet
      CYAN      = COLOR565(  0, 255, 255), // cyan
      DARKCYAN  = COLOR565(  0, 100, 100), // darkcyan
      TEAL      = COLOR565(  0, 128, 128), // teal
      TOMATO    = COLOR565(255, 99, 71), // tomato
      VIOLET    = COLOR565(238, 130, 238), // violet
      PURPLE    = COLOR565(143, 15, 223), // purple
      ORANGE    = COLOR565(245, 78,  9), // orange
      LIGHTORANGE = COLOR565(255, 165, 0), // lightorange
      PINK      = COLOR565(255, 192, 203), // pink
      MAGENTA   = COLOR565(255, 0, 255), // magenta
      SALMON    = COLOR565(250, 128, 114) // salmon
    };

public:
    OledDriver();
    ~OledDriver();

    void init(const uint16_t fillColor = BLACK);

    bool getInitialized() const;

    uint16_t getBackgroundColor() const;

    //DRAW
    void fillScreen(const uint16_t color = BLACK);

    void drawPixel(const uint8_t x, const uint8_t y, const uint16_t color);

    void drawLine(const uint8_t fromX, const uint8_t fromY, const uint8_t toX, const uint8_t toY, const uint16_t color);

    void drawFillRectangle(const uint8_t x, const uint8_t y, const uint8_t width,
                       const uint8_t height, const uint16_t borderColor, const uint16_t fillColor);

    void drawFillRectangle(const uint8_t x, const uint8_t y, const uint8_t width,
                       const uint8_t height, const uint16_t color);

    void drawRectangle(const uint8_t x, const uint8_t y, const uint8_t width,
                       const uint8_t height, const uint16_t color);

    void drawSymbol(const char symbol, const uint8_t x, const uint8_t y, const uint16_t color);
    void drawSymbol(const char symbol, const uint8_t x, const uint8_t y, const uint16_t color, const uint16_t backColor);

    void printText(const char* text, const uint8_t x, const uint8_t y, const uint16_t color);
    void printText(const char* text, const uint8_t x, const uint8_t y, const uint16_t color, const uint16_t backColor);

private:
    enum PinState
    {
        LOW  = GPIO_PIN_RESET,
        HIGH = GPIO_PIN_SET
    };

    enum OledCommand
    {
        DRAW_LINE                      = 0x21,
        DRAW_RECTANGLE                 = 0x22,
        COPY_WINDOW                    = 0x23,
        DIM_WINDOW                     = 0x24,
        CLEAR_WINDOW                   = 0x25,
        FILL_WINDOW                    = 0x26,
        DISABLE_FILL                   = 0x00,
        ENABLE_FILL                    = 0x01,
        CONTINUOUS_SCROLLING_SETUP     = 0x27,
        DEACTIVE_SCROLLING             = 0x2E,
        ACTIVE_SCROLLING               = 0x2F,
        SET_COLUMN_ADDRESS             = 0x15,
        SET_ROW_ADDRESS                = 0x75,
        SET_CONTRAST_A                 = 0x81,
        SET_CONTRAST_B                 = 0x82,
        SET_CONTRAST_C                 = 0x83,
        MASTER_CURRENT_CONTROL         = 0x87,
        SET_PRECHARGE_SPEED_A          = 0x8A,
        SET_PRECHARGE_SPEED_B          = 0x8B,
        SET_PRECHARGE_SPEED_C          = 0x8C,
        SET_REMAP                      = 0xA0,
        SET_DISPLAY_START_LINE         = 0xA1,
        SET_DISPLAY_OFFSET             = 0xA2,
        NORMAL_DISPLAY                 = 0xA4,
        ENTIRE_DISPLAY_ON              = 0xA5,
        ENTIRE_DISPLAY_OFF             = 0xA6,
        INVERSE_DISPLAY                = 0xA7,
        SET_MULTIPLEX_RATIO            = 0xA8,
        DIM_MODE_SETTING               = 0xAB,
        SET_MASTER_CONFIGURE           = 0xAD,
        DIM_MODE_DISPLAY_ON            = 0xAC,
        DISPLAY_OFF                    = 0xAE,
        NORMAL_BRIGHTNESS_DISPLAY_ON   = 0xAF,
        POWER_SAVE_MODE                = 0xB0,
        PHASE_PERIOD_ADJUSTMENT        = 0xB1,
        DISPLAY_CLOCK_DIV              = 0xB3,
        SET_GRAY_SCALE_TABLE           = 0xB8,
        ENABLE_LINEAR_GRAY_SCALE_TABLE = 0xB9,
        SET_PRECHARGE_VOLTAGE          = 0xBB,
        SET_V_VOLTAGE                  = 0xBE
    };

private:
    void sendCommand(const uint8_t cmd);
    void sendData(const uint8_t data);
    void transmitSpi(const uint8_t data);
    void setCS(const PinState state);
    void setDC(const PinState state);
    void setRST(const PinState state);
    void reset();

    void pushColor(const uint16_t color);

private:
    bool isInitialized;
    uint8_t spiBuffer;
    uint16_t backgroundColor;
};

#endif // OLED_DRIVER_H
