#pragma once
#include "Display/Colors.h"


class PageDisplay
{
public:
    static void OnPress_Accumulation_Clear();
    static void OnChanged_RefreshFPS(bool);
    static void OnChanged_Grid_Brightness();
    static ColorType colorTypeGrid;
};
