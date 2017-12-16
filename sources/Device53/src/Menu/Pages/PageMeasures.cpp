#include "PageMemory.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Measures.h"
#include "Utils/Math.h"
#include "Menu/Pages/Definition.h"
#include "Menu/MenuItems.h"
#include "Utils/CommonFunctions.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Colors.h"
#include "defines.h"
#include "Display/Painter.h"
#include "stub.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageMeasures
 *  @{
 */

extern const PageBase pMeasures;
extern const PageBase mainPage;
extern const PageBase mspMeasTune;

static CursCntrl    GetMeasuresCursCntrlActive();           // Каким курсором из активной пары сейчас происходит управление.


void DrawSB_MeasTune_Settings(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x62');
    Painter::SetFont(TypeFont_8);
}

void PressSB_MeasTune_Settings()
{
    Measures_ShorPressOnSmallButtonSettings();
}

void DrawSB_MeasTune_Markers(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x60');
    Painter::SetFont(TypeFont_8);
}

void PressSB_MeasTune_Markers()
{
    Measure_ShortPressOnSmallButonMarker();
}

void RotateRegMeasureSetField(int angle)
{
    CursCntrl cursCntrl = GetMeasuresCursCntrlActive();
    CursActive cursActive = MEAS_CURS_ACTIVE;

    int sign = Sign(angle);

    if (cursCntrl == CursCntrl_1 || cursCntrl == CursCntrl_1_2)
    {
        if (cursActive == CursActive_T)
        {
            MEAS_POS_CUR_T0 -= sign;
            Limitation<int16>(&MEAS_POS_CUR_T0, 0, sMemory_GetNumPoints(false) - 1);
        }
        else
        {
            MEAS_POS_CUR_U0 += sign;
            Limitation<int16>(&MEAS_POS_CUR_U0, 0, 200);
        }
    }
    if (cursCntrl == CursCntrl_2 || cursCntrl == CursCntrl_1_2)
    {
        if (cursActive == CursActive_T)
        {
            MEAS_POS_CUR_T1 -= sign;
            Limitation<int16>(&MEAS_POS_CUR_T1, 0, sMemory_GetNumPoints(false) - 1);
        }
        else
        {
            MEAS_POS_CUR_U1 += sign;
            Limitation<int16>(&MEAS_POS_CUR_U1, 0, 200);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON(   sbMeasTuneSettings,                                                                    //--- ИЗМЕРЕНИЯ - НАСТРОИТЬ - Настройка ---
    "Настройка", "Setup",
    "Позволяет выбрать необходимые измерения",
    "Allows to choose necessary measurements",
    mspMeasTune, FuncActive, PressSB_MeasTune_Settings, DrawSB_MeasTune_Settings
);

DEF_SMALL_BUTTON(   sbMeasTuneMarkers,                                                                        //--- ИЗМЕРЕНИЯ - НАСТРОИТЬ - Маркер ---
    "Маркер", "Marker",
    "Позволяет установить маркеры для визуального контроля измерений",
    "Allows to establish markers for visual control of measurements",
    mspMeasTune, FuncActive, PressSB_MeasTune_Markers, DrawSB_MeasTune_Markers
);

/*
static bool IsActiveChoiceMeasuresChannels()
{
    return SHOW_MEASURES;
}
*/

/*
static bool IsActivePageMeasuresFields()
{
    return SHOW_MEASURES;
}
*/

static bool IsActiveChoiceMeasuresSignal()
{
    return SHOW_MEASURES;
}

static bool IsActiveButtonMeasuresTune()
{
    return SHOW_MEASURES;
}

/*
static bool IsActiveButtonMeasuresFieldSet()
{
    return MEAS_FIELD_IS_HAND;
}
*/

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActiveChoiceMeasuresNumber()
{
    return SHOW_MEASURES;
}

DEF_CHOICE_7(       mcMeasuresNumber,                                                                                 //--- ИЗМЕРЕНИЯ - Количество ---
    "Количество", "Number"
    ,
    "Устанавливает максимальное количество выводимых измерений:\n"
    "\"1\" - одно измерение\n"
    "\"2\" - два измерения\n"
    "\"1х5\" - 1 строка с пятью измерениями\n"
    "\"2х5\" - 2 строки с пятью измерениями в каждой\n"
    "\"3х5\" - 3 строки с пятью измерениями в каждой\n"
    "\"6x1\" - 6 строк по одному измерению в каждой\n"
    "\"6х2\" - 6 строк по два измерения в каждой"
    ,
    "Sets the maximum number of output measurements:\n"
    "\"1\" - one measurement\n"
    "\"2\" - two measurements\n"
    "\"1x5\" - 1 line with the five dimensions\n"
    "\"2x5\" - two rows with five measurements in each\n"
    "\"3x5\" - 3 lines with five measurements in each"
    "\"6x1\" - 6 lines, one in each dimension\n"
    "\"6x2\" - 6 lines of two dimensions in each\n"
    ,
    "1",   "1",
    "2",   "2",
    "1x5", "1x5",
    "2x5", "2x5",
    "3x5", "3x5",
    "6x1", "6x1",
    "6x2", "6x2",
    NUM_MEASURES, pMeasures, IsActiveChoiceMeasuresNumber, FuncChangedChoice, FuncDraw
)

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_3(   mcMeasuresChannels,                                                                                       //--- ИЗМЕРЕНИЯ - Каналы ---
    "Каналы", "Channels",
    "По каким каналам выводить измерения",
    "Which channels to output measurement",
    "1",     "1",
    "2",     "2",
    "1 и 2", "1 and 2",
    SOURCE_MEASURE, pMeasures, IsActiveChoiceMeasuresNumber, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMeasuresIsShow,                                                                                 //--- ИЗМЕРЕНИЯ - Показывать ---
    "Показывать", "Show",
    "Выводить или не выводить измерения на экран",
    "Output or output measurements on screen",
    "Нет", "No",
    "Да",  "Yes",
    SHOW_MEASURES, pMeasures, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMeasuresSignal,                                                                                        //--- ИЗМЕРЕНИЯ - Вид ---
    "Вид", "View",
    "Уменьшать или нет зону вывода сигнала для исключения перекрытия его результами измерений",
    "Decrease or no zone output signal to avoid overlapping of its measurement results",
    "Как есть",  "As is",
    "Уменьшать", "Reduce",
    MODE_VIEW_SIGNALS, pMeasures, IsActiveChoiceMeasuresSignal, FuncChangedChoice, FuncDraw
);

/*
// ИЗМЕРЕНИЯ -> ЗОНА -> Область
const Choice mcMeasuresFieldType =
{
    Item_Choice, &mspMeasuresField, 0,
    {
        "Область", "Field",
        "Выбор области, из которой будут браться значения для автоматических измерений",
        "Select an area from which the values will be taken for automatic measurements"
    },
    {   
        {"Экран",   "Screen"},
        {"Память",  "Memorye"},
        {"Курсоры", "Cursors"}
    },
    (int8*)&MEAS_FIELD
};
*/

CursCntrl GetMeasuresCursCntrlActive()
{
    if(MEAS_CURS_ACTIVE_IS_T)
    {
        return MEAS_CURS_CNTRL_T;
    }
    return MEAS_CURS_CNTRL_U;
}

static void PressSB_MeasTune_Exit()
{
    Display::RemoveAddDrawFunction();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON_EXIT(  sbExitMeasTune,                                                                        //--- ИЗМЕРЕНИЯ - НАСТРОИТЬ - Выход ---
    mspMeasTune, FuncActive, PressSB_MeasTune_Exit, DrawSB_Exit
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_SB(        mspMeasTune,                                                                                          // ИЗМЕРЕНИЯ - Настроить ///
    "НАСТРОИТЬ", "CONFIGURE",
    "Переход в режми точной настройки количества и видов измерений",
    "Transition to rezhm of exact control of quantity and types of measurements",
    &sbExitMeasTune,
    0,
    0,
    0,
    &sbMeasTuneMarkers,
    &sbMeasTuneSettings,
    PageSB_Measures_Tune, &pMeasures, IsActiveButtonMeasuresTune, EmptyPressPage, FuncDrawPage, Measure_RotateRegSet
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_5(         pMeasures,                                                                                                        // ИЗМЕРЕНИЯ ///
    "ИЗМЕРЕНИЯ", "MEASURES",
    "Автоматические измерения",
    "Automatic measurements",
    mcMeasuresIsShow,
    mcMeasuresNumber,
    mcMeasuresChannels,
    mcMeasuresSignal,
    mspMeasTune,
    Page_Measures, &mainPage, FuncActive, EmptyPressPage
);

/** @}  @}
 */
