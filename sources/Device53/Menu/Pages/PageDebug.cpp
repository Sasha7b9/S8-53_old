#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "FlashDrive/FlashDrive.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGATypes.h"
#include "Hardware/FSMC.h"
#include "Hardware/Sound.h"
#include "Hardware/FLASH.h"
#include "Menu/Menu.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Utils/Dictionary.h"


/** @addtogroup Menu
 *  @{
 *  @defgroup PageDebug
 *  @{
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// В этой структуре будут храниться данные серийного номера при открытой странице ppSerialNumer
typedef struct
{
    int number;     ///< Соответственно, порядковый номер.
    int year;       ///< Соответственно, год.
    int curDigt;    ///< Соответственно, номером (0) или годом (1) управляет ручка УСТАНОВКА.
} StructForSN;


extern const PageBase mainPage;
extern const PageBase mpConsole_Registers;
extern const PageBase pDebug;
extern const PageBase mpConsole;
extern const PageBase mpADC;
extern const PageBase mpADC_Balance;
extern const PageBase mpADC_Stretch;
extern const PageBase mpADC_AltRShift;
extern const PageBase mpRandomizer;
extern const PageBase ppSerialNumber;

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_ShowAll,                                               //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все ---
    "Показывать все", "Show all",
    "Показывать все значения, засылаемые в регистры",
    "To show all values transferred in registers",
    "Нет", "No",
    "Да",  "Yes",
    IS_SHOW_REGISTERS_ALL, mpConsole_Registers, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Console_Registers()
{
    return !IS_SHOW_REGISTERS_ALL;
}

DEF_CHOICE_2(       mcConsole_Registers_RD_FL,                                                          //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL ---
    "RD_FL", "RD_FL",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.flag, mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_RShiftA,                                                     //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к ---
    "U см. 1к", "U shift 1ch",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.rShiftA, mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_RShiftB,                                                     //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к ---
    "U см. 2к", "U shift 2ch",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.rShiftB, mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_TrigLev,                                                     //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр. ---
    "U синхр.", "U trig.",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.trigLev, mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_RangeA,                                                   //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1 ---
    "ВОЛЬТ/ДЕЛ 1", "Range 1",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.range[A], mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_RangeB,                                                   //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2 ---
    "ВОЛЬТ/ДЕЛ 2", "Range 2",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.range[B], mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_TrigParam,                                              //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр. ---
    "Парам. синхр.", "Trig param",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.trigParam, mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_ChanParamA,                                             //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1 ---
    "Парам. кан. 1",  "Chan 1 param",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.chanParam[A], mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_ChanParamB,                                             //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2 ---
    "Парам. кан. 2", "Chan 2 param",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.chanParam[B], mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_TBase,                                                      //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ ---
    "ВРЕМЯ/ДЕЛ", "TBase",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.tBase, mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_Registers_tShift,                                                         //--- ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см. ---
    "Т см.", "tShift",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    set.debug.showRegisters.tShift, mpConsole_Registers, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_12(        mpConsole_Registers,                                                                           // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ ///
    Page_DebugShowRegisters,  &mpConsole, FuncActive, EmptyPressPage,
    "РЕГИСТРЫ", "REGISTERS",
    "",
    "",
    mcConsole_Registers_ShowAll,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все
    mcConsole_Registers_RD_FL,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL
    mcConsole_Registers_RShiftA,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к
    mcConsole_Registers_RShiftB,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к
    mcConsole_Registers_TrigLev,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр.
    mcConsole_Registers_RangeA,     // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1
    mcConsole_Registers_RangeB,     // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2
    mcConsole_Registers_TrigParam,  // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр.
    mcConsole_Registers_ChanParamA, // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1
    mcConsole_Registers_ChanParamB, // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
    mcConsole_Registers_TBase,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ
    mcConsole_Registers_tShift      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см.
)

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int16 shiftADCA;
static int16 shiftADCB;

static void Draw_ADC_Balance_Mode(int x, int y)
{
    int8 shift[2][3] =
    {
        {0, (int8)SET_BALANCE_ADC_A, (int8)BALANCE_ADC_A},
        {0, (int8)SET_BALANCE_ADC_B, (int8)BALANCE_ADC_B}
    };

    shiftADCA = shift[0][BALANCE_ADC_TYPE];
    shiftADCB = shift[1][BALANCE_ADC_TYPE];
}

static void OnChanged_ADC_Balance_Mode(bool active)
{
    Draw_ADC_Balance_Mode(0, 0);

    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)shiftADCA, false);
    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)shiftADCB, false);
}

DEF_CHOICE_3(       mcADC_Balance_Mode,                                                                       //--- ОТЛАДКА - АЦП - БАЛАНС - Режим ---
    "Режим", "Mode",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    "Реальный", "Real",
    "Ручной",   "Manual",
    BALANCE_ADC_TYPE, mpADC_Balance, FuncActive, OnChanged_ADC_Balance_Mode, Draw_ADC_Balance_Mode
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_Balance_ShiftA()
{
    BALANCE_ADC_A = shiftADCA;
    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)BALANCE_ADC_A, false);
}

static bool IsActive_ADC_Balance_Shift()
{
    return BALANCE_ADC_TYPE_IS_HAND;
}

DEF_GOVERNOR(       mgADC_Balance_ShiftA,                                                                //--- ОТЛАДКА - АЦП - БАЛАНС - Смещение 1 ---
    "Смещение 1", "Offset 1",
    "",
    "",
    shiftADCA, -125, 125, mpADC_Balance, IsActive_ADC_Balance_Shift, OnChanged_ADC_Balance_ShiftA, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_Balance_ShiftB()
{
    BALANCE_ADC_B = shiftADCB;
    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)BALANCE_ADC_B, false);
}

DEF_GOVERNOR(       mgADC_Balance_ShiftB,                                                                //--- ОТЛАДКА - АЦП - БАЛАНС - Смещение 2 ---
    "Смещение 2", "Offset 2",
    "",
    "",
    shiftADCB, -125, 125, mpADC_Balance, IsActive_ADC_Balance_Shift, OnChanged_ADC_Balance_ShiftB, FuncBeforeDraw
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_3(         mpADC_Balance,                                                                                       // ОТЛАДКА - АЦП - БАЛАНС ///
    "БАЛАНС", "BALANCE",
    "",
    "",
    mcADC_Balance_Mode,     // ОТЛАДКА - АЦП - БАЛАНС - Режим
    mgADC_Balance_ShiftA,   // ОТЛАДКА - АЦП - БАЛАНС - Смещение 1
    mgADC_Balance_ShiftB,   // ОТЛАДКА - АЦП - БАЛАНС - Смещение 2
    Page_DebugADCbalance, &mpADC, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadStretchADC(Channel chan)
{
    if (DEBUG_STRETCH_ADC_TYPE_IS_DISABLED)
    {
        FPGA::WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, 0x80, true);
    }
    else if (DEBUG_STRETCH_ADC_TYPE_IS_HAND)
    {
        FPGA::WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, (uint8)DEBUG_STRETCH_ADC(chan), true);
    }
    else if (DEBUG_STRETCH_ADC_TYPE_IS_SETTINGS)
    {
        FPGA::LoadKoeffCalibration(chan);
    }
}

static void OnChanged_ADC_Stretch_Mode(bool active)
{
    if (active)
    {
        LoadStretchADC(A);
        LoadStretchADC(B);
    }
}

DEF_CHOICE_3(       mcADC_Stretch_Mode,                                                                     //--- ОТЛАДКА - АЦП - РАСТЯЖКА - Режим ---
    "Режим", "Mode",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    "Реальный", "Real",
    "Ручной",   "Manual",
    DEBUG_STRETCH_ADC_TYPE, mpADC_Stretch, FuncActive, OnChanged_ADC_Stretch_Mode, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_ADC_Stretch_ADC()
{
    return DEBUG_STRETCH_ADC_TYPE_IS_HAND;
}

static void OnChanged_ADC_Stretch_ADC_A()
{
    FPGA::WriteToHardware(WR_CAL_A, (uint8)DEBUG_STRETCH_ADC_A, true);
}

DEF_GOVERNOR(       mgADC_Stretch_ADC_A,                                                                //--- ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 1к ---
    "Коэфф. 1к", "Koeff. 1ch",
    "",
    "",
    DEBUG_STRETCH_ADC_A, 0, 255, mpADC_Stretch, IsActive_ADC_Stretch_ADC, OnChanged_ADC_Stretch_ADC_A, FuncBeforeDraw
)

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_Stretch_ADC_B()
{
    FPGA::WriteToHardware(WR_CAL_B, (uint8)DEBUG_STRETCH_ADC_B, true);
}

DEF_GOVERNOR(       mgADC_Stretch_ADC_B,                                                                //--- ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 2к ---
    "Коэфф. 2к", "Koeff. 2ch",
    "",
    "",
    DEBUG_STRETCH_ADC_B, 0, 255, mpADC_Stretch, IsActive_ADC_Stretch_ADC, OnChanged_ADC_Stretch_ADC_B, FuncBeforeDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_3(         mpADC_Stretch,                                                                                     // ОТЛАДКА - АЦП - РАСТЯЖКА ///
    "РАСТЯЖКА", "STRETCH",
    "",
    "",
    mcADC_Stretch_Mode,     // ОТЛАДКА - АЦП - РАСТЯЖКА - Режим
    mgADC_Stretch_ADC_A,    // ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 1к
    mgADC_Stretch_ADC_B,    // ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 2к
    Page_DebugADCstretch, &mpADC, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_ADC_AltRShift_Reset()
{
    for (int chan = 0; chan < 2; chan++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                set.chan[chan].rShiftAdd[range][mode] = 0;
            }
        }
    }
    FPGA::SetRShift(A, SET_RSHIFT_A);
    FPGA::SetRShift(B, SET_RSHIFT_B);
}

DEF_BUTTON(         mbADC_AltRShift_Reset,                                                                  //--- ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс ---
    "Сброс", "Reset",
    "", "",
    mpADC_AltRShift, FuncActive, OnPress_ADC_AltRShift_Reset, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_AltRShift_A()
{
    FPGA::SetRShift(A, SET_RSHIFT_A);
}

DEF_GOVERNOR(       mbADC_AltRShift_2mV_DC_A,                                                      //--- ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост ---
    "См 1к 2мВ пост", "Shift 1ch 2mV DC",
    "",
    "",
    set.chan[A].rShiftAdd[Range_2mV][ModeCouple_DC], -100, 100, mpADC_AltRShift, FuncActive, OnChanged_ADC_AltRShift_A, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_AltRShift_B()
{
    FPGA::SetRShift(B, SET_RSHIFT_B);
}

DEF_GOVERNOR(       mbADC_AltRShift_2mV_DC_B,                                                      //--- ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост ---
    "См 2к 2мВ пост", "Shift 2ch 2mV DC",
    "",
    "",
    set.chan[B].rShiftAdd[Range_2mV][ModeCouple_DC], -100, 100, mpADC_AltRShift, FuncActive, OnChanged_ADC_AltRShift_B, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mbADC_AltRShift_5mV_DC_A,                                                      //--- ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост ---
    "См 1к 5мВ пост", "Shift 1ch 5mV DC",
    "",
    "",
    set.chan[A].rShiftAdd[Range_5mV][ModeCouple_DC], -100, 100, mpADC_AltRShift, FuncActive, OnChanged_ADC_AltRShift_A, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mbADC_AltRShift_5mV_DC_B,                                                         // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост ---
    "См 2к 5мВ пост", "Shift 2ch 5mV DC",
    "",
    "",
    set.chan[B].rShiftAdd[Range_5mV][ModeCouple_DC], -100, 100, mpADC_AltRShift, FuncActive, OnChanged_ADC_AltRShift_B, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mbADC_AltRShift_10mV_DC_A,                                                       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост ---
    "См 1к 10мВ пост", "Shift 1ch 10mV DC",
    "",
    "",
    set.chan[A].rShiftAdd[Range_10mV][ModeCouple_DC], -100, 100, mpADC_AltRShift,  FuncActive, OnChanged_ADC_AltRShift_A, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mbADC_AltRShift_10mV_DC_B,                                                       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост ---
    "См 2к 10мВ пост", "Shift 2ch 10mV DC",
    "",
    "",
    set.chan[B].rShiftAdd[Range_10mV][ModeCouple_DC], -100, 100, mpADC_AltRShift, FuncActive, OnChanged_ADC_AltRShift_B, FuncBeforeDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_7(         mpADC_AltRShift,                                                                                   // ОТЛАДКА - АЦП - ДОП СМЕЩ ///
    "ДОП СМЕЩ", "ADD RSHFIT",
    "",
    "",
    mbADC_AltRShift_Reset,      // ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс
    mbADC_AltRShift_2mV_DC_A,   // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост
    mbADC_AltRShift_2mV_DC_B,   // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост
    mbADC_AltRShift_5mV_DC_A,   // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост
    mbADC_AltRShift_5mV_DC_B,   // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост
    mbADC_AltRShift_10mV_DC_A,  // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост
    mbADC_AltRShift_10mV_DC_B,  // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост
    Page_DebugADCaltShift, &mpADC, FuncActive, EmptyPressPage
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_3(         mpADC,                                                                                                        // ОТЛАДКА - АЦП ///
    "АЦП", "ADC",
    "",
    "",
    mpADC_Balance,      // ОТЛАДКА - АЦП - БАЛАНС
    mpADC_Stretch,      // ОТЛАДКА - АЦП - РАСТЯЖКА
    mpADC_AltRShift,    // ОТЛАДКА - АЦП - ДОП СМЕЩ
    Page_DebugADC, &pDebug, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Randomizer_SamplesForGates()
{
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
}

DEF_GOVERNOR(       mgRandomizer_SamplesForGates,                                                             // ОТЛАДКА - РАНД-ТОР - Выб-к/ворота ---
    "Выб-к/ворота", "Samples/gates",
    "",
    "",
    NUM_MEAS_FOR_GATES, 1, 2500, mpRandomizer, FuncActive, OnChanged_Randomizer_SamplesForGates, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Randomizer_AltTShift0()
{
    FPGA::SetDeltaTShift(ADD_SHIFT_T0);
}

DEF_GOVERNOR(       mgRandomizer_AltTShift0,                                                                   // ОТЛАДКА - РАНД-ТОР - tShift доп. ---
    "tShift доп.", "tShift alt.",
    "",
    "",
    ADD_SHIFT_T0, 0, 510, mpRandomizer, FuncActive, OnChanged_Randomizer_AltTShift0, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mgRandomizer_Average,                                                                          // ОТЛАДКА - РАНД-ТОР - Усредн. ---
    "Усредн.", "Average",
    "",
    "",
    NUM_AVE_FOR_RAND, 1, 32, mpRandomizer, FuncActive, FuncChanged, FuncBeforeDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_3(         mpRandomizer,                                                                                            // ОТЛАДКА - РАНД-ТОР ///
    "РАНД-ТОР", "RANDOMIZER",
    "",
    "",
    mgRandomizer_SamplesForGates, // ОТЛАДКА - РАНД-ТОР - Выб-к/ворота
    mgRandomizer_AltTShift0,      // ОТЛАДКА - РАНД-ТОР - tShift доп.
    mgRandomizer_Average,         // ОТЛАДКА - РАНД-ТОР - Усредн.
    Page_DebugRandomizer, &pDebug, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnDraw_SizeSettings(int x, int y)
{
    Painter::DrawFormatText(x + 5, y + 21, "Размер %d", sizeof(Settings));
}

static int8 temp = 0;

DEF_CHOICE_2(       mcSizeSettings,                                                                                  // ОТЛАДКА - Размер настроек  ---
    "Размер настроек", "Size settings",
    "Вывод размера структуры Settings",
    "Show size of struct Settings",
    "Размер", "Size",
    "Размер", "Size",
    temp, pDebug, FuncActive, FuncChangedChoice, OnDraw_SizeSettings
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_SaveFirmware()
{
    return gBF.flashDriveIsConnected;
}

static void OnPress_SaveFirmware()
{
    StructForWrite structForWrite;

    FDrive::OpenNewFileForWrite("S8-53.bin", &structForWrite);

    uint8 *address = (uint8*)0x08020000;
    uint8 *endAddress = address + 128 * 1024 * 3;

    int sizeBlock = 512;

    while (address < endAddress)
    {
        FDrive::WriteToFile(address, sizeBlock, &structForWrite);
        address += sizeBlock;
    }

    FDrive::CloseFile(&structForWrite);

    Display::ShowWarningGood(FirmwareSaved);
}

DEF_BUTTON(         mbSaveFirmware,                                                                                    // ОТЛАДКА - Сохр. прошивку ---
    "Сохр. прошивку", "Save firmware",
    "Сохранение прошивки - секторов 5, 6, 7 общим объёмом 3 х 128 кБ, где хранится программа",
    "Saving firmware - sectors 5, 6, 7 with a total size of 3 x 128 kB, where the program is stored",
    pDebug, IsActive_SaveFirmware, OnPress_SaveFirmware, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SerialNumber_Exit()
{
    Display::RemoveAddDrawFunction();
    FREE_EXTRAMEM();
}

DEF_SMALL_BUTTON_EXIT(  bSerialNumber_Exit,                                                                            //--- ОТЛАДКА - С/Н - Выход ---
    ppSerialNumber, FuncActive, OnPress_SerialNumber_Exit, DrawSB_Exit
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SerialNumber_Change()
{
    ACCESS_EXTRAMEM(StructForSN, s);
    ++s->curDigt;
    s->curDigt %= 2;
    Painter::ResetFlash();
}

static void Draw_SerialNumber_Change(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_TAB);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   bSerialNumber_Change,                                                                           //--- ОТЛАДКА - С/Н - Вставить ---
    "Вставить", "Insert",
    "Вставляет выбраный символ",
    "Inserts the chosen symbol",
    ppSerialNumber,  FuncActive, OnPress_SerialNumber_Change, Draw_SerialNumber_Change
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SerialNumber_Save()
{
    ACCESS_EXTRAMEM(StructForSN, s);

    char stringSN[20];

    snprintf(stringSN, 19, "%02d %04d", s->number, s->year);

    if (!OTPMem::SaveSerialNumber(stringSN))
    {
        Display::ShowWarningBad(FullyCompletedOTP);
    }
}

static void Draw_SerialNumber_Save(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   bSerialNumber_Save,                                                                               // ОТЛАДКА - С/Н - Сохранить ---
    "Сохранить", "Save",
    "Записывает серийный номер в OTP",
    "Records the serial number in OTP",
    ppSerialNumber, FuncActive, OnPress_SerialNumber_Save, Draw_SerialNumber_Save
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Draw_EnterSerialNumber()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Painter::DrawRectangle(x0, y0, width, height, Color::FILL);
    Painter::FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, Color::BACK);

    int deltaX = 10;

    ACCESS_EXTRAMEM(StructForSN, s);

    bool selNumber = s->curDigt == 0;

    char buffer[20];
    snprintf(buffer, 19, "%02d", s->number);

    Color colorText = Color::FILL;
    Color colorBackground = Color::BACK;

    if (selNumber)
    {
        colorText = Color::FLASH_01;
        colorBackground = Color::FLASH_10;
    }

    int y = y0 + 50;

    Painter::SetColor(colorText);
    int x = Painter::DrawTextOnBackground(x0 + deltaX, y, buffer, colorBackground);

    colorText = Color::FLASH_01;
    colorBackground = Color::FLASH_10;

    if (selNumber)
    {
        colorText = Color::FILL;
        colorBackground = Color::BACK;
    }

    snprintf(buffer, 19, "%04d", s->year);

    Painter::SetColor(colorText);
    Painter::DrawTextOnBackground(x + 5, y, buffer, colorBackground);

    // Теперь выведем информацию об оставшемся месте в OTP-памяти для записи

    int allShots = OTPMem::GetSerialNumber(buffer);

    Painter::SetColor(Color::FILL);

    Painter::DrawFormatText(x0 + deltaX, y0 + 130, "Текущий сохранённый номер %s", buffer[0] == 0 ? "-- ----" : buffer);

    Painter::DrawFormatText(x0 + deltaX, y0 + 100, "Осталось места для %d попыток", allShots);
}

static void OnPress_SerialNumber()
{
    Menu::OpenPageAndSetItCurrent(PageSB_Debug_SerialNumber);
    Display::SetAddDrawFunction(Draw_EnterSerialNumber);
    MALLOC_EXTRAMEM(StructForSN, s);
    s->number = 01;
    s->year = 2017;
    s->curDigt = 0;
}

static void OnRegSet_SerialNumber(int angle)
{
    typedef int(*pFunc)(int*, int, int);

    typedef void (Math::*pFuncRIII)(int&, int, int);

    pFuncVpIII p = angle > 0 ? CircleIncrease<int> : CircleDecrease<int>;

    ACCESS_EXTRAMEM(StructForSN, s);

    if (s->curDigt == 0)
    {
        p(&s->number, 1, 99);
    }
    else
    {
        p(&s->year, 2014, 2050);
    }
    Sound::GovernorChangedValue();
}

DEF_PAGE_SB(        ppSerialNumber,                                                                                               // ОТЛАДКА - С/Н ///
    "С/Н", "S/N",
    "Запись серийного номера в OTP-память. ВНИМАНИЕ!!! ОТP-память - память с однократной записью.",
    "Serial number recording in OTP-memory. ATTENTION!!! OTP memory is a one-time programming memory.",
    &bSerialNumber_Exit,     // ОТЛАДКА - С/Н - Выход
    &bSerialNumber_Change,   // ОТЛАДКА - С/Н - Перейти
    0,
    0,
    0,
    &bSerialNumber_Save,     // ОТЛАДКА - С/Н - Сохранить
    PageSB_Debug_SerialNumber, &pDebug, FuncActive, OnPress_SerialNumber, FuncDrawPage, OnRegSet_SerialNumber
);



//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcStats,                                                                                            //--- ОТЛАДКА - Статистика ---
    "Статистика", "Statistics"
    ,
    "Показывать/не показывать время/кадр, кадров в секунду, количество сигналов с последними настройками в памяти/количество сохраняемых в памяти "
    "сигналов"
    ,
    "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory",
    "Не показывать", "Hide",
    "Показывать",    "Show",
    SHOW_STATS, pDebug, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mgConsole_NumStrings,                                                                    //--- ОТЛАДКА - КОНСОЛЬ - Число строк ---
    "Число строк", "Number strings",
    "",
    "",
    NUM_STRINGS, 0, 33, mpConsole, FuncActive, FuncChanged, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcConsole_SizeFont,                                                                    //--- ОТЛАДКА - КОНСОЛЬ - Размер шрифта ---
    "Размер шрифта", "Size font",
    "",
    "",
    "5", "5",
    "8", "8",
    SIZE_FONT_CONSOLE, mpConsole, FuncActive, FuncChangedChoice, FuncDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_3(         mpConsole,                                                                                                // ОТЛАДКА - КОНСОЛЬ ///
    "КОНСОЛЬ", "CONSOLE",
    "",
    "",
    mgConsole_NumStrings,   // ОТЛАДКА - КОНСОЛЬ - Число строк
    mcConsole_SizeFont,     // ОТЛАДКА - КОНСОЛЬ - Размер шрифта
    mpConsole_Registers,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
    Page_DebugConsole, &pDebug, FuncActive, EmptyPressPage
);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_6(         pDebug,                                                                                                             // ОТЛАДКА ///
    "ОТЛАДКА", "DEBUG",
    "",
    "",
    mcStats,            // ОТЛАДКА - Статистика
    mpConsole,          // ОТЛАДКА - КОНСОЛЬ
    mpADC,              // ОТЛАДКА - АЦП
    mpRandomizer,       // ОТЛАДКА - РАНД-ТОР
    mcSizeSettings,     // ОТЛАДКА - Размер настроек
    mbSaveFirmware,     // ОТЛАДКА - Сохр. прошивку
    Page_Debug, &mainPage, FuncActive, EmptyPressPage
);

/** @}  @}
 */
