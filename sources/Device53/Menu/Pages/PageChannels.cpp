#include "defines.h"
#include "Log.h"
#include "Settings/SettingsTypes.h"
#include "Menu/MenuItems.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Utils/Dictionary.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageChannels
 *  @{
 */

extern const PageBase mainPage;
extern const PageBase pChanA;
extern const PageBase pChanB;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const char chanInputRu[] =   "1. \"Вкл\" - выводить сигнал на экран.\n"
                                    "2. \"Откл\" - не выводить сигнал на экран.";
extern const char chanInputEn[] =   "1. \"Enable\" - signal output to the screen.\n"
                                    "2. \"Disable\" - no output to the screen.";

extern const char chanCoupleRu[] =  "Задаёт вид связи с источником сигнала.\n"
                                    "1. \"Пост\" - открытый вход.\n"
                                    "2. \"Перем\" - закрытый вход.\n"
                                    "3. \"Земля\" - вход соединён с землёй.";
extern const char chanCoupleEn[] =  "Sets a type of communication with a signal source.\n"
                                    "1. \"AC\" - open input.\n"
                                    "2. \"DC\" - closed input.\n"
                                    "3. \"Ground\" - input is connected to the ground.";

extern const char chanFiltrRu[] = "Включает/отключает фильтр для ограничения полосы пропускания.";
extern const char chanFiltrEn[] = "Includes/switches-off the filter for restriction of a pass-band.";

extern const char chanInverseRu[] = "При \"Вкл\" сигнал на экране будет симметрично отражён относительно U = 0В.";
extern const char chanInverseEn[] = "When \"Enable\" signal on the screen will be reflected symmetrically with respect to U = 0V.";

extern const char chanMultiplierRu[] = "Ослабление сигнала:\n\"x1\" - сигнал не ослабляется.\n\"x10\" - сигнал ослабляется в 10 раз";
extern const char chanMultiplierEn[] = "Attenuation: \n\"x1\" - the signal is not attenuated.\n\"x10\" - the signal is attenuated by 10 times";


//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_InputA(bool active)
{
    Panel::EnableLEDChannelA(sChannel_Enabled(A));
}

DEF_CHOICE_2(       mcInputA,                                                                                                 //--- КАНАЛ 1 - Вход ---
    "Вход", "Input",
    chanInputRu,
    chanInputEn,
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    SET_ENABLED_A, pChanA, FuncActive, OnChanged_InputA, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_CoupleA(bool active)
{
    FPGA::SetModeCouple(A, SET_COUPLE_A);
}

DEF_CHOICE_3(       mcCoupleA,                                                                                               //--- КАНАЛ 1 - Связь ---
    "Связь", "Couple",
    chanCoupleRu,
    chanCoupleEn,
    "Пост",  "AC",
    "Перем", "DC",
    "Земля", "Ground",
    SET_COUPLE_A, pChanA, FuncActive, OnChanged_CoupleA, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_FiltrA(bool active)
{
    FPGA::EnableChannelFiltr(A, FILTR_A);
}

DEF_CHOICE_2(       mcFiltrA,                                                                                               //--- КАНАЛ 1 - Фильтр ---
    "Фильтр", "Filtr",
    chanFiltrRu,
    chanFiltrEn,
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    FILTR_A, pChanA, FuncActive, OnChanged_FiltrA, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_InverseA(bool active)
{
    FPGA::SetRShift(A, SET_RSHIFT_A);
}

DEF_CHOICE_2(       mcInverseA,                                                                                           //--- КАНАЛ 1 - Инверсия ---
    "Инверсия", "Inverse",
    chanInverseRu,
    chanInverseEn,
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    SET_INVERSE_A, pChanA, FuncActive, OnChanged_InverseA, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMultiplierA,                                                                                       //--- КАНАЛ 1 - Множитель ---
    "Множитель", "Divider",
    chanMultiplierRu,
    chanMultiplierEn,
    "х1",  "x1",
    "x10", "x10",
    SET_DIVIDER(A), pChanA, FuncActive, FuncChangedChoice, FuncDraw
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_5(         pChanA,                                                                                                             // КАНАЛ 1 ///
    "КАНАЛ 1", "CHANNEL 1",
    "Содержит настройки канала 1.",
    "Contains settings of the channel 1.",
    mcInputA,       // КАНАЛ 1 - Вход
    mcCoupleA,      // КАНАЛ 1 - Связь
    mcFiltrA,       // КАНАЛ 1 - Фильтр
    mcInverseA,     // КАНАЛ 1 - Инверсия
    mcMultiplierA,  // КАНАЛ 1 - Множитель
    Page_ChannelA, &mainPage, FuncActive, EmptyPressPage
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_InputB(bool active)
{
    Panel::EnableLEDChannelB(sChannel_Enabled(B));
}

DEF_CHOICE_2(       mcInputB,                                                                                                 //--- КАНАЛ 2 - Вход ---
    "Вход", "Input",
    chanInputRu,
    chanInputEn,
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    SET_ENABLED_B, pChanB, FuncActive, OnChanged_InputB, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_CoupleB(bool active)
{
    FPGA::SetModeCouple(B, SET_COUPLE_B);
}

DEF_CHOICE_3(       mcCoupleB,                                                                                               //--- КАНАЛ 2 - Связь ---
    "Связь", "Couple",
    chanCoupleRu,
    chanCoupleEn,
    "Пост",  "AC",
    "Перем", "DC",
    "Земля", "Ground",
    SET_COUPLE_B, pChanB, FuncActive, OnChanged_CoupleB, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChanged_FiltrB(bool active)
{
    FPGA::EnableChannelFiltr(B, FILTR_B);
}

DEF_CHOICE_2(       mcFiltrB,                                                                                               //--- КАНАЛ 2 - Фильтр ---
    "Фильтр", "Filtr",
    chanFiltrRu,
    chanFiltrEn,
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    FILTR_B, pChanB, FuncActive, OnChanged_FiltrB, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_InverseB(bool active)
{
    FPGA::SetRShift(B, SET_RSHIFT_B);
}

DEF_CHOICE_2(       mcInverseB,                                                                                           //--- КАНАЛ 2 - Инверсия ---
    "Инверсия", "Inverse",
    chanInverseRu,
    chanInverseEn,
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    SET_INVERSE_B, pChanB, FuncActive, OnChanged_InverseB, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMultiplierB,                                                                                       //--- КАНАЛ 2 - Множитель ---
    "Множитель", "Divider",
    chanMultiplierRu,
    chanMultiplierEn,
    "х1",  "x1",
    "x10", "x10",
    SET_DIVIDER(B), pChanB, FuncActive, FuncChangedChoice, FuncDraw
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_5(         pChanB,                                                                                                             // КАНАЛ 2 ///
    "КАНАЛ 2", "CHANNEL 2",
    "Содержит настройки канала 2.",
    "Contains settings of the channel 2.",
    mcInputB,       // КАНАЛ 2 - Вход
    mcCoupleB,      // КАНАЛ 2 - Связь
    mcFiltrB,       // КАНАЛ 2 - Фильтр
    mcInverseB,     // КАНАЛ 2 - Инверсия
    mcMultiplierB,  // КАНАЛ 2 - Множитель
    Page_ChannelB, &mainPage, FuncActive, EmptyPressPage
);


/** @}  @}
 */
