#include "oled_driver.h"
#include "tim.h"
#include <functional>
#include "oledfont.h"

OledDriver::OledDriver() :
    isInitialized(false),
    spiBuffer(0x00),
    backgroundColor(BLACK)
{
}

OledDriver::~OledDriver()
{
}

void OledDriver::init(const uint16_t fillColor)
{
    reset();

    sendCommand(DISPLAY_OFF);
    sendCommand(CLEAR_WINDOW);
    sendCommand(SET_CONTRAST_A);
    sendCommand(0x80);
    sendCommand(SET_CONTRAST_B);
    sendCommand(0x80);
    sendCommand(SET_CONTRAST_C);
    sendCommand(0x80);
    sendCommand(MASTER_CURRENT_CONTROL);
    sendCommand(12);
    sendCommand(SET_PRECHARGE_SPEED_A);
    sendCommand(0x64);
    sendCommand(SET_PRECHARGE_SPEED_B);
    sendCommand(0x78);
    sendCommand(SET_PRECHARGE_SPEED_C);
    sendCommand(0x64);
    sendCommand(SET_REMAP);
    sendCommand(0B01110010);
    sendCommand(SET_DISPLAY_START_LINE);
    sendCommand(0x0);
    sendCommand(SET_DISPLAY_OFFSET);
    sendCommand(0x0);
    sendCommand(NORMAL_DISPLAY);
    sendCommand(SET_MULTIPLEX_RATIO);
    sendCommand(0x3F);
    sendCommand(SET_MASTER_CONFIGURE);
    sendCommand(0x8E);
    sendCommand(POWER_SAVE_MODE);
    sendCommand(0x01);
    sendCommand(PHASE_PERIOD_ADJUSTMENT);
    sendCommand(0x31);
    sendCommand(DISPLAY_CLOCK_DIV);
    sendCommand(0xF0);
    sendCommand(SET_PRECHARGE_VOLTAGE);
    sendCommand(0x3A);
    sendCommand(SET_V_VOLTAGE);
    sendCommand(0x3E);
    sendCommand(DEACTIVE_SCROLLING);
    sendCommand(NORMAL_BRIGHTNESS_DISPLAY_ON);
    sendCommand(FILL_WINDOW);
    sendCommand(0x01);

    fillScreen(fillColor);

    isInitialized = true;
}

bool OledDriver::getInitialized() const
{
    return isInitialized;
}

uint16_t OledDriver::getBackgroundColor() const
{
    return backgroundColor;
}

void OledDriver::fillScreen(const uint16_t color)
{
    backgroundColor = color;
    for (uint8_t y = 0; y < 64; y += 16)
    {
        for (uint8_t x = 0; x < 96; x += 16)
        {
            drawFillRectangle(x, y, 16, 16, color);
        }
    }
}

void OledDriver::drawPixel(const uint8_t x, const uint8_t y, const uint16_t color)
{
    //printf("drawPixel x: %d, y: %d, color: %d\n", x, y, color);
    sendCommand(SET_COLUMN_ADDRESS);
    sendCommand(x);
    sendCommand(OLED_WIDTH - 1);

    sendCommand(SET_ROW_ADDRESS);
    sendCommand(y);
    sendCommand(OLED_HEIGHT - 1);

    setDC(HIGH);
    setCS(LOW);

    transmitSpi(color >> 8);
    transmitSpi(color);

    setCS(HIGH);
}

void OledDriver::drawLine(const uint8_t fromX, const uint8_t fromY, const uint8_t toX, const uint8_t toY, const uint16_t color)
{
    /*std::list<OledRequest*>* requests = new std::list<OledRequest*>();

    requests->push_back(new OledRequest(DRAW_LINE));

    requests->push_back(new OledRequest(fromX));
    requests->push_back(new OledRequest(fromY));
    requests->push_back(new OledRequest(toX));
    requests->push_back(new OledRequest(toY));
    //drawQueue->push_back(new DrawRequest(requests, 5));

    //requests = new std::list<OledRequest*>();
    pushColor(requests, color);
    drawQueue->push_back(new DrawRequest(requests, 1));

    performDraw();*/
}

void OledDriver::drawFillRectangle(const uint8_t x, const uint8_t y, const uint8_t width,
                               const uint8_t height, const uint16_t borderColor, const uint16_t fillColor)
{

    sendCommand(DRAW_RECTANGLE);
    sendCommand(x);
    sendCommand(y);
    sendCommand(x + width);
    sendCommand(y + height);


    //Border color
    pushColor(borderColor);

    //Fill color
    pushColor(fillColor);

    //drawQueue->push_back(new DrawRequest(requests, 1));
    //performDraw();
}

void OledDriver::drawFillRectangle(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height, const uint16_t color)
{
    drawFillRectangle(x, y, width, height, color, color);
}

void OledDriver::drawRectangle(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height, const uint16_t color)
{
    /*std::list<OledRequest*>* requests = new std::list<OledRequest*>();
    requests->push_back(new OledRequest(FILL_WINDOW));
    requests->push_back(new OledRequest(0x00));
    drawQueue->push_back(new DrawRequest(requests, 1));

    drawFillRectangle(x, y, width, height, color, color);

    requests = new std::list<OledRequest*>();
    requests->push_back(new OledRequest(FILL_WINDOW));
    requests->push_back(new OledRequest(0x01));
    drawQueue->push_back(new DrawRequest(requests, 1));

    performDraw();*/
}

void OledDriver::drawSymbol(const char symbol, const uint8_t x, const uint8_t y, const uint16_t color)
{
    if (symbol >= ' ' && symbol <= '~')
    {
        const uint64_t data = OledFont::getData(symbol);

        for (uint8_t row = 0; row < 8; row ++)
        {
            uint8_t rowData = (data >> row * 8) & 0xFF;
            for (uint8_t col = 0; col < 8; col++)
            {
                if (((rowData >> col) & 0b1) == 1)
                    drawPixel(x + col, y + row, color);
            }
        }
    }
}

void OledDriver::drawSymbol(const char symbol, const uint8_t x, const uint8_t y, const uint16_t color, const uint16_t backColor)
{
    drawFillRectangle(x, y, 8, 8, backColor);
    drawSymbol(symbol, x, y, color);
}

void OledDriver::printText(const char *text, const uint8_t x, const uint8_t y, const uint16_t color)
{
    uint8_t symbolIndex = (y / 8) * 12 + x / 8;
    uint8_t index = 0;
    while (symbolIndex < 96 && text[index] != '\0')
    {
        uint8_t s_y = (symbolIndex / 12) * 8;
        uint8_t s_x = (symbolIndex % 12) * 8;
        drawSymbol(text[index], s_x, s_y, color);
        symbolIndex++;
        index++;
    }
}

void OledDriver::printText(const char *text, const uint8_t x, const uint8_t y, const uint16_t color, const uint16_t backColor)
{
    uint8_t symbolIndex = (y / 8) * 12 + x / 8;
    uint8_t index = 0;
    while (symbolIndex < 96 && text[index] != '\0')
    {
        uint8_t s_y = (symbolIndex / 12) * 8;
        uint8_t s_x = (symbolIndex % 12) * 8;
        drawSymbol(text[index], s_x, s_y, color, backColor);
        symbolIndex++;
        index++;
    }

}

void OledDriver::sendCommand(const uint8_t cmd)
{
    setDC(LOW);
    setCS(LOW);
    transmitSpi(cmd);
    setCS(HIGH);
}

void OledDriver::sendData(const uint8_t data)
{
    setDC(HIGH);
    setCS(LOW);
    transmitSpi(data);
    setCS(HIGH);
}

void OledDriver::transmitSpi(const uint8_t data)
{
    spiBuffer = data;
    HAL_SPI_Transmit(&hspi1, &spiBuffer, 1, 1000);
}

void OledDriver::setCS(const PinState state)
{
    HAL_GPIO_WritePin(Dsp_CS_GPIO_Port, Dsp_CS_Pin, (GPIO_PinState)state);
}

void OledDriver::setDC(const PinState state)
{
    HAL_GPIO_WritePin(Dsp_DC_GPIO_Port, Dsp_DC_Pin, (GPIO_PinState)state);
}

void OledDriver::setRST(const OledDriver::PinState state)
{
    HAL_GPIO_WritePin(Dsp_RST_GPIO_Port, Dsp_RST_Pin, (GPIO_PinState)state);
}

void OledDriver::reset()
{
    setRST(HIGH);
    setRST(LOW);
    setRST(HIGH);
}

void OledDriver::pushColor(const uint16_t color)
{
    sendCommand((uint8_t)((color >> 11) << 1));
    sendCommand((uint8_t)((color >> 5) & 0x3F));
    sendCommand((uint8_t)((color << 1) & 0x3F));
}
