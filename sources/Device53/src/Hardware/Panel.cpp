#include "defines.h"
#include "Panel.h"
#include "Hardware/CPU.h"
#include "Hardware/Controls.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/CommonFunctions.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"
#include <stdio.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_DATA            20

#define LED_CHANA_ENABLE    129
#define LED_CHANA_DISABLE   1
#define LED_CHANB_ENABLE    130
#define LED_CHANB_DISABLE   2
#define LED_TRIG_ENABLE     131
#define LED_TRIG_DISABLE    3
#define POWER_OFF           4

static const uint MIN_TIME = 500;

static PanelButton pressedKey = B_Empty;
static PanelButton pressedButton = B_Empty;         // Это используется для отслеживания нажатой кнопки при отключенной панели
static uint16 dataTransmitted[MAX_DATA] = {0x00};
static uint16 numDataForTransmitted = 0;
static uint timePrevPressButton = 0;
static uint timePrevReleaseButton = 0;

uint8 Panel::dataSPIfromPanel = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void (*Panel::funcOnKeyDown[B_NumButtons])()    =
{    
    0,
    EmptyFuncVV,    // B_ChannelA
    EmptyFuncVV,    // B_Service
    EmptyFuncVV,    // B_ChannelB
    EmptyFuncVV,    // B_Display
    EmptyFuncVV,    // B_Time
    EmptyFuncVV,    // B_Memory                                         
    EmptyFuncVV,    // B_Sinchro
    StartDown,      // B_Start
    EmptyFuncVV,    // B_Cursors
    EmptyFuncVV,    // B_Measures
    PowerDown,      // B_Power
    EmptyFuncVV,    // B_Help
    EmptyFuncVV,    // B_Menu
    EmptyFuncVV,    // B_F1
    EmptyFuncVV,    // B_F2
    EmptyFuncVV,    // B_F3
    EmptyFuncVV,    // B_F4
    EmptyFuncVV     // B_F5
};

static void (*funcOnKeyUp[B_NumButtons])()    =
{
    0,
    EmptyFuncVV,    // B_ChannelA
    EmptyFuncVV,    // B_Service
    EmptyFuncVV,    // B_ChannelB
    EmptyFuncVV,    // B_Display
    EmptyFuncVV,    // B_Time
    EmptyFuncVV,    // B_Memory
    EmptyFuncVV,    // B_Sinchro
    EmptyFuncVV,    // B_Start
    EmptyFuncVV,    // B_Cursors
    EmptyFuncVV,    // B_Measures
    EmptyFuncVV,    // B_Power
    EmptyFuncVV,    // B_Help
    EmptyFuncVV,    // B_Menu
    EmptyFuncVV,    // B_F1
    EmptyFuncVV,    // B_F2
    EmptyFuncVV,    // B_F3
    EmptyFuncVV,    // B_F4
    EmptyFuncVV     // B_F5
};

void (*Panel::funcOnLongPressure[B_NumButtons])() =
{
    0,
    ChannelALong,   // B_ChannelA
    EmptyFuncVV,    // B_Service
    ChannelBLong,   // B_ChannelB
    EmptyFuncVV,    // B_Display
    TimeLong,       // B_Time
    EmptyFuncVV,    // B_Memory
    TrigLong,       // B_Sinchro
    EmptyFuncVV,    // B_Start
    EmptyFuncVV,    // B_Cursors
    EmptyFuncVV,    // B_Measures
    EmptyFuncVV,    // B_Power
    Long_Help,      // B_Help
    MenuLong,       // B_Menu
    F1Long,         // B_F1
    F2Long,         // B_F2
    F3Long,         // B_F3
    F4Long,         // B_F4
    F5Long          // B_F5
};

void (*Panel::funculatorLeft[R_Set + 1])() =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RangeALeft,     // R_RangeA
    RShiftALeft,    // R_RShiftA
    RangeBLeft,     // R_RangeB
    RShiftBLeft,    // R_RShiftB
    TBaseLeft,      // R_TBase
    TShiftLeft,     // R_TShift
    TrigLevLeft,    // R_TrigLev
    SetLeft         // R_Set
};
void (*Panel::funculatorRight[R_Set + 1])() =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RangeARight,    // R_RangeA
    RShiftARight,   // R_RShiftA
    RangeBRight,    // R_RangeB
    RShiftBRight,   // R_RShiftB
    TBaseRight,     // R_TBase
    TShiftRight,    // R_TShift
    TrigLevRight,   // R_TrigLev
    SetRight        // R_Set
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Panel::Long_Help()
{
    gBF.showHelpHints++;
    gStringForHint = 0;
    gItemHint = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::ChannelALong()
{
    Menu::LongPressureButton(B_ChannelA);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::ChannelBLong()
{
    Menu::LongPressureButton(B_ChannelB);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TimeLong()
{
    Menu::LongPressureButton(B_Time);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TrigLong()
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::StartDown()
{
    if (MODE_WORK_IS_DIR)
    {
        Menu::PressButton(B_Start);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::PowerDown()
{
    Settings::Save();
    Panel::TransmitData(0x04);           // Посылаем команду выключения
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::MenuLong()
{
    Menu::LongPressureButton(B_Menu);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::F1Long()
{
    Menu::LongPressureButton(B_F1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::F2Long()
{
    Menu::LongPressureButton(B_F2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::F3Long()
{
    Menu::LongPressureButton(B_F3);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::F4Long()
{
    Menu::LongPressureButton(B_F4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::F5Long()
{
    Menu::LongPressureButton(B_F5);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ChangeRShift(int *prevTime, void(*f)(Channel, int16), Channel chan, int16 relStep)
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SetRShift(Channel ch, int16 rShift)
{
    FPGA::SetRShift(ch, rShift);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RShiftALeft()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, A, -STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RShiftARight()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, A, +STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RShiftBLeft()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, B, -STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RShiftBRight()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, B, +STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SetTrigLev(TrigSource ch, int16 trigLev)
{
    FPGA::SetTrigLev(ch, trigLev);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TrigLevLeft()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIGSOURCE, -STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TrigLevRight()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIGSOURCE, +STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ShiftScreen(int shift)
{
    Display::ShiftScreen(shift);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SetTShift(int tShift)
{
    FPGA::SetTShift(tShift);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TShiftLeft()
{
    XShift(-1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TShiftRight()
{
    XShift(1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RangeALeft()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeIncrease(A);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RangeARight()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeDecrease(A);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RangeBLeft()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeIncrease(B);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::RangeBRight()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeDecrease(B);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TBaseLeft()
{
    Sound::RegulatorSwitchRotate();
    FPGA::TBaseIncrease();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TBaseRight()
{
    Sound::RegulatorSwitchRotate();
    FPGA::TBaseDecrease();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::SetLeft()
{
    Menu::RotateRegSetLeft();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::SetRight()
{
    Menu::RotateRegSetRight();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelButton ButtonIsRelease(uint16 command)
{
    PanelButton button = B_Empty;
    if(command < B_NumButtons && command > B_Empty)
    {
        if(TIME_MS - timePrevReleaseButton > 100)
        {
            button = (PanelButton)command;
            timePrevReleaseButton = TIME_MS;
        }
    }
    return button;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelButton ButtonIsPress(uint16 command)
{
    PanelButton button = B_Empty;
    if (((command & 0x7f) < B_NumButtons) && ((command & 0x7f) > B_Empty))
    {
        if(TIME_MS - timePrevPressButton > 100)
        {
            button = (PanelButton)(command & 0x7f);
            timePrevPressButton = TIME_MS;
        }
    }
    return button;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelRegulator RegulatorLeft(uint16 command)
{
    if(command >= 20 && command <= 27)
    {
        return (PanelRegulator)command;
    }
    return R_Empty;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelRegulator RegulatorRight(uint16 command)
{
    if(((command & 0x7f) >= 20) && ((command & 0x7f) <= 27))
    {
        return (PanelRegulator)(command & 0x7f);
    }
    return R_Empty;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::OnTimerPressedKey()
{
    if(pressedKey != B_Empty)
    {
        void (*func)() = funcOnLongPressure[pressedKey];
        Menu::ReleaseButton(pressedKey);
        if(func != 0)
        {
            func();
        }
        pressedKey = B_Empty;
    }
    Timer::Disable(kPressKey);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Panel::ProcessingCommandFromPIC(uint16 command)
{
    static int allRecData = 0;

    PanelButton releaseButton = ButtonIsRelease(command);
    PanelButton pressButton = ButtonIsPress(command);
    PanelRegulator regLeft = RegulatorLeft(command);
    PanelRegulator regRight = RegulatorRight(command);

    if (pressButton)
    {
        pressedButton = pressButton;
    }

    if(gBF.panelIsRunning == 0)
    {
        return true;
    }

    if(command != 0)
    {
        allRecData++;
    }
    else
    {
        allRecData++;
        allRecData--;
    }

    if(releaseButton)
    {
        Menu::ReleaseButton(releaseButton);
        funcOnKeyUp[releaseButton]();
        if(pressedKey != B_Empty)
        {
            Menu::ShortPressureButton(releaseButton);
            pressedKey = B_Empty;
        }
    }
    else if(pressButton)
    {
        funcOnKeyDown[pressButton]();
        Menu::PressButton(pressButton);
        pressedKey = pressButton;
        Timer::SetAndEnable(kPressKey, OnTimerPressedKey, 500);
    }
    else if(regLeft)
    {
        /*
        if (set.memory.modeWork == ModeWork_Dir || regLeft == R_Set || regLeft == R_TShift)
        {
        */
            funculatorLeft[regLeft]();
        //}
    }
    else if(regRight)
    {
        /*
        if (set.memory.modeWork == ModeWork_Dir || regRight == R_Set || regRight == R_TShift)
        {
        */
            funculatorRight[regRight]();
        //}
    }

    if ((command > R_Set && command < (B_Empty + 1 + 128)) || (command > (R_Set + 128)))
    {
        if(true)
        {
            static int errRecData = 0;
            errRecData++;
            float percent = (float)errRecData / allRecData * 100.0f;
            char buffer[100];
            buffer[0] = 0;
            sprintf(buffer, "%5.3f", (double)percent);
            strcat(buffer, "%");
            LOG_ERROR("Ошибок SPI - %s %d/%d, command = %d", buffer, errRecData, allRecData, (int)command);
        }
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::EnableLEDChannelA(bool enable)
{
    TransmitData(enable ? LED_CHANA_ENABLE : LED_CHANA_DISABLE);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::EnableLEDChannelB(bool enable)
{
    TransmitData(enable ? LED_CHANB_ENABLE : LED_CHANB_DISABLE);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::EnableLEDTrig(bool enable)
{
    static uint timeEnable = 0;
    static bool first = true;
    static bool fired = false;
    if(first)
    {
        TransmitData(LED_TRIG_DISABLE);
        Display::EnableTrigLabel(false);
        timeEnable = TIME_MS;
        first = false;
    }

    if(enable)
    {
        timeEnable = TIME_MS;
    }

    if(enable != fired)
    {
        if(enable)
        {
            TransmitData(LED_TRIG_ENABLE);
            Display::EnableTrigLabel(true);
            fired = true;
        }
        else if(TIME_MS - timeEnable > 100)
        {
            TransmitData(LED_TRIG_DISABLE);
            Display::EnableTrigLabel(false);
            fired = false;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::TransmitData(uint16 data)
{
    if(numDataForTransmitted >= MAX_DATA)
    {
        LOG_WRITE("Не могу послать в панель - мало места");
    }
    else
    {
        dataTransmitted[numDataForTransmitted] = data;
        numDataForTransmitted++;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint16 Panel::NextData()
{
    if (numDataForTransmitted > 0)
    {
        numDataForTransmitted--;
        return dataTransmitted[numDataForTransmitted];
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::Disable()
{
    gBF.panelIsRunning = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::Enable()
{
    gBF.panelIsRunning = 1;
}

/*
    SPI1
    56  - PG0 - программный NSS 
    41  - PA5 - SCK
    42  - PA6 - MISO
    135 - PB5 - MOSI
*/

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::Init()
{
    __SPI1_CLK_ENABLE();

    GPIO_InitTypeDef isGPIOA_B =
    {
        GPIO_PIN_5 | GPIO_PIN_6,    // GPIO_Pin
        GPIO_MODE_AF_PP,            // GPIO_Mode
        GPIO_PULLDOWN,
        GPIO_SPEED_FAST,            // GPIO_Speed
        GPIO_AF5_SPI1
    };
    HAL_GPIO_Init(GPIOA, &isGPIOA_B);
    
    isGPIOA_B.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &isGPIOA_B);
    
    if (HAL_SPI_Init(&handleSPI) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    HAL_NVIC_SetPriority(SPI1_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
    
    // Теперь настроим программный NSS (PG0).
    
    GPIO_InitTypeDef isGPIOG =
    {
        GPIO_PIN_0,             // Pin
        GPIO_MODE_IT_RISING,    // Mode
        GPIO_NOPULL,            // Pull
        GPIO_SPEED_FREQ_LOW,    // Speed
        0                       // Alternate
    };
   
    HAL_GPIO_Init(GPIOG, &isGPIOG);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    // Лампочка УСТАНОВКА
    isGPIOG.Pin = GPIO_PIN_12;
    isGPIOG.Mode = GPIO_MODE_OUTPUT_PP;
    isGPIOG.Speed = GPIO_SPEED_HIGH;
    isGPIOG.Alternate = GPIO_AF0_MCO;
    HAL_GPIO_Init(GPIOG, &isGPIOG);

    EnableLEDRegSet(false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel::EnableLEDRegSet(bool enable)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, enable ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
PanelButton Panel::WaitPressingButton()
{
    pressedButton = B_Empty;
    while (pressedButton == B_Empty) {};
    return pressedButton;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0)
    {
        HAL_SPI_Receive_IT(&handleSPI, &Panel::dataSPIfromPanel, 1);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* handle)
{
    if (!Panel::ProcessingCommandFromPIC(Panel::dataSPIfromPanel))
    {
        HAL_SPI_DeInit(handle);
        HAL_SPI_Init(handle);
    }
    uint16 data = Panel::NextData();
    SPI1->DR = data;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Panel::IsFunctionalButton(PanelButton button)
{
    return button >= B_F1 && button <= B_F5;
}
