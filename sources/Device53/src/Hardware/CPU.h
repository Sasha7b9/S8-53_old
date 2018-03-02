#pragma once
#include "Hardware/stm32/207/stm207.h"
#include "globals.h"


class CPU : public STM207
{
public:
    
    static void Init();

//----------------------------------------------------------------------------------------------------------------------------------------------------
#include "Hardware/CPU_VCP.h"

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class RTC_
    {
    friend class CPU;
        
    private:
        
        static void Init();

    public:

        static bool SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);

        static PackedTime GetPackedTime();
    };

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class ADC_
    {
    friend class CPU;
    
    private:
        static void Init();

    public:

        static uint16 value;
    };
};


#ifdef __cplusplus
extern "C" {
#endif

    /// Для рандомизатора
    void ADC_IRQHandler();
    /// Программный NSS для SPI (смотри Hardware::SPIforPanel.c::PanelInit() and HAL_GPIO_EXTI_Callback().
    void EXTI0_IRQHandler();
    /// See Hardware::SPIforPanel.c::HAL_SPI_RxCpltCallback().
    void SPI1_IRQHandler();

    void OTG_HS_IRQHandler();

    void DMA1_Stream5_IRQHandler();

#ifdef __cplusplus
}
#endif
