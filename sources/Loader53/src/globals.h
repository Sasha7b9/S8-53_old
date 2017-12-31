#pragma once

#ifdef STM32F207xx
#include <stm32f2xx_hal.h>
#endif

#ifdef STM32F746xx
#include <stm32f7xx_hal.h>
#endif

#include <usbh_def.h>

extern HCD_HandleTypeDef handleHCD;
extern USBH_HandleTypeDef handleUSBH;
extern SPI_HandleTypeDef handleSPI;
