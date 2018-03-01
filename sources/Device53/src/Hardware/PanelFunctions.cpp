#include "Panel.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Definition.h"
#include "Menu/Pages/PageMemory.h"
#include "Utils/Math.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"


static const uint MIN_TIME = 500;


static void ChangeRShift(int *prevTime, void(*f)(Channel, int16), Channel chan, int16 relStep);


void Panel::Long_Help()
{
    gBF.showHelpHints++;
    gStringForHint = 0;
    gItemHint = 0;
}

static void ChannelALong()
{
    Menu::LongPressureButton(B_ChannelA);
}

static void ChannelBLong()
{
    Menu::LongPressureButton(B_ChannelB);
}

static void TimeLong()
{
    Menu::LongPressureButton(B_Time);
}

static void TrigLong()
{
    if (MODE_LONG_PRESS_TRIG_IS_LEVEL0)
    {
        Menu::LongPressureButton(B_Trig);
    }
    else
    {
        FPGA::FindAndSetTrigLevel();
    }
}

static void StartDown()                        // B_Start
{
    if (MODE_WORK_IS_DIR)
    {
        Menu::PressButton(B_Start);
    }
}

static void PowerDown()                        // B_Power
{
    Settings::Save();
    Panel::TransmitData(0x04);           // Посылаем команду выключения
}

static void MenuLong() 
{
    Menu::LongPressureButton(B_Menu);
}

static void F1Long()
{
    Menu::LongPressureButton(B_F1);
}

static void F2Long()
{
    Menu::LongPressureButton(B_F2);
}

static void F3Long()
{
    Menu::LongPressureButton(B_F3);
}

static void F4Long()
{
    Menu::LongPressureButton(B_F4);
}

static void F5Long()
{
    Menu::LongPressureButton(B_F5);
}

//****************************************************************************************************************
static int CalculateCount(int *prevTime)
{
    uint time = TIME_MS;
    uint delta = time - (uint)(*prevTime);
    *prevTime = (int)time;

    if (delta > 75)
    {
        return 1;
    }
    else if (delta > 50)
    {
        return 2;
    }
    else if (delta > 25)
    {
        return 3;
    }
    return 4;
}

static bool CanChangeTShift(int16 tShift)
{
    static uint time = 0;
    if (tShift == 0)
    {
        time = TIME_MS;
        return true;
    }
    else if (time == 0)
    {
        return true;
    }
    else if (TIME_MS - time > MIN_TIME)
    {
        time = 0;
        return true;
    }
    return false;
}

static bool CanChangeRShiftOrTrigLev(TrigSource channel, int16 rShift)
{
    static uint time[3] = {0, 0, 0};
    if (rShift == RShiftZero)
    {
        time[channel] = TIME_MS;
        return true;
    }
    else if (time[channel] == 0)
    {
        return true;
    }
    else if (TIME_MS - time[channel] > MIN_TIME)
    {
        time[channel] = 0;
        return true;
    }
    return false;
}

void ChangeRShift(int *prevTime, void(*f)(Channel, int16), Channel chan, int16 relStep)
{
    if (ENUM_ACCUM_IS_NONE)
    {
        FPGA::TemporaryPause();
    }
    int count = CalculateCount(prevTime);
    int rShiftOld = SET_RSHIFT(chan);
    int rShift = SET_RSHIFT(chan) + relStep * count;
    if ((rShiftOld > RShiftZero && rShift < RShiftZero) || (rShiftOld < RShiftZero && rShift > RShiftZero))
    {
        rShift = RShiftZero;
    }
    if (CanChangeRShiftOrTrigLev((TrigSource)chan, (int16)rShift))
    {
        Sound::RegulatorShiftRotate();
        f(chan, (int16)rShift);
    }
}

static void ChangeTrigLev(int *prevTime, void(*f)(TrigSource, int16), TrigSource trigSource, int16 relStep)
{
    int count = CalculateCount(prevTime);
    int trigLevOld = SET_TRIGLEV(trigSource);
    int trigLev = SET_TRIGLEV(trigSource) + relStep * count;
    if ((trigLevOld > TrigLevZero && trigLev < TrigLevZero) || (trigLevOld < TrigLevZero && trigLev > TrigLevZero))
    {
        trigLev = TrigLevZero;
    }
    if (CanChangeRShiftOrTrigLev(trigSource, (int16)trigLev))
    {
        Sound::RegulatorShiftRotate();
        f(trigSource, (int16)trigLev);
    }
}

static void ChangeTShift(int *prevTime, void(*f)(int), int16 relStep)
{
    int count = CalculateCount(prevTime);
    int tShiftOld = SET_TSHIFT;
    float step = (float)(relStep * count);
    if (step < 0)
    {
        if (step > -1)
        {
            step = -1;
        }
    }
    else
    {
        if (step < 1)
        {
            step = 1;
        }
    }

    int16 tShift = (int16)(SET_TSHIFT + step);
    if (((tShiftOld > 0) && (tShift < 0)) || (tShiftOld < 0 && tShift > 0))
    {
        tShift = 0;
    }
    if (CanChangeTShift(tShift))
    {
        Sound::RegulatorShiftRotate();
        f(tShift);
    }
}

static void ChangeShiftScreen(int *prevTime, void(*f)(int), int16 relStep)
{
    int count = CalculateCount(prevTime);
    float step = (float)(relStep * count);
    if (step < 0)
    {
        if (step > -1)
        {
            step = -1;
        }
    }
    else if (step < 1)
    {
        step = 1;
    }
    f((int)step);
}

static void SetRShift(Channel ch, int16 rShift)
{
    FPGA::SetRShift(ch, rShift);
}

static void RShiftALeft()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, A, -STEP_RSHIFT);
}

static void RShiftARight()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, A, +STEP_RSHIFT);
}

static void RShiftBLeft()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, B, -STEP_RSHIFT);
}

static void RShiftBRight()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, B, +STEP_RSHIFT);
}

static void SetTrigLev(TrigSource ch, int16 trigLev)
{
    FPGA::SetTrigLev(ch, trigLev);
}

static void TrigLevLeft()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIGSOURCE, -STEP_RSHIFT);
}

static void TrigLevRight()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIGSOURCE, +STEP_RSHIFT);
}

static void ShiftScreen(int shift)
{
    Display::ShiftScreen(shift);
}

static void SetTShift(int tShift)
{
    FPGA::SetTShift(tShift);
}

static void XShift(int delta)
{
    static int prevTime = 0;
    if (!FPGA::IsRunning() || TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY)
    {
        if (!ENUM_POINTS_IS_281)
        {
            ChangeShiftScreen(&prevTime, ShiftScreen, (int16)(2 * delta));
        }
    }
    else
    {
        ChangeTShift(&prevTime, SetTShift, (int16)delta);
    }
}

static void TShiftLeft()
{
    XShift(-1);
}

static void TShiftRight()
{
    XShift(1);
}

static void RangeALeft()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeIncrease(A);
}

static void RangeARight()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeDecrease(A);
}

static void RangeBLeft()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeIncrease(B);
}

static void RangeBRight()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeDecrease(B);
}

static void TBaseLeft()
{
    Sound::RegulatorSwitchRotate();
    FPGA::TBaseIncrease();
}

static void TBaseRight()
{
    Sound::RegulatorSwitchRotate();
    FPGA::TBaseDecrease();
}

static void SetLeft()
{
    Menu::RotateRegSetLeft();
}

static void SetRight()
{
    Menu::RotateRegSetRight();
}
