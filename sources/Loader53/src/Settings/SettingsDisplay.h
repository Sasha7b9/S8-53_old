#pragma once


/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsDisplay
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define COLOR(x)                    (set.display.colors[x])

#define TIME_SHOW_LEVELS            (set.display.timeShowLevels)                ///< SettingsDisplay.timeShowLevels

#define SHIFT_IN_MEMORY             (set.display.shiftInMemory)                 ///< SettingsDisplay.shiftInMemory

#define TIME_MESSAGES               (set.display.timeMessages)                  ///< SettingsDisplay.timeMessages

#define BRIGHTNESS                  (set.display.brightness)                    ///< SettingsDisplay.brightness

#define BRIGHTNESS_GRID             (set.display.brightnessGrid)                ///< SettingsDisplay.brightnessGrid

#define DISPLAY_COLOR(num)          (set.display.colors[num])                   ///< SettingsDisplay.colors

#define MODE_DRAW_SIGNAL            (set.display.modeDrawSignal)                ///< SettingsDisplay.modeDrawSignal
#define MODE_DRAW_IS_SIGNAL_LINES   (MODE_DRAW_SIGNAL == ModeDrawSignal_Lines)  ///< \c true, если сигнал выводится линиями.

#define TYPE_GRID                   (set.display.typeGrid)                      ///< SettingsDisplay.typeGrid
#define TYPE_GRID_IS_1              (TYPE_GRID == TypeGrid_1)                   ///< Установлен тип сетки TypeGrid_1.
#define TYPE_GRID_IS_2              (TYPE_GRID == TypeGrid_2)                   ///< Установлен тип сетки TypeGrid_2.
#define TYPE_GRID_IS_3              (TYPE_GRID == TypeGrid_3)                   ///< Установлен тип сетки TypeGrid_3.

#define ENUM_ACCUM                  (set.display.enumAccumulation)              ///< SettingsDisplay.numAccumulation
#define ENUM_ACCUM_IS_NONE          (ENUM_ACCUM == NumAccumulation_1)           ///< \c true, если накопление выключено.
#define ENUM_ACCUM_IS_INFINITY      (ENUM_ACCUM == NumAccumulation_Infinity)    ///< \c true, если установлено бесконечное число накоплений.
#define NUM_ACCUM                   (1 << (int)set.display.enumAccumulation)    ///< Количество накоплений.

#define ENUM_AVE                    (set.display.enumAve)                       ///< SettingsDisplay.enumAve
#define NUM_AVE                     (1 << ENUM_AVE)                             ///< Количество усреднений.

#define MODE_AVE                    (set.display.modeAve)                       ///< SettingsDisplay.modeAve

#define ENUM_MIN_MAX                (set.display.enumMinMax)                    ///< SettingsDisplay.enumMinMax
#define NUM_MIN_MAX                 (1 << ENUM_MIN_MAX)                         ///< Количество измерений сигналов для расчёта минимумов и максимумов.

#define SMOOTHING                   (set.display.smoothing)                     ///< SettingsDisplay.smoothing
#define SMOOTHING_IS_DISABLE        (SMOOTHING == Smoothing_Disable)            ///< \c true, если скользящий фильтр выключен.

#define ENUM_SIGNALS_IN_SEC         (set.display.enumSignalsInSec)              ///< SettingsDisplay.enumSignalsInSec

#define LAST_AFFECTED_CHANNEL       (set.display.lastAffectedChannel)           ///< SettingsDisplay.lastAffectedChannel
#define LAST_AFFECTED_CHANNEL_IS_A  (LAST_AFFECTED_CHANNEL == A)                ///< \c true, если последний канал, которым управляли - 1.
#define LAST_AFFECTED_CHANNEL_IS_B  (LAST_AFFECTED_CHANNEL == B)                ///< \c true, если последний канал, которым управляли - 2.

#define MODE_ACCUM                  (set.display.modeAccumulation)              ///< SettingsDisplay.modeAccumulation
#define MODE_ACCUM_IS_RESET         (MODE_ACCUM == ModeAccumulation_Reset)      ///< \c true, если накопление со сбросом.
#define MODE_ACCUM_IS_NORESET       (MODE_ACCUM == ModeAccumulation_NoReset)    ///< \c true, если накопление без сброса.

#define ALT_MARKERS                 (set.display.altMarkers)                    ///< SettingsDisplay.altMarkers
#define ALT_MARKERS_HIDE            (ALT_MARKERS == AM_Hide)                    ///< \c true, если дополнительные маркеры никогда не надо показывать.

#define MENU_AUTO_HIDE              (set.display.menuAutoHide)                  ///< SettingsDisplay.menuAutoHide

#define SHOW_STRING_NAVIGATION      (set.display.showStringNavigation)          ///< SettingsDisplay.showStringNavigation

#define LINKING_RSHIFT              (set.display.linkingRShift)                 ///< SettingsDisplay.linkingRShift
#define LINKING_RSHIFT_IS_VOLTAGE   (LINKING_RSHIFT == LinkingRShift_Voltage)   ///< \c true, если привязка к абсолютному напряжению.


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /// Режим отрисовки сигнала.
typedef enum
{
    ModeDrawSignal_Lines = 0,   ///< Сигнал рисуется линиями.
    ModeDrawSignal_Points = 1   ///< Сигнал рисуется точками.
} ModeDrawSignal;

/// Тип сетки на экране.
typedef enum
{
    TypeGrid_1,
    TypeGrid_2,
    TypeGrid_3,
    TypeGrid_4,
    TG_Size
} TypeGrid;

/// Перечисление накоплений.
typedef enum
{
    ENumAccum_1,
    ENumAccum_2,
    ENumAccum_4,
    ENumAccum_8,
    ENumAccum_16,
    ENumAccum_32,
    ENumAccum_64,
    ENumAccum_128,
    ENumAccum_Infinity
} ENumAccum;

/// Количество усреднений по измерениям.
typedef enum
{
    NumAveraging_1,
    NumAveraging_2,
    NumAveraging_4,
    NumAveraging_8,
    NumAveraging_16,
    NumAveraging_32,
    NumAveraging_64,
    NumAveraging_128,
    NumAveraging_256,
    NumAveraging_512
} ENumAveraging;

/// Тип усреднений по измерениям.
typedef enum
{
    Averaging_Accurately,   ///< Усреднять точно.
    Averaging_Around        ///< Усреднять приблизительно.
} ModeAveraging;

/// Количество измерений для расчёта минимального и максимального значений.
typedef enum
{
    NumMinMax_1,
    NumMinMax_2,
    NumMinMax_4,
    NumMinMax_8,
    NumMinMax_16,
    NumMinMax_32,
    NumMinMax_64,
    NumMinMax_128
} ENumMinMax;

/// Количество точек для расчёта скользящего фильтра.
typedef enum
{
    Smoothing_Disable,
    Smoothing_2points = 1,
    Smoothing_3points = 2,
    Smoothing_4points = 3,
    Smoothing_5points = 4,
    Smoothing_6points = 5,
    Smoothing_7points = 6,
    Smoothing_8points = 7,
    Smoothing_9points = 8,
    Smoothing_10points = 9
} Smoothing;

/// Ограничение FPS.
typedef enum
{
    NumSignalsInSec_25,
    NumSignalsInSec_10,
    NumSignalsInSec_5,
    NumSignalsInSec_2,
    NumSignalsInSec_1
} ENumSignalsInSec;

/// Режим накопления.
typedef enum
{
    ModeAccumulation_NoReset,   ///< В этом режиме показываются строго N последних измерений.
    ModeAccumulation_Reset      ///< В этом режиме набираются N последних измерений и потом сбрасываются.
} ModeAccumulation;

/// Режим отображения дополнительных боковых маркеров смещения по напряжению.
typedef enum
{
    AM_Hide,        ///< Никода не выводить.
    AM_Show,        ///< Всегда выводить.
    AM_AutoHide     ///< Выводить и прятать через timeShowLevels.
} AltMarkers;

/// Через какое время после последнего нажатия кнопки скрывать меню.
typedef enum
{
    MenuAutoHide_None = 0,  ///< Никогда.
    MenuAutoHide_5 = 5,     ///< Через 5 секунд.
    MenuAutoHide_10 = 10,   ///< Через 10 секунд.
    MenuAutoHide_15 = 15,   ///< Через 15 секунд.
    MenuAutoHide_30 = 30,   ///< Через 30 секунд.
    MenuAutoHide_60 = 60    ///< Через 60 секунд.
} MenuAutoHide;

/// Тип привязки к смещению по вертикали
typedef enum
{
    LinkingRShift_Voltage,      ///< Смещение привязано к напряжению
    LinkingRShift_Position      ///< Смещение привязаоно к позиции на экране
} LinkingRShift;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Возвращает ограничение частоты кадров.
int sDisplay_NumSignalsInS();
/// Устанавливает ограничение частоты кадров.
void sDisplay_SetNumSignalsInS(int maxFPS);
/// Возвращает число точек сглаживающего фильтра (режим ДИСПЛЕЙ - Сглаживание).
int sDisplay_NumPointSmoothing();
/// Возвращает адрес первой и последней точки на экране.
void sDisplay_PointsOnDisplay(int *firstPoint, int *lastPoint);
/// Возвращает время, через которое меню автоматически скрывается, если не было больше нажатий.
int sDisplay_TimeMenuAutoHide();
/// Если экран разделён на две части и основной сигнал выводится сверху - например, в режиме вывода спектра
bool sDisplay_IsSeparate();
/// brightness = 1..100
void sDisplay_SetBrightness(int16 brightness);
/// Возвращает режим усреднения
ModeAveraging sDisplay_ModeAveraging();

int sDisplay_NumAverage();


/** @}  @}
 */
