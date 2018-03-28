#include "cardiograph.h"
#include "usart.h"
#include "tim.h"
#include "adc.h"
#include "string.h"

uint32_t Cardiograph::Settings::timeUs = 1000;
uint16_t Cardiograph::Settings::samplesCount = 128;

Cardiograph::Cardiograph() : isAdcConvertionStopping(false),
                             LO_N_PinState(GPIO_PIN_SET),
                             LO_P_PinState(GPIO_PIN_SET),
                             adcConversionState(ADC_IDLE_STATE),
                             uartRxState(WAIT_CMD_ID),
                             uartTxState(TX_IDLE),
                             activeUartCmd(NONE_CMD),
                             uartRxBuffer(nullptr),
                             uartTxBuffer(nullptr),
                             adcLastConvertedData(nullptr),
                             adcConversionBuffer(nullptr),
                             uartTxQueue(new std::list<UartTxData*>()),
                             oledDisplay(new OledDriver()),
                             updateDisplayTimer(new Timer()),
                             adcConversionTimer(new Timer())
{

}

Cardiograph::~Cardiograph()
{
    if (uartRxBuffer != nullptr)
        delete[] uartRxBuffer;

    if (uartTxBuffer != nullptr)
        delete[] uartTxBuffer;

    if (adcLastConvertedData != nullptr)
        delete[] adcLastConvertedData;

    if (adcConversionBuffer != nullptr)
        delete[] adcConversionBuffer;

    if (uartTxQueue != nullptr)
        delete uartTxQueue;

    if (oledDisplay != nullptr)
        delete oledDisplay;

    if (updateDisplayTimer != nullptr)
        delete updateDisplayTimer;

    if (adcConversionTimer != nullptr)
        delete adcConversionTimer;

}

void Cardiograph::init()
{
    initAdcBuffers();
    initTimerPeriod();
    initSoftTimers();
    listenUart();
    initOledDisplay();
    HAL_TIM_Base_Start_IT(&htim1);
}

void Cardiograph::uartRxHandler()
{
    switch (uartRxState)
    {
        case WAIT_CMD_ID:
        {
            decodeCommand();
            break;
        }
        case WAIT_IN_DATA:
        {
            if (activeUartCmd != NONE_CMD)
            {
                applyCommand();
            }
            else
            {
                uartRxState = WAIT_CMD_ID;
                listenUart();
            }
            break;
        }
        default:
        {
            activeUartCmd = NONE_CMD;
            uartRxState = WAIT_CMD_ID;
            listenUart();
            break;
        }
    }
}

void Cardiograph::uartTxHandler()
{
    if (uartTxBuffer != nullptr && uartTxBuffer != adcConversionBuffer)
    {
        delete[] uartTxBuffer;
        uartTxBuffer = nullptr;
    }

    if (uartTxQueue->size() > 0)
    {
        UartTxData* txData = uartTxQueue->front();
        uartTxBuffer = txData->buffer;
        HAL_UART_Transmit_DMA(&huart1, uartTxBuffer, txData->size);
        uartTxQueue->pop_front();
        delete txData;
    }
    else
    {
        uartTxState = TX_IDLE;
    }
}

void Cardiograph::adcConversionHandler()
{
    if (isAdcConvertionStopping == true)
    {
        adcConversionTimer->stop();
        isAdcConvertionStopping = false;
    }
    //memcpy(adcLastConvertedData, adcConversionBuffer, Settings::getSamplesCount() * 2);
}

void Cardiograph::timerPeriodElapsedHandler()
{
    updateDisplayTimer->process();
    adcConversionTimer->process();
}

void Cardiograph::initAdcBuffers()
{
    const size_t bufferLen = Settings::getSamplesCount() * 2;
    if (adcConversionBuffer != nullptr)
        delete[] adcConversionBuffer;

    //if (adcLastConvertedData != nullptr)
    //   delete[] adcLastConvertedData;

    adcConversionBuffer = new uint8_t[bufferLen]{0};
    //adcLastConvertedData = new uint8_t[bufferLen]{0};
}

void Cardiograph::initTimerPeriod()
{
    //printf("initTimerPeriod, prescaler: %d, period: %d, time: %d\n",
    //       Settings::getTimerPrescaler(), Settings::getTimerPeriod(), Settings::getTimeUs());
    MX_TIM1_Init_Custom(Settings::getTimerPrescaler(), Settings::getTimerPeriod());
}

void Cardiograph::initSoftTimers()
{
    adcConversionTimer->setCallback([this]()
    {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcConversionBuffer, Settings::getSamplesCount());
    });
    adcConversionTimer->setDelay(Settings::getTimeMs());

    updateDisplayTimer->setCallback([this](){updateDisplay();});
    updateDisplayTimer->setDelay(100);
    updateDisplayTimer->start();
}

void Cardiograph::initOledDisplay()
{
    oledDisplay->init(OledDriver::DARKBLUE);
    oledDisplay->printText("Status:", 1, 0, OledDriver::WHITE);
    oledDisplay->printText("LO+: ", 1, 9, OledDriver::YELLOW);
    oledDisplay->printText("LO-: ", 1, 18, OledDriver::YELLOW);
    LO_N_PinState = HAL_GPIO_ReadPin(LO_N_GPIO_Port, LO_N_Pin);
    LO_P_PinState = HAL_GPIO_ReadPin(LO_P_GPIO_Port, LO_P_Pin);
    updateLO_N_status();
    updateLO_P_status();
}

void Cardiograph::updateDisplay()
{
    const GPIO_PinState LO_N_NewPinState = HAL_GPIO_ReadPin(LO_N_GPIO_Port, LO_N_Pin);
    const GPIO_PinState LO_P_NewPinState = HAL_GPIO_ReadPin(LO_P_GPIO_Port, LO_P_Pin);

    if (LO_N_NewPinState != LO_N_PinState)
    {
        LO_N_PinState = LO_N_NewPinState;
        updateLO_N_status();
    }
    if (LO_P_NewPinState != LO_P_PinState)
    {
        LO_P_PinState = LO_P_NewPinState;
        updateLO_P_status();
    }
}

void Cardiograph::updateLO_P_status()
{
    const uint16_t backgroundColor = oledDisplay->getBackgroundColor();
    if (LO_P_PinState == GPIO_PIN_RESET)
        oledDisplay->printText("OK  ", 32, 18, OledDriver::GREEN, backgroundColor);
    else
        oledDisplay->printText("FAIL", 32, 18, OledDriver::RED, backgroundColor);
}

void Cardiograph::updateLO_N_status()
{
    const uint16_t backgroundColor = oledDisplay->getBackgroundColor();
    if (LO_N_PinState == GPIO_PIN_RESET)
        oledDisplay->printText("OK  ", 32, 9, OledDriver::GREEN, backgroundColor);
    else
        oledDisplay->printText("FAIL", 32, 9, OledDriver::RED, backgroundColor);
}

void Cardiograph::decodeCommand()
{

    uartRxState = WAIT_IN_DATA;
    switch (uartRxBuffer[0])
    {
    case SET_TIME_CMD >> 8:
        activeUartCmd = SET_TIME_CMD;
        break;
    case SET_SAMPLES_CMD >> 8:
        activeUartCmd = SET_SAMPLES_CMD;
        break;
    case SET_ADC_CONV_STATE_CMD >> 8:
        activeUartCmd = SET_ADC_CONV_STATE_CMD;
        break;
    case GET_TIME_CMD >> 8:
        activeUartCmd = GET_TIME_CMD;
        break;
    case GET_SAMPLES_CMD >> 8:
        activeUartCmd = GET_SAMPLES_CMD;
        break;
    case REQUEST_DATA_CMD >> 8:
        activeUartCmd = REQUEST_DATA_CMD;
        break;
    default:
        activeUartCmd = NONE_CMD;
        uartRxState = WAIT_CMD_ID;
        break;
    }
    listenUart();
}

void Cardiograph::applyCommand()
{
    switch (activeUartCmd)
    {
    case SET_TIME_CMD:
    {
        Settings::setTimeUs((uartRxBuffer[0] << 24) | (uartRxBuffer[1] << 16) | (uartRxBuffer[2] << 8) | uartRxBuffer[3]);
        printf("SET_TIME_CMD: %d, %d\n", Settings::getTimeMs(), Settings::getTimeUs());
        adcConversionTimer->setDelay(Settings::getTimeMs());
        sendCommandResult();
        break;
    }
    case SET_SAMPLES_CMD:
    {
        Settings::setSamplesCount((uartRxBuffer[0] << 8) | uartRxBuffer[1]);
        initAdcBuffers();
        sendCommandResult();
        break;
    }
    case SET_ADC_CONV_STATE_CMD:
    {
        AdcConversionState newAdcState = uartRxBuffer[0] == 0x00 ? ADC_IDLE_STATE : ADC_ACTIVE_STATE;
        if (newAdcState != adcConversionState)
        {
            adcConversionState = newAdcState;
            if (adcConversionState == ADC_ACTIVE_STATE)
            {
                if (isAdcConvertionStopping == true)
                    isAdcConvertionStopping = false;
                else if (!adcConversionTimer->isStarted())
                    adcConversionTimer->start();
            }
            else
            {
                isAdcConvertionStopping = true;
            }
        }
        uint8_t* adcStateDataBuffer = new uint8_t;
        *adcStateDataBuffer = adcConversionState;
        transmitData(adcStateDataBuffer, 1);
        sendCommandResult();
        break;
    }
    case GET_TIME_CMD:
    {
        const uint16_t time = Settings::getTimeUs();
        uint8_t* timeDataBuffer = new uint8_t[4];
        timeDataBuffer[0] = (time >> 24) & 0xFF;
        timeDataBuffer[1] = (time >> 16) & 0xFF;
        timeDataBuffer[2] = (time >> 8) & 0xFF;
        timeDataBuffer[3] = time & 0xFF;
        transmitData(timeDataBuffer, 4);
        sendCommandResult();
        break;
    }
    case GET_SAMPLES_CMD:
    {
        const uint16_t samples = Settings::getSamplesCount();
        uint8_t* samplesDataBuffer = new uint8_t[2];
        samplesDataBuffer[0] = (samples >> 8) & 0xFF;
        samplesDataBuffer[1] = samples & 0xFF;
        transmitData(samplesDataBuffer, 2);
        sendCommandResult();
        break;
    }
    case REQUEST_DATA_CMD:
    {
        transmitData(adcConversionBuffer, Settings::getSamplesCount() * 2);
        sendCommandResult();
        break;
    }
    default:
        sendCommandResult(CMD_ERROR);
        break;
    }
    uartRxState = WAIT_CMD_ID;
    activeUartCmd = NONE_CMD;
    listenUart();
}

void Cardiograph::listenUart()
{
    if (uartRxBuffer != nullptr)
    {
        delete[] uartRxBuffer;
        uartRxBuffer = nullptr;
    }

    const uint8_t uartRxBufferLen = uartRxState == WAIT_CMD_ID ? UART_CMD_ID_LEN : activeUartCmd & 0xFF;\

    if (uartRxBufferLen > 0)
    {
        uartRxBuffer = new uint8_t[uartRxBufferLen]{0x00};
        HAL_UART_Receive_DMA(&huart1, uartRxBuffer, uartRxBufferLen);
    }
    else
    {
        if (activeUartCmd == NONE_CMD)
        {
            sendCommandResult(CMD_ERROR);
            uartRxState = WAIT_CMD_ID;
            activeUartCmd = NONE_CMD;
            listenUart();
        }
        else
        {
            applyCommand();
        }
    }
}

void Cardiograph::transmitData(uint8_t *buffer, const int16_t size)
{
    switch (uartTxState)
    {
    case TX_IDLE:
    {
        uartTxBuffer = buffer;
        HAL_UART_Transmit_DMA(&huart1, uartTxBuffer, size);
        uartTxState = TX_BUSY;
        break;
    }
    case TX_BUSY:
    {
        uartTxQueue->push_back(new UartTxData(buffer, size));
        break;
    }
    default:
        break;
    }
}

void Cardiograph::sendCommandResult(const UARTCommandResult cmdResult)
{
    uint8_t* txBuffer = new uint8_t[UART_CMD_CONFIRM_LEN];

    txBuffer[0] = (activeUartCmd >> 8) & 0xFF;
    txBuffer[1] = cmdResult;
    txBuffer[2] = (activeUartCmd >> 8) & 0xFF;

    transmitData(txBuffer, UART_CMD_CONFIRM_LEN);

}
