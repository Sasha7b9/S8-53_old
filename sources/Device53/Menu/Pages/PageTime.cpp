#include "defines.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "PageMemory.h"
#include "FPGA/FPGA.h"
#include "Log.h"
#include "Display/Display.h"
#include "Utils/Math.h"
#include "Hardware/FSMC.h"
#include "Display/Grid.h"
#include "Utils/Dictionary.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageTime
 *  @{
 */


extern const PageBase pTime;
extern const PageBase mainPage;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Sample()
{
    return sTime_RandomizeModeEnabled();
}

DEF_CHOICE_2(       mcSample,                                                                                            //--- РАЗВЕРТКА - Выборка ---
    "Выборка", "Sampling",
    "\"Реальная\" - \n"
    "\"Эквивалентная\" -",
    "\"Real\" - \n"
    "\"Equals\" - ",
    "Реальное время", "Real",
    "Эквивалентная",  "Equals",
   SAMPLE_TYPE, pTime, IsActive_Sample, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_PeakDet()
{
    return (SET_TBASE >= MIN_TBASE_PEC_DEAT);
}

void OnChanged_PeakDet(bool active)
{
    if (active)
    {
        FPGA::SetPeackDetMode(SET_PEAKDET);
        FPGA::SetTBase(SET_TBASE);
        if (PEAKDET_IS_DISABLE)
        {
            int8 shift[2][3] =
            {
                {0, (int8)SET_BALANCE_ADC_A, (int8)BALANCE_ADC_A},
                {0, (int8)SET_BALANCE_ADC_B, (int8)BALANCE_ADC_B}
            };

            FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC1, shift[0][BALANCE_ADC_TYPE], false);
            FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC2, shift[1][BALANCE_ADC_TYPE], false);
        }
        else
        {
            FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC1, 3, false);     // Почему-то при пиковом детекторе смещение появляется. Вот его и компенсируем.
            FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC2, 3, false);
        }
        if (PEAKDET_IS_DISABLE)
        {
            int centerX = SHIFT_IN_MEMORY + Grid::Width() / 2;
            SHIFT_IN_MEMORY = centerX * 2 - Grid::Width() / 2;
            ENUM_POINTS = set.time.oldNumPoints;
            ChangeC_Memory_NumPoints(true);
        }
        else if (PEAKDET_IS_ENABLE)
        {
            int centerX = SHIFT_IN_MEMORY + Grid::Width() / 2;
            SHIFT_IN_MEMORY = centerX / 2 - Grid::Width() / 2;
            Limitation<int16>(&SHIFT_IN_MEMORY, 0, sMemory_GetNumPoints(false) - Grid::Width());
            ChangeC_Memory_NumPoints(true);
        }
    }
    else
    {
        Display::ShowWarningBad(TooSmallSweepForPeakDet);
    }
}

DEF_CHOICE_2(       mcPeakDet,                                                                                           //--- РАЗВЕРТКА - Пик дет ---
    "Пик дет", "Pic deat",
    "Включает/выключает пиковый детектор.",
    "Turns on/off peak detector.",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
   SET_PEAKDET, pTime, IsActive_PeakDet, OnChanged_PeakDet, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_TPos(bool active)
{
    ChangeC_Memory_NumPoints(active);
    FPGA::SetTShift(TSHIFT);
}

DEF_CHOICE_3(       mcTPos,                                                                                                   //--- РАЗВЕРТКА - To ---
    "\x7b", "\x7b",
    "Задаёт точку привязки нулевого смещения по времени к экрану - левый край, центр, правый край.",
    "Sets the anchor point nuleovgo time offset to the screen - the left edge, center, right edge.",
    "Лево",  "Left",
    "Центр", "Center",
    "Право", "Right",
    TPOS, pTime, FuncActive, OnChanged_TPos, FuncDraw
);


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_SelfRecorder()
{
    return SET_TBASE >= MIN_TBASE_P2P;
}

DEF_CHOICE_2(       mcSelfRecorder,                                                                                    //--- РАЗВЕРТКА - Самописец ---
    "Самописец", "Self-Recorder",
    "Включает/выключает режим самописца. Этот режим доступен на развёртках 20мс/дел и более медленных.",
    "Turn on/off the recorder. This mode is available for scanning 20ms/div and slower.",
    ENABLE_RU,  ENABLE_EN,
    DISABLE_RU, DISABLE_EN,
    SELFRECORDER, pTime, IsActive_SelfRecorder, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcDivRole,                                                                                      //--- РАЗВЕРТКА - Ф-ция ВР/ДЕЛ ---
    "Ф-ция ВР/ДЕЛ", "Func Time/DIV"
    ,
    "Задаёт функцию для ручки ВРЕМЯ/ДЕЛ: в режиме сбора информации (ПУСК/СТОП в положении ПУСК):\n"
    "1. \"Время\" - изменение смещения по времени.\n"
    "2. \"Память\" - перемещение по памяти."
    ,
    "Sets the function to handle TIME/DIV: mode of collecting information (START/STOP to start position):\n"
    "1. \"Time\" - change the time shift.\n"
    "2. \"Memory\" - moving from memory.",
    "Время",  "Time",
    "Память", "Memory",
    TIME_DIV_XPOS, pTime, FuncActive, FuncChangedChoice, FuncDraw
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_5(         pTime,                                                                                                            // РАЗВЕРТКА ///
    "РАЗВЕРТКА", "SCAN",
    "Содержит настройки развёртки.",
    "Contains scan settings.",
    mcSample,       // РАЗВЕРТКА - Выборка
    mcPeakDet,      // РАЗВЕРТКА - Пик дет
    mcTPos,         // РАЗВЕРТКА - To
    mcSelfRecorder, // РАЗВЕРТКА - Самописец
    mcDivRole,      // РАЗВЕРТКА - Ф-ция ВР/ДЕЛ
    Page_Time, &mainPage, FuncActive, EmptyPressPage
);


/** @}  @}
 */
