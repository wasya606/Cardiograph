#include "../Inc/oled_driver.h"
#include "tim.h"
#include <functional>

OledDriver::OledDriver() :
    isSpiBusy(false),
    isDrawBusy(false),
    isInitialized(false),
    spiBuffer(0x00),
    spiTxQueue(new std::list<uint8_t>()),
    timers(new std::map<uint8_t, Timer*>),
    drawQueue(new std::list<DrawRequest*>())
{
    setGlobalTimerDelay(1000);//1ms
    startGlobalTimer();

    (*timers)[DRAW_TIMER] = new Timer(5, [this]()
    {
        //onDrawComplete();
        timers->at(DRAW_TIMER)->stop();
        if (!isInitialized)
            isInitialized = true;
        isDrawBusy = false;
        performDraw();
    });
}

OledDriver::~OledDriver()
{

}

void OledDriver::init(const Callback&&)
{
    auto rstClb = [this]()->void
    {
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

        drawFillRectangle(0, 0, OLED_WIDTH, OLED_HEIGHT, BLACK);

        /*drawFillRectangle(0, 0, OLED_WIDTH, OLED_HEIGHT / 7, RED);
        drawFillRectangle(0, OLED_HEIGHT / 7, OLED_WIDTH, OLED_HEIGHT / 7, ORANGE);
        drawFillRectangle(0, OLED_HEIGHT * 2 / 7, OLED_WIDTH, OLED_HEIGHT / 7, YELLOW);
        drawFillRectangle(0, OLED_HEIGHT * 3 / 7, OLED_WIDTH, OLED_HEIGHT / 7, GREEN);
        drawFillRectangle(0, OLED_HEIGHT * 4 / 7, OLED_WIDTH, OLED_HEIGHT / 7, SKYBLUE);
        drawFillRectangle(0, OLED_HEIGHT * 5 / 7, OLED_WIDTH, OLED_HEIGHT / 7, BLUE);
        drawFillRectangle(0, OLED_HEIGHT * 6 / 7, OLED_WIDTH, OLED_HEIGHT / 7, PURPLE);

        drawRectangle(23, 5, 50, 53, DODGERBLUE);
        drawRectangle(28, 10, 40, 43, LIGHTORANGE);
        drawRectangle(33, 15, 30, 33, WHITE);

        drawLine(0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1, DARKBLUE);
        drawLine(OLED_WIDTH - 1, 0, 0, OLED_HEIGHT - 1, DARKBLUE);*/

    };

    reset(rstClb);
}

void OledDriver::spiTxHandler()
{
    setCS(HIGH);
    isSpiBusy = false;
    transmitSpi();
}

void OledDriver::timerHandler()
{
    if (timers->empty())
        return;

    for (auto it = timers->begin(); it != timers->end(); it++)
    {
        Timer* timer = it->second;
        if (timer != nullptr)
            timer->process();
    }
}

bool OledDriver::getInitialized()
{
    return isInitialized;
}

void OledDriver::drawPixel(const uint8_t x, const uint8_t y, const uint16_t color)
{
    /*
    sendCommand(SET_COLUMN_ADDRESS);
    sendCommand(x);
    sendCommand(x);

    sendCommand(SET_ROW_ADDRESS);
    sendCommand(y);
    sendCommand(y);

    sendData(color >> 8);
    sendData(color);*/

    /*std::list<OledRequest*>* requests = new std::list<OledRequest*>();

    requests->push_back(new OledRequest(SET_COLUMN_ADDRESS));
    requests->push_back(new OledRequest(x));
    requests->push_back(new OledRequest(OLED_WIDTH - 1));

    requests->push_back(new OledRequest(SET_ROW_ADDRESS));
    requests->push_back(new OledRequest(y));
    requests->push_back(new OledRequest(OLED_HEIGHT - 1));

    requests->push_back(new OledRequest((color >> 8) & 0xFF, DATA));
    requests->push_back(new OledRequest(color & 0xFF, DATA));

    drawQueue->push_back(new DrawRequest(requests, 1));
    performDraw();*/

    drawLine(x, y, x, y, color);
}

void OledDriver::drawLine(const uint8_t fromX, const uint8_t fromY, const uint8_t toX, const uint8_t toY, const uint16_t color)
{
    std::list<OledRequest*>* requests = new std::list<OledRequest*>();

    requests->push_back(new OledRequest(DRAW_LINE));

    requests->push_back(new OledRequest(fromX));
    requests->push_back(new OledRequest(fromY));
    requests->push_back(new OledRequest(toX));
    requests->push_back(new OledRequest(toY));
    //drawQueue->push_back(new DrawRequest(requests, 5));

    //requests = new std::list<OledRequest*>();
    pushColor(requests, color);
    drawQueue->push_back(new DrawRequest(requests, 1));

    performDraw();
}

void OledDriver::drawFillRectangle(const uint8_t x, const uint8_t y, const uint8_t width,
                               const uint8_t height, const uint16_t borderColor, const uint16_t fillColor)
{
    std::list<OledRequest*>* requests = new std::list<OledRequest*>();

    requests->push_back(new OledRequest(DRAW_RECTANGLE));

    requests->push_back(new OledRequest(x));
    requests->push_back(new OledRequest(y));
    requests->push_back(new OledRequest(x + width - 1));
    requests->push_back(new OledRequest(y + height - 1));

    //Border color
    pushColor(requests, borderColor);

    //Fill color
    pushColor(requests, fillColor);

    drawQueue->push_back(new DrawRequest(requests, 3));
    performDraw();
}

void OledDriver::drawFillRectangle(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height, const uint16_t color)
{
    drawFillRectangle(x, y, width, height, color, color);
}

void OledDriver::drawRectangle(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height, const uint16_t color)
{
    std::list<OledRequest*>* requests = new std::list<OledRequest*>();
    requests->push_back(new OledRequest(FILL_WINDOW));
    requests->push_back(new OledRequest(0x00));
    drawQueue->push_back(new DrawRequest(requests, 1));

    drawFillRectangle(x, y, width, height, color, color);

    requests = new std::list<OledRequest*>();
    requests->push_back(new OledRequest(FILL_WINDOW));
    requests->push_back(new OledRequest(0x01));
    drawQueue->push_back(new DrawRequest(requests, 1));

    performDraw();
}

void OledDriver::sendCommand(const uint8_t cmd)
{
    setDC(LOW);
    spiTxQueue->push_back(cmd);
    transmitSpi();
}

void OledDriver::sendData(const uint8_t data)
{
    setDC(HIGH);
    spiTxQueue->push_back(data);
    transmitSpi();
}

void OledDriver::transmitSpi()
{
    if (!spiTxQueue->empty() && !isSpiBusy)
    {
        spiBuffer = spiTxQueue->front();
        spiTxQueue->pop_front();
        setCS(LOW);
        isSpiBusy = true;
        HAL_SPI_Transmit_DMA(&hspi1, &spiBuffer, 1);
    }
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

void OledDriver::reset(const Callback&& callback)
{
    auto clb2 = [this, callback]()->void
    {
        setRST(HIGH);
        callback();

        auto it = timers->find(RESET_TIMER);
        if (it != timers->end())
        {
            Timer* timer = it->second;
            if (timer != nullptr)
                delete timer;
            timers->erase(it);
        }
    };

    auto clb1 = [this, clb2]()->void
    {
        setRST(LOW);
        auto it = timers->find(RESET_TIMER);
        if (it != timers->end())
        {
            Timer* timer = it->second;
            if (timer != nullptr)
            {
                timer->setCallback(clb2);
                timer->start();
            }
        }
    };

    Timer* resetTimer = new Timer(50, clb1);
    (*timers)[RESET_TIMER] = resetTimer;
    setRST(HIGH);

    resetTimer->start();
}

void OledDriver::setGlobalTimerDelay(const uint32_t delay_us)
{
    MX_TIM2_Init_Custom(72, delay_us - 1);
}

void OledDriver::startGlobalTimer()
{
    HAL_TIM_Base_Start_IT(&htim2);
}

void OledDriver::stopGlobalTimer()
{
    HAL_TIM_Base_Stop_IT(&htim2);
}

void OledDriver::pushColor(std::list<OledDriver::OledRequest *>* requests, const uint16_t color)
{
    if (requests != nullptr)
    {
        requests->push_back(new OledRequest((uint8_t)((color >> 11) << 1)));
        requests->push_back(new OledRequest((uint8_t)((color >> 5) & 0x3F)));
        requests->push_back(new OledRequest((uint8_t)((color << 1) & 0x3F)));
    }
}

void OledDriver::performDraw()
{
    if (!isDrawBusy && !drawQueue->empty())
    {
        DrawRequest* drawRequest = drawQueue->front();
        drawQueue->pop_front();

        if (drawRequest == nullptr || drawRequest->oledRequests->empty())
            return;

        isDrawBusy = true;

        for (auto it = drawRequest->oledRequests->begin(); it != drawRequest->oledRequests->end(); it++)
        {
            OledRequest* request = *it;
            if (request != nullptr)
            {
                if (request->type == COMMAND)
                {
                    sendCommand(request->data);
                }
                else if (request->type == DATA)
                {
                    sendData(request->data);
                }
            }
        }
        timers->at(DRAW_TIMER)->setDelay(drawRequest->delay);
        delete drawRequest;
        timers->at(DRAW_TIMER)->start();
    }
}

void OledDriver::onDrawComplete()
{
    timers->at(DRAW_TIMER)->stop();
    isDrawBusy = false;
    performDraw();
}
