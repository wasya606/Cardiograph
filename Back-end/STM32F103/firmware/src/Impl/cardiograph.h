#ifndef _CARDIOGRAPH_H_VDYKYY_
#define _CARDIOGRAPH_H_VDYKYY_
#include <sys/_stdint.h>
#include "adc_buffer.h"
#include <list>
#include <stdio.h>

class Cardiograph
{

public:
    Cardiograph();
    ~Cardiograph();
    void init();
    void uartRxHandler();
    void uartTxHandler();
    void adcConversionHandler();
    void timerPeriodElapsedHandler();

private:
    static const constexpr uint8_t UART_RX_CMD_LEN = 4;
    static const constexpr uint16_t UART_BUFFER_MAX_SIZE = 5;
    static const constexpr uint8_t ADC_CONVERSIONS_MAX_COUNT = 10;

    enum UartCommandRequest
    {
        SET_TIME            = 0xC0,
        GET_TIME            = 0xC1,
        SET_SAMPLES         = 0xC2,
        GET_SAMPLES         = 0xC3,
        REQUEST_DATA        = 0xC4
    };

    enum UartCommandResponse
    {
        CMD_OK      = 0xF0D0,
        CMD_BUSY    = 0xF0FE,
        CMD_ERROR   = 0xF0FF
    };

    struct Settings
    {
        Settings()
        {
            updatePeriod();
        }

        void updatePeriod()
        {
            period = time * 1000 / samples - 1;
            printf("updatePeriod: %u\n", period);
        }

        uint16_t time = 50;
        uint16_t prescaler = 72;
        uint16_t samples = 128;
        uint8_t adcConversionCount = 1;
        uint16_t period = 1;
    };

    struct UartTxData
    {
        UartTxData(uint8_t* buffer, const uint16_t size) : buffer(buffer), size(size) {}

        uint8_t* buffer;
        uint16_t size;
    };


private:
    void initTimerPeriod();
    void decodeCommand(const uint8_t cmd, const uint16_t data);
    void listenCommand();
    void transmitData(uint8_t* buffer, const int16_t size);
    void sendCommandResponse(const UartCommandResponse response);

private:
    bool isUartTxBusy;
    uint8_t pendingBlocks;
    uint8_t* uartBuffer;
    uint8_t* uartTxBuffer;
    uint32_t* adcConversionBuffer;

    AdcBuffer* adcBuffer;
    Settings* settings;
    std::list<UartTxData*>* uartTxQueue;
};

#endif // _CARDIOGRAPH_H_VDYKYY_
