#include "Menu/MenuItems.h"
#include "defines.h"
#include "Menu/Menu.h"
#include "Tables.h"
#include "Display/Painter.h"
#include "FPGA/FPGA.h"
#include "FPGA/DataStorage.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Utils/Dictionary.h"


/** @addtogroup Menu
 *  @{
 *  @defgroup PageDisplay
 *  @{
 */

extern const PageBase mainPage;
extern const PageBase pDisplay;
extern const PageBase mspAccumulation;
extern const PageBase mspAveraging;
extern const PageBase mspGrid;
extern const PageBase mspSettings_Colors;
extern const PageBase mspSettings;

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMapping,                                                                                         //--- ДИСПЛЕЙ - Отображение ---
    "Отображение", "View",
    "Задаёт режим отображения сигнала.",
    "Sets the display mode signal.",
    "Вектор", "Vector",
    "Точки",  "Points",
    MODE_DRAW_SIGNAL, pDisplay, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_REG_9(   mcAccumulation_Number,                                                                    // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Количество ---
    "Количество", "Number"
    ,
    "Задаёт максимальное количество последних сигналов на экране. Если в настройке \"Режим\" выбрано \"Бесконечность\", экран очищается только "
    "нажатием кнопки \"Очистить\"."
    "\"Бесконечность\" - каждое измерение остаётся на дисплее до тех пор, пока не будет нажата кнопка \"Очистить\"."
    ,
    "Sets the maximum quantity of the last signals on the screen. If in control \"Mode\" it is chosen \"Infinity\", the screen is cleared only "
    "by pressing of the button \"Clear\"."
    "\"Infinity\" - each measurement remains on the display until the button \"Clear\" is pressed.",
    DISABLE_RU,      DISABLE_EN,
    "2",             "2",
    "4",             "4",
    "8",             "8",
    "16",            "16",
    "32",            "32",
    "64",            "64",
    "128",           "128",
    "Бесконечность", "Infinity",
    ENUM_ACCUM, mspAccumulation, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcAccumulation_Mode,                                                                           // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Режим ---
    "Режим", "Mode"
    ,
    "1. \"Сбрасывать\" - после накопления заданного количества измерения происходит очистка дисплея. Этот режим удобен, когда памяти не хватает "
    "для сохранения нужного количества измерений.\n"
    "2. \"Не сбрасывать\" - на дисплей всегда выводится заданное или меньшее (в случае нехватки памяти) количество измерений. Недостатком является "
    "меньшее быстродействие и невозможность накопления заданного количества измерений при недостатке памяти."
    ,
    "1. \"Dump\" - after accumulation of the set number of measurement there is a cleaning of the Display:: This mode is convenient when memory "
    "isn't enough for preservation of the necessary number of measurements.\n"
    "2. \"Not to dump\" - the number of measurements is always output to the display set or smaller (in case of shortage of memory). Shortcoming "
    "is smaller speed and impossibility of accumulation of the set number of measurements at a lack of memory.",
    "Не сбрасывать", "Not to dump",
    "Сбрасывать",    "Dump",
    MODE_ACCUM, mspAccumulation, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Accumulation_Clear()
{
    return ENUM_ACCUM_IS_INFINITY;
}

void OnPress_Accumulation_Clear()
{
    Display::Redraw();
}

DEF_BUTTON(mcAccumulation_Clear,                                                                       // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Очистить ///
    "Очистить", "Clear",
    "Очищает экран от накопленных сигналов.",
    "Clears the screen of the saved-up signals.",
    mspAccumulation, IsActive_Accumulation_Clear, OnPress_Accumulation_Clear, FuncDraw
)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsActive_Accumulation()
{
    return SET_TBASE > TBase_50ns;
}

DEF_PAGE_3(         mspAccumulation,                                                                                       // ДИСПЛЕЙ - НАКОПЛЕНИЕ ///
    "НАКОПЛЕНИЕ", "ACCUMULATION",
    "Настройки режима отображения последних сигналов на экране.",
    "Mode setting signals to display the last screen.",
    mcAccumulation_Number, // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Количество
    mcAccumulation_Mode,   // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Режим
    mcAccumulation_Clear,  // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Очистить
    Page_Display_Accum, &pDisplay, IsActive_Accumulation, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_REG_10(  mcAveraging_Number,                                                                       // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Количество ---
    "Количество", "Number",
    "Задаёт количество последних измерений, по которым производится усреднение.",
    "Sets number of the last measurements on which averaging is made.",
    DISABLE_RU, DISABLE_EN,
    "2",   "2",
    "4",   "4",
    "8",   "8",
    "16",  "16",
    "32",  "32",
    "64",  "64",
    "128", "128",
    "256", "256",
    "512", "512",
    ENUM_AVE, mspAveraging, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcAveraging_Mode,                                                                              // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Режим ---
    "Режим", "Mode"
    ,
    "1. \"Точно\" - точный режим усреднения, когда в расчёте участвуют только последние сигналы.\n"
    "2. \"Приблизительно\" - приблизительный режим усреднения. Имеет смысл использовать, когда задано количество измерений большее, чем может "
    "поместиться в памяти."
    ,
    "1. \"Accurately\" - the exact mode of averaging when only the last signals participate in calculation.\n"
    "2. \"Around\" - approximate mode of averaging. It makes sense to use when the number of measurements bigger is set, than can be located in "
    "memory.",
    "Точно",          "Accurately",
    "Приблизительно", "Around",
    MODE_AVE, mspAveraging, FuncActive, FuncChangedChoice, FuncDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsActive_Averaging()
{
    return true;
}

DEF_PAGE_2(         mspAveraging,                                                                                          // ДИСПЛЕЙ - УСРЕДНЕНИЕ ///
    "УСРЕДНЕНИЕ", "AVERAGE",
    "Настройки режима усреднения по последним измерениям.",
    "Settings of the mode of averaging on the last measurements.",
    mcAveraging_Number,     // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Количество
    mcAveraging_Mode,       // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Режим
   Page_Display_Average, &pDisplay, IsActive_Averaging, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_MinMax()
{
    return SET_TBASE > TBase_50ns;
}

static void OnChanged_MinMax(bool active)
{
}

DEF_CHOICE_8(       mcMinMax,                                                                                                // ДИСПЛЕЙ - Мин Макс ---
    "Мин Макс", "Min Max"
    ,
    "Задаёт количество последних измерений, по которым строятся ограничительные линии, огибающие минимумы и максимумы измерений."
    ,
    "Sets number of the last measurements on which the limiting lines which are bending around minima and maxima of measurements are under ",
    DISABLE_RU, DISABLE_EN,
    "2",        "2",
    "4",        "4",
    "8",        "8",
    "16",       "16",
    "32",       "32",
    "64",       "64",
    "128",      "128",
    ENUM_MIN_MAX, pDisplay, IsActive_MinMax, OnChanged_MinMax, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_REG_10(  mcSmoothing,                                                                                          // ДИСПЛЕЙ - Сглаживание ---
    "Сглаживание", "Smoothing",
    "Устанавливает количество точек для расчёта сглаженного по соседним точкам сигнала.",
    "Establishes quantity of points for calculation of the signal smoothed on the next points.",
    DISABLE_RU, DISABLE_EN,
    "2 точки",  "2 points",
    "3 точки",  "3 points",
    "4 точки",  "4 points",
    "5 точек",  "5 points",
    "6 точек",  "6 points",
    "7 точек",  "7 points",
    "8 точек",  "8 points",
    "9 точек",  "9 points",
    "10 точек", "10 points",
    SMOOTHING, pDisplay, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_RefreshFPS(bool active)
{
    FPGA::SetNumSignalsInSec(sDisplay_NumSignalsInS());
}

DEF_CHOICE_5(       mcRefreshFPS,                                                                                      // ДИСПЛЕЙ - Частота обновл ---
    "Частота обновл", "Refresh rate",
    "Задаёт максимальное число выводимых в секунду кадров.",
    "Sets the maximum number of the shots removed in a second.",
    "25", "25",
    "10", "10",
    "5",  "5",
    "2",  "2",
    "1",  "1",
    ENUM_SIGNALS_IN_SEC, pDisplay, FuncActive, OnChanged_RefreshFPS, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_4(       mcGrid_Type,                                                                                          // ДИСПЛЕЙ - СЕТКА - Тип ---
    "Тип", "Type",
    "Выбор типа сетки.",
    "Choice like Grid::",
    "Тип 1", "Type 1",
    "Тип 2", "Type 2",
    "Тип 3", "Type 3",
    "Тип 4", "Type 4",
    TYPE_GRID, mspGrid, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
ColorType colorTypeGrid = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::GRID};

void OnChanged_Grid_Brightness()
{
    colorTypeGrid.SetBrightness(BRIGHTNESS_GRID / 1e2f);
}

static void BeforeDraw_Grid_Brightness()
{
    colorTypeGrid.Init(false);
    BRIGHTNESS_GRID = (int16)(colorTypeGrid.brightness * 100.0f);
}

DEF_GOVERNOR(       mgGrid_Brightness,                                                                                // ДИСПЛЕЙ - СЕТКА - Яркость ---
    "Яркость", "Brightness",
    "Устанавливает яркость сетки.",
    "Adjust the brightness of the Grid::",
    BRIGHTNESS_GRID, 0, 100, mspGrid, FuncActive, OnChanged_Grid_Brightness, BeforeDraw_Grid_Brightness
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_2(         mspGrid,                                                                                                    // ДИСПЛЕЙ - СЕТКА ///
    "СЕТКА", "GRID",
    "Содержит настройки отображения координатной сетки.",
    "Contains settings of display of a coordinate Grid::",
    mcGrid_Type,        // ДИСПЛЕЙ - СЕТКА - Тип
    mgGrid_Brightness,  // ДИСПЛЕЙ - СЕТКА - Яркость
    Page_Display_Grid, &pDisplay, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcTypeShift,                                                                                             // ДИСПЛЕЙ - Смещение ---
    "Смещение", "Offset"
    ,
    "Задаёт режим удержания смещения по вертикали\n1. \"Напряжение\" - сохраняется напряжение смещения.\n2. \"Деления\" - сохраняется положение "
    "смещения на экране."
    ,
    "Sets the mode of retaining the vertical displacement\n1. \"Voltage\" - saved dressing bias.\n2. \"Divisions\" - retained the position of "
    "the offset on the screen.",
    "Напряжение", "Voltage",
    "Деления",    "Divisions",
    LINKING_RSHIFT, pDisplay, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcSettings_Colors_Scheme,                                                   //--- ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема ---
    "Цветовая схема", "Color scheme",
    "Режим работы калибратора",
    "Mode of operation of the calibrator",
    "Схема 1", "Scheme 1",
    "Схема 2", "Scheme 2",
    COLOR_SCHEME, mspSettings_Colors, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static ColorType colorT1 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::CHAN[A]};
DEF_GOVERNOR_COLOR( mgcSettings_Colors_ChannelA,                                                       //--- ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1 ---
    "Канал 1", "Channel 1",
    "",  "",
    colorT1, mspSettings_Colors
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static ColorType colorT2 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::CHAN[B]};
DEF_GOVERNOR_COLOR( mgcSettings_Colors_ChannelB,                                                       //--- ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2 ---
    "Канал 2", "Channel 2",
    "",
    "",
    colorT2, mspSettings_Colors
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR_COLOR( mgcSettings_Colors_Grid,                                                             //--- ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка ---
    "Сетка", "Grid",
    "Устанавливает цвет сетки",
    "Sets the grid color",
    colorTypeGrid, mspSettings_Colors
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_4(         mspSettings_Colors,                                                                             // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА ///
    "ЦВЕТА", "COLORS",
    "Выбор цветов дисплея",
    "The choice of colors display",
    mcSettings_Colors_Scheme,    // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема
    mgcSettings_Colors_ChannelA, // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1
    mgcSettings_Colors_ChannelB, // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2
    mgcSettings_Colors_Grid,     // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка
    Page_Service_Display_Colors, &mspSettings, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Settings_Brightness()
{
    Painter::SetBrightnessDisplay(BRIGHTNESS);
}

DEF_GOVERNOR(       mgSettings_Brightness,                                                                        // ДИСПЛЕЙ - НАСТРОЙКИ - Яркость ---
    "Яркость", "Brightness",
    "Установка яркости свечения дисплея",
    "Setting the brightness of the display",
    BRIGHTNESS, 0, 100, mspSettings, FuncActive, OnChanged_Settings_Brightness, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mgSettings_Levels,                                                                             // ДИСПЛЕЙ - НАСТРОЙКИ - Уровни ---
    "Уровни", "Levels",
    "Задаёт время, в течение которого после поворота ручки сещения напряжения на экране остаётся вспомогательная метка уровня смещения",
    "Defines the time during which, after turning the handle visits to the voltage on the screen remains auxiliary label offset level",
    TIME_SHOW_LEVELS, 0, 125, mspSettings, FuncActive, FuncChanged, FuncBeforeDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       mgSettings_TimeMessages,                                                                        // ДИСПЛЕЙ - НАСТРОЙКИ - Время ---
    "Время", "Time",
    "Установка времени, в течение которого сообщения будут находиться на экране",
    "Set the time during which the message will be on the screen",
    TIME_MESSAGES, 1, 99, mspSettings, FuncActive, FuncChanged, FuncBeforeDraw
)

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcSettings_ShowStringNavigation,                                                          // ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню ---
    "Строка меню", "Path menu",
    "При выборе \nПоказывать\n слева вверху экрана выводится полный путь до текущей страницы меню",
    "When choosing \nDisplay\n at the top left of the screen displays the full path to the current page menu",
    "Скрывать",   "Hide",
    "Показывать", "Show",
    SHOW_STRING_NAVIGATION, mspSettings, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Settings_ShowAltMarkers(bool)
{
    Display::ChangedRShiftMarkers();
}

DEF_CHOICE_3(       mcSettings_ShowAltMarkers,                                                            //--- ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры ---
    "Доп. маркеры", "Alt. markers"
    ,
    "Устанавливает режим отображения дополнительных маркеров уровней смещения и синхронизации:\n"
    "\"Скрывать\" - дополнительные маркеры не показываются,\n"
    "\"Показывать\" - дополнительные маркеры показываются всегда,\n"
    "\"Авто\" - дополнительные маркеры показываются в течение 5 сек после поворота ручки смещения канала по напряжению или уровня синхронизации"
    ,
    "Sets the display mode of additional markers levels of displacement and synchronization:\n"
    "\"Hide\" - additional markers are not shown,\n"
    "\"Show\" - additional markers are shown always,\n"
    "\"Auto\" - additional markers are displayed for 5 seconds after turning the handle channel offset voltage or trigger level",
    "Скрывать",   "Hide",
    "Показывать", "Show",
    "Авто",       "Auto",
    ALT_MARKERS, mspSettings, FuncActive, OnChanged_Settings_ShowAltMarkers, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Settings_AutoHide(bool autoHide)
{
    Menu::SetAutoHide(autoHide);
}

DEF_CHOICE_6(       mcSettings_AutoHide,                                                                      //--- ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать ---
    "Скрывать", "Hide",
    "Установка после последнего нажатия кнопки или поворота ручки, по истечении которого меню автоматически убирается с экрана",
    "Installation after the last keystroke or turning the handle, after which the menu will automatically disappear",
    "Никогда",      "Never",
    "Через 5 сек",  "Through 5 s",
    "Через 10 сек", "Through 10 s",
    "Через 15 сек", "Through 15 s",
    "Через 30 сек", "Through 30 s",
    "Через 60 сек", "Through 60 s",
    MENU_AUTO_HIDE, mspSettings, FuncActive, OnChanged_Settings_AutoHide, FuncDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_7(         mspSettings,                                                                                            // ДИСПЛЕЙ - НАСТРОЙКИ ///
    "НАСТРОЙКИ", "SETTINGS",
    "Дополнительные настройки дисплея",
    "Additional display settings",
    mspSettings_Colors,              // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА
    mgSettings_Brightness,           // ДИСПЛЕЙ - НАСТРОЙКИ - Яркость
    mgSettings_Levels,               // ДИСПЛЕЙ - НАСТРОЙКИ - Уровни
    mgSettings_TimeMessages,         // ДИСПЛЕЙ - НАСТРОЙКИ - Время
    mcSettings_ShowStringNavigation, // ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню
    mcSettings_ShowAltMarkers,       // ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры
    mcSettings_AutoHide,             // ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать
    Page_Service_Display, &pDisplay, FuncActive, EmptyPressPage
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_9(         pDisplay,                                                                                                           // ДИСПЛЕЙ ///
    "ДИСПЛЕЙ", "DISPLAY",
    "Содержит настройки отображения дисплея.",
    "Contains settings of display of the Display::",
    mcMapping,       // ДИСПЛЕЙ - Отображение
    mspAccumulation, // ДИСПЛЕЙ - НАКОПЛЕНИЕ
    mspAveraging,    // ДИСПЛЕЙ - УСРЕДНЕНИЕ
    mcMinMax,        // ДИСПЛЕЙ - Мин Макс
    mcSmoothing,     // ДИСПЛЕЙ - Сглаживание
    mcRefreshFPS,    // ДИСПЛЕЙ - Частота обновл
    mspGrid,         // ДИСПЛЕЙ - СЕТКА
    mcTypeShift,     // ДИСПЛЕЙ - Смещение
    mspSettings,     // ДИСПЛЕЙ - НАСТРОЙКИ
    Page_Display, &mainPage, FuncActive, EmptyPressPage
);


/** @}  @}
 */
