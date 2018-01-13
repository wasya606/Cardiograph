#include "cardiograph.h"
#include "usart.h"
#include "tim.h"
#include "adc.h"

Cardiograph::Cardiograph() : isUartTxBusy(false),
                             pendingBlocks(0),
                             uartBuffer(new uint8_t[UART_BUFFER_MAX_SIZE]{0}),
                             uartTxBuffer(nullptr),
                             adcConversionBuffer(new uint32_t[ADC_CONVERSIONS_MAX_COUNT]{0}),
                             adcBuffer(nullptr),
                             settings(new Settings()),
                             uartTxQueue(new std::list<UartTxData*>())
{

}

Cardiograph::~Cardiograph()
{
    if (uartBuffer != nullptr)
        delete[] uartBuffer;

    if (adcConversionBuffer != nullptr)
        delete[] adcConversionBuffer;

    if (settings != nullptr)
        delete settings;

}

void Cardiograph::init()
{   
    adcBuffer = new AdcBuffer(settings->samples);
    initTimerPeriod();
    listenCommand();
}

void Cardiograph::uartRxHandler()
{
    printf("UART RX: %X %X %X %X\n", uartBuffer[0], uartBuffer[1], uartBuffer[2], uartBuffer[3]);
    if (uartBuffer[0] == uartBuffer[3])
    {
        uint16_t data = (uartBuffer[1] << 8) | uartBuffer[2];
        decodeCommand(uartBuffer[0], data);
    }
    else
    {
        sendCommandResponse(CMD_ERROR);
    }
}

void Cardiograph::uartTxHandler()
{
    if (uartTxBuffer != nullptr)
    {
        delete[] uartTxBuffer;
        uartTxBuffer = nullptr;
    }

    if (uartTxQueue->size() > 0)
    {
        UartTxData* txData = uartTxQueue->front();
        uartTxBuffer = txData->buffer;
        //printf("Queue size: %u, txSize: %u\n", uartTxQueue->size(), txData->size);
        HAL_UART_Transmit_DMA(&huart1, uartTxBuffer, txData->size);
        uartTxQueue->pop_front();
        delete txData;
    }
    else
    {
        isUartTxBusy = false;
    }
}

void Cardiograph::adcConversionHandler()
{
    /*uint16_t sum = 0;
    for (uint8_t i = 0; i < settings->adcConversionCount / 2; i++)
    {
        sum += ((adcConversionBuffer[i] >> 16) & 0xFFFF);
        sum += (adcConversionBuffer[i] & 0xFFFF);
    }
    adcBuffer->push(sum / settings->adcConversionCount);*/
    adcBuffer->push(adcConversionBuffer[0] & 0x0FFF);
    HAL_ADC_Stop_DMA(&hadc1);

    if (adcBuffer->isFull())
    {
        uint8_t* txBuffer = new uint8_t[adcBuffer->getSize() * 2] {0x00};
        adcBuffer->getBytes(txBuffer);
        transmitData(txBuffer, adcBuffer->getSize() * 2);
        adcBuffer->reset();
        //printf("Pending blocks: %u\n", pendingBlocks);
        if (--pendingBlocks == 0)
        {
            listenCommand();
            HAL_TIM_Base_Stop_IT(&htim1);
        }
    }
}

void Cardiograph::timerPeriodElapsedHandler()
{
    HAL_ADC_Start_DMA(&hadc1, adcConversionBuffer, settings->adcConversionCount);
}

void Cardiograph::initTimerPeriod()
{
    MX_TIM1_Init_Custom(settings->prescaler, settings->period);
}

void Cardiograph::decodeCommand(const uint8_t cmd, const uint16_t data)
{
    uint8_t* txBuffer = nullptr;
    switch (cmd)
    {
    case SET_TIME:
        settings->time = data;
        settings->updatePeriod();
        initTimerPeriod();
        sendCommandResponse(CMD_OK);
        listenCommand();
        break;
    case GET_TIME:
        sendCommandResponse(CMD_OK);
        txBuffer = new uint8_t[2];
        txBuffer[0] = (settings->time >> 8) & 0xFF;
        txBuffer[1] = settings->time & 0xFF;
        transmitData(txBuffer, 2);
        listenCommand();
        break;
    case SET_SAMPLES:
        settings->samples = data;
        settings->updatePeriod();
        adcBuffer->setSize(settings->samples);
        initTimerPeriod();
        sendCommandResponse(CMD_OK);
        listenCommand();
        break;
    case GET_SAMPLES:
        sendCommandResponse(CMD_OK);
        txBuffer = new uint8_t[2];
        txBuffer[0] = (settings->samples >> 8) & 0xFF;
        txBuffer[1] = settings->samples & 0xFF;
        transmitData(txBuffer, 2);
        listenCommand();
        break;
    case REQUEST_DATA:
        if (pendingBlocks == 0)
        {
            sendCommandResponse(CMD_OK);
            pendingBlocks = data & 0xFF;
            HAL_TIM_Base_Start_IT(&htim1);
        }
        else
        {
            sendCommandResponse(CMD_BUSY);
        }
        break;
    default:
        sendCommandResponse(CMD_ERROR);
        break;
    }
}

void Cardiograph::sendCommandResponse(const UartCommandResponse response)
{
    uint8_t* txBuffer = new uint8_t[2];
    txBuffer[0] = (response >> 8) & 0xFF;
    txBuffer[1] = response & 0xFF;
    transmitData(txBuffer, 2);
    //printf("SENT COMMAND RESPONSE: %X\n", response);
}

void Cardiograph::listenCommand()
{
    HAL_UART_Receive_DMA(&huart1, uartBuffer, UART_RX_CMD_LEN);
    //printf("START LISTEN COMMAND\n");
}

void Cardiograph::transmitData(uint8_t *buffer, const int16_t size)
{
    if (!isUartTxBusy)
    {
        uartTxBuffer = buffer;
        HAL_UART_Transmit_DMA(&huart1, uartTxBuffer, size);
        isUartTxBusy = true;
    }
    else
    {
        uartTxQueue->push_back(new UartTxData(buffer, size));
    }
}
