#pragma once 
#include "defines.h"
#include "Controls.h"


class Panel
{
public:
    static void Init();
    static void DeInit();
    static PanelButton PressedButton();
};
