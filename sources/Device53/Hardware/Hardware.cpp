#include "defines.h"
#include "Hardware.h"
#include "ADC.h"
#include "FLASH.h"
#include "Hardware/Sound.h"
#include "Hardware/RTC.h"
#include "FSMC.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Ethernet/Ethernet.h"
#include "Display/Display.h"
#include "FlashDrive/FlashDrive.h"
#include "Panel/Panel.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CRC_HandleTypeDef crcHandle;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void SystemClock_Config();


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
    
void Hardware_Init()
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
    
    __TIM2_CLK_ENABLE();        // Для тиков
    __TIM7_CLK_ENABLE();        // Для DAC1 (бикалка)
    __PWR_CLK_ENABLE();

    __SYSCFG_CLK_ENABLE();

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    // Таймер для тиков
    TIM_HandleTypeDef tim2handle =
    {
        TIM2,
        {
            0,
            TIM_COUNTERMODE_UP,
            0xffffffff,
            TIM_CLOCKDIVISION_DIV1
        }
    };

    if (HAL_TIM_Base_Init(&tim2handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_TIM_Base_Start(&tim2handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    Sound::Init();
    
    Panel::Init();

    FSMC::Init();

    ADConverter::Init();

    FDrive::Init();
    
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

    RTClock::Init();

    crcHandle.Instance = CRC;
    if (HAL_CRC_Init(&crcHandle) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}

#ifdef __cplusplus
}
#endif

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
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        // Initialization Error
        ERROR_HANDLER();
    }
    // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        // Initialization Error
        ERROR_HANDLER();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint Hardware_CalculateCRC32(uint address, uint numBytes)
{
    return HAL_CRC_Calculate(&crcHandle, (uint *)address, numBytes);
}
