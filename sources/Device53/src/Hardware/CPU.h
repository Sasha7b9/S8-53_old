#pragma once
#include "Hardware/stm32/207/stm207.h"
#include "globals.h"


class CPU : public STM207
{
public:

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    class RTC_
    {
    public:
        static void Init();

        static bool SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);

        static PackedTime GetPackedTime();
    };
};

