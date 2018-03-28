#ifndef _CARDIOGRAPH_H_VDYKYY_
#define _CARDIOGRAPH_H_VDYKYY_
#include <sys/_stdint.h>
#include <list>
#include <stdio.h>
#include "stm32f103xb.h"
#include "oled_driver.h"
#include "timer.h"

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
    static const constexpr uint8_t UART_CMD_ID_LEN = 1;
    static const constexpr uint8_t UART_CMD_CONFIRM_LEN = 3;

    //High byte - command ID, low byte - command expected data length
    //Cmd format: CC:[DD:DD:...], CC - command id byte, DD - input data bytes
    //Response format: [DD:DD:DD:...]:CC:RR, DD - response data bytes, CC - command id byte, RR - result byte
    enum UARTCommand
    {
        NONE_CMD                = 0x0000,
        SET_TIME_CMD            = 'T' << 8 | 0x04,//Expected 4 bytes of input data;
        SET_SAMPLES_CMD         = 'S' << 8 | 0x02,//Expected 2 bytes of input data;
        SET_ADC_CONV_STATE_CMD  = 'C' << 8 | 0x01,//Expected 1 bytes of input data;
        GET_TIME_CMD            = 't' << 8 | 0x00,//No input data expected;
        GET_SAMPLES_CMD         = 's' << 8 | 0x00,//No input data expected;
        REQUEST_DATA_CMD        = 'R' << 8 | 0x00 //No input data expected;
    };

    enum UARTCommandResult
    {
        CMD_OK      = 'O',
        CMD_ERROR   = 'E'
    };

    enum UART_RxState
    {
        WAIT_CMD_ID     = 0x00,
        WAIT_IN_DATA    = 0x01
    };

    enum UART_TxState
    {
        TX_IDLE = 0x00,
        TX_BUSY = 0x01
    };

    enum AdcConversionState
    {
        ADC_IDLE_STATE   = 0x00,
        ADC_ACTIVE_STATE = 0x01
    };

    struct Settings
    {
    public:
        static const constexpr uint32_t TIMER_CLOCK = 1000000;
        static const constexpr uint32_t TIMER_DELAY = 1000;
        static const constexpr uint8_t TIMER_PRESCALER_DIV = 2;

        static void setTimeUs(const uint32_t newTimeUs) { timeUs = newTimeUs; }
        static uint32_t getTimeUs() { return timeUs; }
        static void setTimeMs(const uint16_t timeMs) { timeUs = timeMs * 1000; }
        static uint32_t getTimeMs() { return timeUs / 1000; }
        static void setSamplesCount(const uint16_t newSamplesCount) { samplesCount = newSamplesCount; }
        static uint16_t getSamplesCount()  { return samplesCount; }

        static constexpr uint16_t getTimerPeriod() { return TIMER_DELAY * TIMER_PRESCALER_DIV - 1; }
        static uint16_t getTimerPrescaler() {return (uint16_t)(SystemCoreClock / TIMER_CLOCK / TIMER_PRESCALER_DIV) - 1; }

    private:
        static uint32_t timeUs;
        static uint16_t samplesCount;
    };

    struct UartTxData
    {
        UartTxData(uint8_t* buffer, const uint16_t size) : buffer(buffer), size(size) {}

        uint8_t* buffer;
        uint16_t size;
    };


private:
    void initAdcBuffers();
    void initTimerPeriod();
    void initSoftTimers();
    void initOledDisplay();

    void updateDisplay();
    void updateLO_P_status();
    void updateLO_N_status();
    void decodeCommand();
    void applyCommand();
    void listenUart();
    void transmitData(uint8_t* buffer, const int16_t size);
    void sendCommandResult(const UARTCommandResult cmdResult = CMD_OK);

private:
    bool isAdcConvertionStopping;

    GPIO_PinState LO_N_PinState;
    GPIO_PinState LO_P_PinState;

    uint8_t adcConversionState;
    uint8_t uartRxState;
    uint8_t uartTxState;

    uint16_t activeUartCmd;

    uint8_t* uartRxBuffer;
    uint8_t* uartTxBuffer;
    uint8_t* adcLastConvertedData;
    uint8_t* adcConversionBuffer;

    std::list<UartTxData*>* uartTxQueue;
    OledDriver* oledDisplay;
    Timer* updateDisplayTimer;
    Timer* adcConversionTimer;
};

#endif // _CARDIOGRAPH_H_VDYKYY_
