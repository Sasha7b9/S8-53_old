#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPU
{
public:

    static void Init();

    static void DeInit();

//----------------------------------------------------------------------------------------------------------------------------------------------------
#include "Hardware/CPU_VCP.h"
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

    void TIM3_IRQHandler();

#ifdef __cplusplus
}
#endif
