#include "usbd_desc.h"
#include "defines.h"
#include "Log.h"
#include "globals.h"
#include "Hardware/Timer.h"
#include <usbd_cdc_interface.h>


#ifdef __cplusplus
extern "C" {
#endif

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    void ADC_IRQHandler()
    {
        HAL_ADC_IRQHandler(&handleADC);
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    /// This interrupt call soft NSS for spi (see Hardware::SPIforPanel.c::PanelInit() and HAL_GPIO_EXTI_Callback().
    void EXTI0_IRQHandler()
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    /// See Hardware::SPIforPanel.c::HAL_SPI_RxCpltCallback().
    void SPI1_IRQHandler()
    {
        HAL_SPI_IRQHandler(&handleSPI);
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    void OTG_HS_IRQHandler()
    {
        HAL_HCD_IRQHandler(&handleHCD);
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    void DMA1_Stream5_IRQHandler()
    {
        HAL_DMA_IRQHandler(handleDAC.DMA_Handle1);
    }

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------
// Эта функция вызывается при срабатывании любого аппаратного таймера
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    Timer::ElapsedCallback(htim);
}
