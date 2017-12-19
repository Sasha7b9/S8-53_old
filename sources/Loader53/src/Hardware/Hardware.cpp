#include "defines.h"
#include "Hardware.h"
#include "FLASH.h"
#include "FSMC.h"
#include "Hardware/Timer.h"
#include "Display/Display.h"
#include "FlashDrive/FlashDrive.h"
#include "Panel/Panel.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void SystemClock_Config();


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Hardware::Init() 
{
    HAL_Init();

    SystemClock_Config();

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __DMA1_CLK_ENABLE();        // Для DAC1 (бикалка)
    __TIM7_CLK_ENABLE();        // Для DAC1 (бикалка)
    __DAC_CLK_ENABLE();         // Для бикалки
    __PWR_CLK_ENABLE();
    __SYSCFG_CLK_ENABLE();

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    Panel_Init();

    Timer::Init();

    FSMC_Init();
    
// Analog and DAC programmable SPI ////////////////////////////////////////

    GPIO_InitTypeDef isGPIOG =
    {
        GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_7,     // GPIO_PIN_1 - для работы с дисплеем
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SPEED_HIGH,
        GPIO_AF0_MCO
    };
    HAL_GPIO_Init(GPIOG, &isGPIOG);

    isGPIOG.Pin = GPIO_PIN_1;
    isGPIOG.Mode = GPIO_MODE_OUTPUT_PP;
    isGPIOG.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOG, &isGPIOG);
    
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);                   // PG1 - когда равен 1, чтение дисплея, в остальных случаях 0

}

void Hardware::DeInit()
{
    HAL_DeInit();

    __GPIOA_CLK_DISABLE();
    __GPIOB_CLK_DISABLE();
    __GPIOC_CLK_DISABLE();
    __GPIOD_CLK_DISABLE();
    __GPIOE_CLK_DISABLE();
    __GPIOF_CLK_DISABLE();
    __GPIOG_CLK_DISABLE();
    __DMA1_CLK_DISABLE();        // Для DAC1 (бикалка)

    __TIM7_CLK_DISABLE();        // Для DAC1 (бикалка)
    __DAC_CLK_DISABLE();         // Для бикалки
    __PWR_CLK_DISABLE();

    __SYSCFG_CLK_DISABLE();

    Timer::DeInit();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SystemClock_Config()
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    // Enable HSE Oscillator and activate PLL with HSE as source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 240;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        ERROR_HANDLER();
    };
    // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        ERROR_HANDLER();
    };
}
