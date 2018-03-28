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

Cardiograph* cardiograph = nullptr;

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

    cardiograph = new Cardiograph();
    cardiograph->init();


    //printf("*************\n*************\n-----------\n");
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
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (cardiograph != nullptr)
        cardiograph->adcConversionHandler();
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}
