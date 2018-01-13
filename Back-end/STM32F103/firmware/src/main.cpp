#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "spi.h"
#include "sys_conf.h"
#include "cardiograph.h"
#include "stm32f1xx_hal_rcc.h"
#include "oled_driver.h"
#include "timer.h"

Cardiograph* cardiograph = nullptr;
OledDriver* oledDisplay = nullptr;

GPIO_PinState lo_n = GPIO_PIN_SET;
GPIO_PinState lo_p = GPIO_PIN_SET;

Timer* myTim1;

void updateDisplay();

int main(void)
{
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    MX_SPI1_Init();

    oledDisplay = new OledDriver();
    oledDisplay->init();

    cardiograph = new Cardiograph();
    cardiograph->init();

    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_SET);

    myTim1 = new Timer(100, []()
    {
        updateDisplay();
    },
    false);

    myTim1->start();
    //updateDisplay();

    printf("*************\n*************\n-----------\n");

    while (1)
    {

    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (cardiograph != nullptr)
        cardiograph->uartRxHandler();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (cardiograph != nullptr)
        cardiograph->uartTxHandler();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        if (cardiograph != nullptr)
            cardiograph->timerPeriodElapsedHandler();

    }
    else if(htim->Instance == TIM2)
    {
        if (oledDisplay != nullptr)
            oledDisplay->timerHandler();

        if (myTim1 != nullptr)
            myTim1->process();
    }
}

void updateDisplay()
{
    if (oledDisplay == nullptr || !oledDisplay->getInitialized())
        return;

    GPIO_PinState new_lo_n = HAL_GPIO_ReadPin(LO_N_GPIO_Port, LO_N_Pin);
    GPIO_PinState new_lo_p = HAL_GPIO_ReadPin(LO_P_GPIO_Port, LO_P_Pin);

    if (new_lo_n != lo_n || new_lo_p != lo_p)
    {
        lo_n = new_lo_n;
        lo_p = new_lo_p;
        uint16_t color = lo_n ? OledDriver::RED : OledDriver::GREEN;
        oledDisplay->drawFillRectangle(5, 10, 20, 20, color);
        color = lo_p ? OledDriver::RED : OledDriver::GREEN;
        oledDisplay->drawFillRectangle(30, 10, 20, 20, color);

    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (cardiograph != nullptr)
        cardiograph->adcConversionHandler();
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (oledDisplay != nullptr)
        oledDisplay->spiTxHandler();
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}
