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
};

