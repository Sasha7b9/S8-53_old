#include "CPU.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/Panel.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CRC_HandleTypeDef crcHandle;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPU::Init()
{
    STM207::Init();

    EnablePeriphery();

    RTC_::Init();

    ADC_::Init();

    FSMC::Init();

    crcHandle.Instance = CRC;
    if (HAL_CRC_Init(&crcHandle) != HAL_OK)
    {
        ERROR_HANDLER();
    }

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

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint CPU::CalculateCRC32(uint address, uint numBytes)
{
    return HAL_CRC_Calculate(&crcHandle, (uint *)address, numBytes);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void CPU::EnablePeriphery()
{
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();

    __PWR_CLK_ENABLE();
    __SYSCFG_CLK_ENABLE();
    __USB_OTG_HS_CLK_ENABLE();
}
