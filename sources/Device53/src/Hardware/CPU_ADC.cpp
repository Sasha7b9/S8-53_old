#include "defines.h"
#include "Hardware/CPU.h"
#include "Hardware.h"
#include "globals.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16 CPU::ADC_::value = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPU::ADC_::Init()
{
    /*
    ј÷ѕ дл€ рандомизатора
    вход - ADC3 - 18 ADC3_IN4 - PF6
    тактова€ частота 25ћ√ц
    режим работы :
    -измерение по 1 регул€рному каналу
    - одиночное измерение по фронту внешнего запуска(прерывание от 112 - EXT11 - PC11)
    */

    __ADC3_CLK_ENABLE();

    static GPIO_InitTypeDef isGPIO =
    {
        GPIO_PIN_6,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL,
        0, 0
    };

    HAL_GPIO_Init(GPIOF, &isGPIO);

    GPIO_InitTypeDef isGPIOC =
    {
        GPIO_PIN_11,
        GPIO_MODE_IT_RISING,
        GPIO_NOPULL,
        0, 0
    };

    HAL_GPIO_Init(GPIOC, &isGPIOC);

    HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    handleADC.Instance = ADC3;
    handleADC.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    handleADC.Init.Resolution = ADC_RESOLUTION12b;
    handleADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    handleADC.Init.ScanConvMode = DISABLE;
    handleADC.Init.EOCSelection = ENABLE;
    handleADC.Init.ContinuousConvMode = DISABLE;
    handleADC.Init.DMAContinuousRequests = DISABLE;
    handleADC.Init.NbrOfConversion = 1;
    handleADC.Init.DiscontinuousConvMode = DISABLE;
    handleADC.Init.NbrOfDiscConversion = 0;
    handleADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    handleADC.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_Ext_IT11;

    if (HAL_ADC_Init(&handleADC) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&handleADC, &sConfig) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_ADC_Start_IT(&handleADC) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


#ifdef __cplusplus
extern "C" {
#endif

    //------------------------------------------------------------------------------------------------------------------------------------------------
    void ADC_IRQHandler()
    {
        ADC_HandleTypeDef *hadc = &handleADC;

        uint tmp1 = __HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC);
        uint tmp2 = __HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_EOC);
        /* Check End of conversion flag for regular channels */
        if (tmp1 && tmp2)
        {
            /* Update state machine on conversion status if not in error state */
            if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL))
            {
                /* Set ADC state */
                SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);
            }

            /* Determine whether any further conversion upcoming on group regular   */
            /* by external trigger, continuous mode or scan sequence on going.      */
            /* Note: On STM32F2, there is no independent flag of end of sequence.   */
            /*       The test of scan sequence on going is done either with scan    */
            /*       sequence disabled or with end of conversion flag set to        */
            /*       of end of sequence.                                            */
            if (ADC_IS_SOFTWARE_START_REGULAR(hadc) &&
                (hadc->Init.ContinuousConvMode == DISABLE) &&
                (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) ||
                 HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)))
            {
                /* Disable ADC end of single conversion interrupt on group regular */
                /* Note: Overrun interrupt was enabled with EOC interrupt in          */
                /* HAL_ADC_Start_IT(), but is not disabled here because can be used   */
                /* by overrun IRQ process below.                                      */
                __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);

                /* Set ADC state */
                CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

                if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
                {
                    SET_BIT(hadc->State, HAL_ADC_STATE_READY);
                }
            }

            /* Conversion complete callback */
            CPU::ADC_::value = (uint16)ADC3->DR;

            /* Clear regular group conversion flag */
            __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_STRT | ADC_FLAG_EOC);
        }

        tmp1 = __HAL_ADC_GET_FLAG(hadc, ADC_FLAG_OVR);
        tmp2 = __HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_OVR);
        /* Check Overrun flag */
        if (tmp1 && tmp2)
        {
            /* Note: On STM32F2, ADC overrun can be set through other parameters    */
            /*       refer to description of parameter "EOCSelection" for more      */
            /*       details.                                                       */

            /* Set ADC error code to overrun */
            SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_OVR);

            /* Clear ADC overrun flag */
            __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_OVR);

            /* Error callback */
            //HAL_ADC_ErrorCallback(hadc);

            /* Clear the Overrun flag */
            __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_OVR);
        }
    }

#ifdef __cplusplus
}
#endif
