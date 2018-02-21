#pragma once
#include "defines.h"


class Display
{
public:
    static void Init();
    static void Update();
    static void Update1();
    static bool IsRun();
    static void AddStringToIndicating(pString string);
};
