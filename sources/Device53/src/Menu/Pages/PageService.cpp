#include "Menu/Pages/Definition.h"
#include "Display/Symbols.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGAMath.h"
#include "Menu/Menu.h"
#include "Panel/Panel.h"
#include "Hardware/FLASH.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Utils/CommonFunctions.h"
#include "Utils/Dictionary.h"

/** @addtogroup Menu
 *  @{
 *  @addtogroup PageService
 *  @{
 */

extern const PageBase mainPage;
extern const PageBase pService;
extern const PageBase ppCalibrator;
extern const PageBase pppMath_Function;
extern const PageBase ppMath;
extern const PageBase pppMath_FFT;
extern const PageBase ppppMath_FFT_Cursors;
extern const PageBase ppEthernet;
extern const PageBase ppInformation;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncDraw()
{
    Painter::BeginScene(Color::BACK);

    Painter::DrawTextInRectWithTransfersC(30, 110, 300, 200, "Подтвердите сброс настроек нажатием кнопки ПУСК/СТОП.\n"
                                         "Нажмите любую другую кнопку, если сброс не нужен.", Color::FILL);

    Painter::EndScene();
}

static void OnTimerDraw()
{
    Display::Update();
}

static void OnPress_ResetSettings()
{
    Panel::Disable();
    Display::SetDrawMode(DrawMode_Hand, FuncDraw);
    Timer::SetAndEnable(kTimerDisplay, OnTimerDraw, 100);

    if (Panel::WaitPressingButton() == B_Start)
    {
        Settings_Load(true);
    }

    Timer::Disable(kTimerDisplay);
    Display::SetDrawMode(DrawMode_Auto, 0);
    Panel::Enable();
}

DEF_BUTTON(         bResetSettings,                                                                                  //--- СЕРВИС - Сброс настроек ---
    "Сброс настроек", "Reset settings",
    "Сброс настроек на настройки по умолчанию",
    "Reset to default settings",
    pService, FuncActive, OnPress_ResetSettings, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_AutoSearch()
{
    FPGA::StartAutoFind();
};

DEF_BUTTON(         bAutoSearch,                                                                                      //--- СЕРВИС - Поиск сигнала ---
    "Поиск сигнала", "Find signal",
    "Устанавливает оптимальные установки осциллографа для сигнала в канале 1",
    "Sets optimal settings for the oscilloscope signal on channel 1",
    pService, FuncActive, OnPress_AutoSearch, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Calibrator_Mode(bool active)
{
    FPGA::SetCalibratorMode(CALIBRATOR);
}

DEF_CHOICE_3(       cCalibrator_Mode,                                                                       //--- СЕРВИС - КАЛИБРАТОР - Калибратор ---
    "Калибратор", "Calibrator",
    "Режим работы калибратора",
    "Mode of operation of the calibrator",
    "Перем", "DC",
    "Пост",  "AC",
    "0В",    "OV",
    CALIBRATOR, ppCalibrator, FuncActive, OnChanged_Calibrator_Mode, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Calibrator_Calibrate()
{
    gStateFPGA.needCalibration = true;
}

DEF_BUTTON(         cCalibrator_Calibrate,                                                                 //--- СЕРВИС - КАЛИБРАТОР - Калибровать ---
    "Калибровать", "Calibrate",
    "Запуск процедуры калибровки",
    "Running the calibration procedure",
    ppCalibrator, FuncActive, OnPress_Calibrator_Calibrate, FuncDraw
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_2(         ppCalibrator,                                                                                           // СЕРВИС - КАЛИБРАТОР ///
    "КАЛИБРАТОР", "CALIBRATOR",
    "Управлением калибратором и калибровка осциллографа",
    "Control of the calibrator and calibration of an oscillograph",
    cCalibrator_Mode,       // СЕРВИС - КАЛИБРАТОР - Калибратор
    cCalibrator_Calibrate,  // СЕРВИС - КАЛИБРАТОР - Калибровать
    Page_Service_Calibrator, &pService, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON_EXIT(  sbMath_Function_Exit,                                                          //--- СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Выход ---
    pppMath_Function, FuncActive, FuncPress, DrawSB_Exit
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Math_Function_ModeDraw_Disable(int x, int y)
{
    Painter::DrawText(x + 2 + (set.common.lang == English ? 2 : 0), y + 5, set.common.lang == Russian ? "Вык" : "Dis");
}

static void Draw_Math_Function_ModeDraw_Separate(int x, int y)
{
    Painter::DrawRectangle(x + 3, y + 5, 13, 9);
    Painter::DrawHLine(y + 9, x + 3, x + 16);
    Painter::DrawHLine(y + 10, x + 3, x + 16);
}

static void Draw_Math_Function_ModeDraw_Together(int x, int y)
{
    Painter::DrawRectangle(x + 3, y + 5, 13, 9);
}

static void OnPress_Math_Function_ModeDraw()
{
    if (FFT_ENABLED)
    {
        Display::ShowWarningBad(ImpossibleEnableMathFunction);
    }
    else
    {
        CircleIncrease<int8>((int8 *)&FUNC_MODE_DRAW, 0, 2);
    }
}

static void Draw_Math_Function_ModeDraw(int x, int y)
{
    static const pFuncVII funcs[3] =
    {
        Draw_Math_Function_ModeDraw_Disable,
        Draw_Math_Function_ModeDraw_Separate,
        Draw_Math_Function_ModeDraw_Together
    };
    funcs[FUNC_MODE_DRAW](x, y);
}

DEF_SMALL_BUTTON_HINTS_3(   sbMath_Function_ModeDraw,                                                  //--- СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Экран ---
    "Экран", "Display",
    "Выбирает режим отображения математического сигнала",
    "Chooses the mode of display of a mathematical signal",
    pppMath_Function, FuncActive, OnPress_Math_Function_ModeDraw, Draw_Math_Function_ModeDraw,
    Draw_Math_Function_ModeDraw_Disable,   "Вывод математической функции отключён",
                                           "The conclusion of mathematical function is disconnected",
    Draw_Math_Function_ModeDraw_Separate,  "Сигналы и математическая функция выводятся в разных окнах",
                                           "Signals and mathematical function are removed in different windows",
    Draw_Math_Function_ModeDraw_Together,  "Сигналы и математическая функция выводятся в одном окне",
                                           "Signals and mathematical function are removed in one window"
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Math_Function_Type_Sum(int x, int y)
{
    Painter::DrawHLine(y + 9, x + 4, x + 14);
    Painter::DrawVLine(x + 9, y + 4, y + 14);
}

static void Draw_Math_Function_Type_Mul(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 4, y + 3, SYMBOL_MATH_FUNC_MUL);
    Painter::SetFont(TypeFont_8);
}

static void OnPress_Math_Function_Type()
{
    CircleIncrease<int8>((int8 *)&MATH_FUNC, 0, 1);
}

static void Draw_Math_Function_Type(int x, int y)
{
    const pFuncVII funcs[2] = {Draw_Math_Function_Type_Sum, Draw_Math_Function_Type_Mul};
    funcs[MATH_FUNC](x, y);
}

DEF_SMALL_BUTTON_HINTS_2(   sbMath_Function_Type,                                                        //--- СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Вид ---
    "Вид", "Type",
    "Выбор математической функции",
    "Choice of mathematical function",
    pppMath_Function, FuncActive, OnPress_Math_Function_Type, Draw_Math_Function_Type,
    Draw_Math_Function_Type_Sum, "Сложение",  "Addition",
    Draw_Math_Function_Type_Mul, "Умножение", "Multiplication"
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Math_Function_ModeRegSet_Range(int x, int y)
{
    Painter::DrawChar(x + 7, y + 5, set.common.lang == Russian ? 'M' : 'S');
}

static void Draw_Math_Function_ModeRegSet_RShift(int x, int y)
{
    Painter::DrawText(x + 5 - (set.common.lang == English ? 3 : 0), y + 5, set.common.lang == Russian ? "См" : "Shif");
}

static void Draw_Math_Function_ModeRegSet(int x, int y)
{
    const pFuncVII funcs[2] = {Draw_Math_Function_ModeRegSet_Range, Draw_Math_Function_ModeRegSet_RShift};
    funcs[MATH_MODE_REG_SET](x, y);
}

static void OnPress_Math_Function_ModeRegSet()
{
    CircleIncrease<int8>((int8 *)&MATH_MODE_REG_SET, 0, 1);
}

DEF_SMALL_BUTTON_HINTS_2(   sbMath_Function_ModeRegSet,                                //--- СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Режим ручки УСТАНОВКА ---
    "Режим ручки УСТАНОВКА", "Mode regulator SET",
    "Выбор режима ручки УСТАНОВКА - управление масштабом или смещением",
    "Choice mode regulcator УСТАНОВКА - management of scale or shift",
    pppMath_Function, FuncActive, OnPress_Math_Function_ModeRegSet, Draw_Math_Function_ModeRegSet,
    Draw_Math_Function_ModeRegSet_Range,  "Управление масштабом", "Management of scale",
    Draw_Math_Function_ModeRegSet_RShift, "Управление смещением", "Management of shift"
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Math_Function_RangeA()
{
    SET_RANGE_MATH = SET_RANGE_A;
    MATH_MULTIPLIER = SET_DIVIDER_A;
}

static void Draw_Math_Function_RangeA(int x, int y)
{
    Painter::DrawChar(x + 8, y + 5, '1');
}

DEF_SMALL_BUTTON(   sbMath_Function_RangeA,                                              //--- СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 1-го канала ---
    "Масштаб 1-го канала", "Scale of the 1st channel",
    "Берёт масштаб для математического сигнала из первого канала",
    "Takes scale for a mathematical signal from the first channel",
    pppMath_Function, FuncActive, OnPress_Math_Function_RangeA, Draw_Math_Function_RangeA
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Math_Function_RangeB()
{
    SET_RANGE_MATH = SET_RANGE_B;
    MATH_MULTIPLIER = SET_DIVIDER_B;
}

static void Draw_Math_Function_RangeB(int x, int y)
{
    Painter::DrawChar(x + 8, y + 5, '2');
}

DEF_SMALL_BUTTON(   sbMath_Function_RangeB,                                              //--- СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 2-го канала ---
    "Масштаб 2-го канала", "Scale of the 2nd channel",
    "Берёт масштаб для математического сигнала из второго канала",
    "Takes scale for a mathematical signal from the second channel",
    pppMath_Function, FuncActive, OnPress_Math_Function_RangeB, Draw_Math_Function_RangeB
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsActive_Math_Function()
{
    return !FFT_ENABLED;
}

static void OnPress_Math_Function()
{
    if (FFT_ENABLED)
    {
        Display::ShowWarningBad(ImpossibleEnableMathFunction);
    }
}

static void OnRegSet_Math_Function(int delta)
{
    if (FUNC_MODE_DRAW_IS_DISABLED)
    {
        return;
    }

    if (MATH_MODE_REG_SET_IS_RSHIFT)
    {
        int16 prevRShift = SET_RSHIFT_MATH;
        int16 rShift = prevRShift;
        if (delta > 0)
        {
            if (rShift < RShiftMax)
            {
                rShift += 4 * STEP_RSHIFT;
                LIMIT_ABOVE(rShift, RShiftMax);
                if (prevRShift < RShiftZero && rShift > RShiftZero)
                {
                    rShift = RShiftZero;
                }
                Sound::RegulatorShiftRotate();
                SET_RSHIFT_MATH = rShift;
            }
        }
        else if (delta < 0)
        {
            if (rShift > RShiftMin)
            {
                rShift -= 4 * STEP_RSHIFT;
                LIMIT_BELOW(rShift, RShiftMin);
                if (prevRShift > RShiftZero && rShift < RShiftZero)
                {
                    rShift = RShiftZero;
                }
                Sound::RegulatorShiftRotate();
                SET_RSHIFT_MATH = rShift;
            }
        }
    }
    if (MATH_MODE_REG_SET_IS_RANGE)
    {
        static int sum = 0;
        sum -= delta;

        float rShiftAbs = RSHIFT_2_ABS(SET_RSHIFT_MATH, SET_RANGE_MATH);

        if (sum > 2)
        {
            if (SET_RANGE_MATH < RangeSize - 1)
            {
                ++SET_RANGE_MATH;
                SET_RSHIFT_MATH = RSHIFT_2_REL(rShiftAbs, SET_RANGE_MATH);
                Sound::RegulatorSwitchRotate();
            }
            sum = 0;
        }
        else if (sum < -2)
        {
            if (SET_RANGE_MATH > 0)
            {
                --SET_RANGE_MATH;
                SET_RSHIFT_MATH = RSHIFT_2_REL(rShiftAbs, SET_RANGE_MATH);
                Sound::RegulatorSwitchRotate();
            }
            sum = 0;
        }
    }
}

DEF_PAGE_SB(        pppMath_Function,                                                                             // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ ///
    "ФУНКЦИЯ", "FUNCTION",
    "Установка и выбор математической функции - сложения или умножения",
    "Installation and selection of mathematical functions - addition or multiplication",
    &sbMath_Function_Exit,       // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Выход
    &sbMath_Function_ModeDraw,   // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Экран
    &sbMath_Function_Type,       // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Вид
    &sbMath_Function_ModeRegSet, // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Режим ручки УСТАНОВКА
    &sbMath_Function_RangeA,     // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 1-го канала
    &sbMath_Function_RangeB,     // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 2-го канала
    PageSB_Service_Function, &ppMath, IsActive_Math_Function, OnPress_Math_Function, EmptyFuncVV, OnRegSet_Math_Function
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       cMath_FFT_Enable,                                                             //--- СЕРВИС - МАТЕМАТИКА - СПЕКТР - Отображение ---
    "Отображение", "Display",
    "Включает и выключает отображение спектра",
    "Enables or disables the display of the spectrum",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    FFT_ENABLED, pppMath_FFT, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       cMath_FFT_Scale,                                                                    //--- СЕРВИС - МАТЕМАТИКА - СПЕКТР - Шкала ---
    "Шкала", "Scale",
    "Задаёт масштаб вывода спектра - линейный или логарифмический",
    "Sets the scale of the output spectrum - linear or logarithmic",
    "Логарифм", "Log",
    "Линейная", "Linear",
    SCALE_FFT, pppMath_FFT, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_3(       cMath_FFT_Source,                                                                //--- СЕРВИС - МАТЕМАТИКА - СПЕКТР - Источник ---
    "Источник", "Source",
    "Выбор источника для расчёта спектра",
    "Selecting the source for the calculation of the spectrum",
    "Канал 1",     "Channel 1",
    "Канал 2",     "Channel 2",
    "Канал 1 + 2", "Channel 1 + 2",
    SOURCE_FFT, pppMath_FFT, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_4(       cMath_FFT_Window,                                                                    //--- СЕРВИС - МАТЕМАТИКА - СПЕКТР - Окно ---
    "Окно", "Window",
    "Задаёт окно для расчёта спектра",
    "Sets the window to calculate the spectrum",
    "Прямоугольн", "Rectangle",
    "Хэмминга",    "Hamming",
    "Блэкмена",    "Blackman",
    "Ханна",       "Hann",
    WINDOW_FFT, pppMath_FFT, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Math_FFT_Cursors_Exit()
{
    Display::RemoveAddDrawFunction();
}

DEF_SMALL_BUTTON_EXIT(  cMath_FFT_Cursors_Exit,                                               //--- СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Выход ---
    ppppMath_FFT_Cursors, 
    FuncActive, OnPress_Math_FFT_Cursors_Exit, DrawSB_Exit
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Math_FFT_Cursors_Source()
{
    FFT_CUR_CURSOR = (FFT_CUR_CURSOR + 1) % 2;
}

static void Draw_Math_FFT_Cursors_Source(int x, int y)
{
    Painter::DrawText(x + 7, y + 5, FFT_CUR_CURSOR_IS_0 ? "1" : "2");
}

DEF_SMALL_BUTTON(   cMath_FFT_Cursors_Source,                                              //--- СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Источник ---
    "Источник", "Source",
    "Выбор источника для расчёта спектра",
    "Source choice for calculation of a range",
    ppppMath_FFT_Cursors, FuncActive, OnPress_Math_FFT_Cursors_Source, Draw_Math_FFT_Cursors_Source
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsActive_Math_FFT_Cursors()
{
    return FFT_ENABLED;
}

static void OnRegSet_Math_FFT_Cursors(int angle)
{
    FFT_POS_CURSOR(FFT_CUR_CURSOR) += angle;
    Sound::RegulatorShiftRotate();
}

DEF_PAGE_SB(        ppppMath_FFT_Cursors,                                                                // СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ ///
    "КУРСОРЫ", "CURSORS",
    "Включает курсоры для измерения параметров спектра",
    "Includes cursors to measure the parameters of the spectrum",
    &cMath_FFT_Cursors_Exit,   // СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Выход
    &cMath_FFT_Cursors_Source, // СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Источник
    0,
    0,
    0,
    0,
    PageSB_Service_FFT_Cursors, &pppMath_FFT, IsActive_Math_FFT_Cursors, EmptyPressPage, FuncDrawPage, OnRegSet_Math_FFT_Cursors
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Math_FFT_Limit()
{
    return SCALE_FFT_IS_LOG;
}

DEF_CHOICE_3(       cMath_FFT_Limit,                                                                 //--- СЕРВИС - МАТЕМАТИКА - СПЕКТР - Диапазон ---
    "Диапазон", "Range",
    "Здесь можно задать предел наблюдения за мощностью спектра",
    "Here you can set the limit of monitoring the power spectrum",
    "-40дБ", "-40dB",
    "-60дБ", "-60dB",
    "-80дБ", "-80dB",
    MAX_DB_FFT, pppMath_FFT, IsActive_Math_FFT_Limit, FuncChangedChoice, FuncDraw
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsActive_Math_FFT()
{
    return FUNC_MODE_DRAW_IS_DISABLED;
}

static void OnPress_Math_FFT()
{
    if (!IsActive_Math_FFT())
    {
        Display::ShowWarningBad(ImpossibleEnableFFT);
    }
}

DEF_PAGE_6(         pppMath_FFT,                                                                                   // СЕРВИС - МАТЕМАТИКА - СПЕКТР ///
    "СПЕКТР", "SPECTRUM",
    "Отображение спектра входного сигнала",
    "Mapping the input signal spectrum",
    cMath_FFT_Enable,       // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Отображение
    cMath_FFT_Scale,        // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Шкала
    cMath_FFT_Source,       // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Источник
    cMath_FFT_Window,       // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Окно
    ppppMath_FFT_Cursors,   // СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ
    cMath_FFT_Limit,        // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Диапазон
    Page_MathFFT, &ppMath, IsActive_Math_FFT, OnPress_Math_FFT
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_2(         ppMath,                                                                                                 // СЕРВИС - МАТЕМАТИКА ///
    "МАТЕМАТИКА", "MATH",
    "Математические функции и БПФ",
    "Mathematical functions and FFT",
    pppMath_Function,   // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ
    pppMath_FFT,        // СЕРВИС - МАТЕМАТИКА - СПЕКТР
    Page_Math, &pService, FuncActive, FuncPress
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Ethernet_Enable(bool active)
{
    Display::ShowWarningGood(NeedRebootDevice2);
    Display::ShowWarningGood(NeedRebootDevice1);
};

DEF_CHOICE_2(       cEthernet_Enable,                                                                           //--- СЕРВИС - ETHERNET - Ethernet ---
    "Ethernet", "Ethernet"
    ,
    "Чтобы задействовать ethernet, выберите \"Включено\" и выключите прибор.\n"
    "Чтобы отключить ethernet, выберите \"Отключено\" и выключите прибор."
    ,
    "To involve ethernet, choose \"Included\" and switch off the device.\n"
    "To disconnect ethernet, choose \"Disconnected\" and switch off the device.",
    "Включено",  "Included",
    "Отключено", "Disconnected",
    ETH_ENABLE, ppEthernet, FuncActive, OnChanged_Ethernet_Enable, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_IP_ADDRESS(     ipEthernet_IP,                                                                              //--- СЕРВИС - ETHERNET - IP адрес ---
    "IP адрес", "IP-address",
    "Установка IP адреса",
    "Set of IP-address",
    IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3, &PORT,
    ppEthernet, FuncActive, OnChanged_Ethernet_Enable
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_IP_ADDRESS(     ipEthernet_Mask,                                                                       //--- СЕРВИС - ETHERNET - Маска подсети ---
    "Маска подсети", "Network mask",
    "Установка маски подсети",
    "Set of network mask",
    NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3, 0,
    ppEthernet, FuncActive, OnChanged_Ethernet_Enable
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_IP_ADDRESS(     ipEthernet_Gateway,                                                                             //--- СЕРВИС - ETHERNET - Шлюз ---
    "Шлюз", "Gateway",
    "Установка адреса основного шлюза",
    "Set of gateway address",
    GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3, 0,
    ppEthernet, FuncActive, OnChanged_Ethernet_Enable
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_MAC_ADDRESS(    macEthernet_MAC,                                                                           //--- СЕРВИС - ETHERNET - Физ адрес ---
    "Физ адрес", "MAC-address",
    "Установка физического адреса",
    "Set of MAC-address",
    MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5,
    ppEthernet, FuncActive, OnChanged_Ethernet_Enable
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_5(         ppEthernet,                                                                                               // СЕРВИС - ETHERNET ///
    "ETHERNET", "ETHERNET",
    "Настройки ethernet",
    "Settings of ethernet",
    cEthernet_Enable,       // СЕРВИС - ETHERNET - Ethernet
    ipEthernet_IP,          // СЕРВИС - ETHERNET - IP адрес
    ipEthernet_Mask,        // СЕРВИС - ETHERNET - Маска подсети
    ipEthernet_Gateway,     // СЕРВИС - ETHERNET - Шлюз
    macEthernet_MAC,        // СЕРВИС - ETHERNET - Физ адрес
    Page_Service_Ethernet, &pService, FuncActive, FuncPress
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       cSound,                                                                                                    //--- СЕРВИС - Звук ---
    "Звук", "Sound",
    "Включение/выключение звука",
    "Inclusion/switching off of a sound",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    SOUND_ENABLED, pService, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       cLang,                                                                                                     //--- СЕРВИС - Язык ---
    "Язык", "Language",
    "Позволяет выбрать язык меню",
    "Allows you to select the menu language",
    "Русский",    "Russian",
    "Английский", "English",
    LANG, pService, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;
DEF_TIME(           tTime,                                                                                                    //--- СЕРВИС - Время ---
    "Время", "Time"
    ,
    "Установка текущего времени.\nПорядок работы:\n"
    "Нажать на элемент меню \"Время\". Откроется меню установки текущего времени. Короткими нажатиями кнопки на цифровой клавиатуре, соответсвующей "
    "элементу управления \"Время\", выделить часы, минуты, секунды, год, месяц, или число. Выделенный элемент обозначается мигающей областью. "
    "Вращением ручки УСТАНОВКА установить необходимое значение. Затем выделить пункт \"Сохранить\", нажать и удреживать более 0.5 сек кнопку на "
    "панели управления. Меню установки текущего временя закроется с сохранением нового текущего времени. Нажатие длительное удержание кнопки на "
    "любом другом элементе приведёт к закрытию меню установки текущего времени без сохранения нового текущего времени"
    ,
    "Setting the current time. \nPoryadok work:\n"
    "Click on the menu item \"Time\".The menu set the current time.By briefly pressing the button on the numeric keypad of conformity "
    "Control \"Time\", highlight the hours, minutes, seconds, year, month, or a number.The selected item is indicated by a flashing area. "
    "Turn the setting knob to set the desired value. Then highlight \"Save\", press and udrezhivat more than 0.5 seconds, the button on the panel "
    "Control. Menu Setting the current time will be closed to the conservation of the new current time. Pressing a button on the prolonged retention "
    "of any other element will lead to the closure of the current time setting menu without saving the new current time"
    ,
    pService, FuncActive, dServicetime, hours, minutes, secondes, month, day, year
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       cModeLongPressButtonTrig,                                                                        //--- СЕРВИС - Реж длит СИНХР ---
    "Реж длит СИНХР", "Mode long СИНХР"
    ,
    "Устанавливает действия для длительного нажатия кнопки СИНХР:\n\"Сброс уровня\" - установка уровня синхронизации в ноль,\n\"Автоуровень\" "
    "- автоматическое определение и установка уровня синхронизации"
    ,
    "Sets the action for long press CLOCK:\n\"Reset trig lvl\" - to set the trigger level to zero, \n\"Auto level\""
    "- Automatically detect and install the trigger level",
    "Сброс уровня", "Reset trig level",
    "Автоуровень",  "Autolevel",
    MODE_LONG_PRESS_TRIG, pService, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Information_Exit()
{
    Display::SetDrawMode(DrawMode_Auto, 0);
    Display::RemoveAddDrawFunction();
}

DEF_SMALL_BUTTON_EXIT(  sbInformation_Exit,                                                                      //--- СЕРВИС - ИНФОРМАЦИЯ - Выход ---
    ppInformation, FuncActive, OnPress_Information_Exit, DrawSB_Exit
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Information_Draw()
{
    Language lang = LANG;

    Painter::BeginScene(Color::BLACK);
    int x = 100;
    int dY = 20;
    int y = 20;
    Painter::DrawRectangle(0, 0, 319, 239, Color::FILL);
    y += dY;
    Painter::DrawText(x, y, lang == Russian ? "ИНФОРМАЦИЯ" : "INFORMATION");
    y += dY;
    Painter::DrawText(x, y, lang == Russian ? "Модель : С8-53/1" : "Model : S8-53/1");
    y += dY;

    char buffer[100];
    Painter::DrawText(x, y, lang == Russian ? "Программное обеспечение:" : "Software:");
    y += dY;
    sprintf(buffer, (const char*)((lang == Russian) ? "версия %s" : "version %s"), NUM_VER);
    Painter::DrawText(x, y, buffer);
    y += dY;

    Painter::SetColor(Color::FILL);
    Painter::DrawFormatText(x, y, "CRC32 : %X", Hardware_CalculateCRC32());

    dY = -10;
    Painter::DrawStringInCenterRect(0, 190 + dY, 320, 20, "Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ");
    Painter::DrawStringInCenterRect(0, 205 + dY, 320, 20, "Отдел маркетинга: тел./факс. 8-017-262-57-50");
    Painter::DrawStringInCenterRect(0, 220 + dY, 320, 20, "Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-262-57-51");

    Menu::Draw();
    Painter::EndScene();
}

static void OnPress_Information()
{
    Menu::OpenPageAndSetItCurrent(PageSB_Service_Information);
    Display::SetDrawMode(DrawMode_Hand, Information_Draw);
}

DEF_PAGE_SB(        ppInformation,                                                                                          // СЕРВИС - ИНФОРМАЦИЯ ///
    "ИНФОРМАЦИЯ", "INFORMATION",
    "Выводит на экран идентификационные данные осциллографа",
    "Displays identification data of the oscilloscope",
    &sbInformation_Exit, // СЕРВИС - ИНФОРМАЦИЯ - Выход
    0,
    0,
    0,
    0,
    0,
    PageSB_Service_Information, &pService, FuncActive, OnPress_Information, FuncDrawPage, FuncRegSetPage
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_10(        pService,                                                                                                            // СЕРВИС ///
    "СЕРВИС", "SERVICE",
    "Дополнительные настройки, калибровка, поиск сигнала, математические функции",
    "Additional settings, calibration, signal search, mathematical functions",
    bResetSettings,           // СЕРВИС - Сброс настроек
    bAutoSearch,              // СЕРВИС - Поиск сигнала
    ppCalibrator,             // СЕРВИС - КАЛИБРАТОР
    ppMath,                   // СЕРВИС - МАТЕМАТИКА
    ppEthernet,               // СЕРВИС - ETHERNET
    cSound,                   // СЕРВИС - Звук
    cLang,                    // СЕРВИС - Язык
    tTime,                    // СЕРВИС - Время
    cModeLongPressButtonTrig, // СЕРВИС - Реж длит СИНХР
    ppInformation,            // СЕРВИС - ИНФОРМАЦИЯ
    Page_Service, &mainPage, FuncActive, FuncPress
);


/** @}  @}
 */
