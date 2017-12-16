#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "PageMemory.h"
#include "Menu/Pages/Definition.h"
#include "FlashDrive/FileManager.h"
#include "FPGA/FPGA.h"
#include "FPGA/DataStorage.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/StringUtils.h"
#include "Utils/CommonFunctions.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/FLASH.h"
#include "Hardware/Sound.h"
#include "Log.h"
#include "Tables.h"
#include "Utils/Dictionary.h"
#include <string.h>
#include <stdio.h>
#include "stub.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageMemory
 *  @{
 */


extern const PageBase pMemory;
extern const PageBase mainPage;
extern const PageBase mspMemLast;
extern const PageBase mspMemInt;
extern const PageBase mpSetName;
extern const PageBase mspSetMask;
extern const PageBase mspFileManager;
extern const PageBase mpSetName;
extern const PageBase mspMemoryExt;

extern void PressSB_SetName_Exit();

static void DrawSetMask();  // Эта функция рисует, когда выбран режим задания маски.
static void DrawSetName();  // Эта функция рисует, когда нужно задать имя файла для сохранения


void ChangeC_Memory_NumPoints(bool active)
{
    if(sMemory_GetNumPoints(false) == 281)
    {
        SHIFT_IN_MEMORY = 0;
    }
    else
    {
        if(TPOS_IS_LEFT)
        {
            SHIFT_IN_MEMORY = 0;
        }
        else if(TPOS_IS_CENTER)
        {
            SHIFT_IN_MEMORY = sMemory_GetNumPoints(false) / 2 - Grid::Width() / 2;
        }
        else if(TPOS_IS_RIGHT)
        {
            SHIFT_IN_MEMORY = sMemory_GetNumPoints(false) - Grid::Width() - 2;
        }
    }
    FPGA::SetTShift(TSHIFT);
}

// Активна ли ПАМЯТЬ - ВНЕШН ЗУ - Маска
bool IsActiveMemoryExtSetMask()
{                       
    return FILE_NAMING_MODE_IS_MASK;
}


void DrawSB_MemLastSelect(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 3, y + 2, set.memory.strMemoryLast.isActiveModeSelect ? '\x2a' : '\x28');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_MemLast_Prev(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x20');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_MemLast_Next(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x64');
    Painter::SetFont(TypeFont_8);
}

void PressSB_MemLastSelect()
{
    set.memory.strMemoryLast.isActiveModeSelect = !set.memory.strMemoryLast.isActiveModeSelect;
}

void PressSB_MemLast_Next()
{
    CircleIncrease<int16>(&gMemory.currentNumLatestSignal, 0, Storage::AllDatas() - 1);
}

void PressSB_MemLast_Prev()
{
    CircleDecrease<int16>(&gMemory.currentNumLatestSignal, 0, Storage::AllDatas() - 1);
}

static void RotateSB_MemLast(int angle)
{
    if (Storage::AllDatas() > 1)
    {
        Sound::RegulatorSwitchRotate();
    }
    if (Sign(angle) > 0)
    {
        PressSB_MemLast_Next();
    }
    else
    {
        PressSB_MemLast_Prev();
    }
}

static void DrawSB_MemExtSetNameSave(int x, int y)
{
    if (gBF.flashDriveIsConnected == 1)
    {
        Painter::SetFont(TypeFont_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter::SetFont(TypeFont_8);
    }
}

static void PressSB_MemExtSetNameSave()
{
    if (gBF.flashDriveIsConnected == 1)
    {
        PressSB_SetName_Exit();
        gMemory.needForSaveToFlashDrive = 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON(   sbMemLastPrev,                                                                           //--- ПАМЯТЬ - ПОСЛЕДНИЕ - Предыдущий ---
    "Предыдущий", "Previous",
    "Перейти к предыдущему сигналу",
    "Go to the previous signal",
    mspMemLast, FuncActive, PressSB_MemLast_Prev, DrawSB_MemLast_Prev

);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON(   sbMemLastNext,                                                                            //--- ПАМЯТЬ - ПОСЛЕДНИЕ - Следующий ---
    "Следующий", "Next",
    "Перейти к следующему сигналу",
    "Go to the next signal",
    mspMemLast, FuncActive, PressSB_MemLast_Next, DrawSB_MemLast_Next
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void PressSB_MemLast_IntEnter()
{
    Menu::OpenPageAndSetItCurrent(PageSB_Memory_Internal);
    MODE_WORK = ModeWork_ROM;
    FLASHMem::GetData(gMemory.currentNumIntSignal, &gDSmemInt, &gData0memInt, &gData1memInt);
    gMemory.exitFromIntToLast = 1;
}

void DrawSB_MemLast_IntEnter(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x40');
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbMemLastIntEnter,                                                                         //--- ПАМЯТЬ - ПОСЛЕДНИЕ - Внутр ЗУ ---
    "Внутр ЗУ", "Internal storage",
    "Нажмите эту кнопку, чтобы сохранить сигнал во внутреннем запоминающем устройстве",
    "Press this button to keep a signal in an internal memory",
    mspMemLast, FuncActive, PressSB_MemLast_IntEnter, DrawSB_MemLast_IntEnter
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_MemLast_SaveToFlash()
{
    gMemory.exitFromModeSetNameTo = RETURN_TO_LAST_MEM;
    Memory_SaveSignalToFlashDrive();
}

void DrawSB_MemLast_SaveToFlash(int x, int y)
{
    if (gBF.flashDriveIsConnected == 1)
    {
        Painter::SetFont(TypeFont_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter::SetFont(TypeFont_8);
    }
}

DEF_SMALL_BUTTON(   sbMemLastSaveToFlash,                                                                     //--- ПАМЯТЬ - ПОСЛЕДНИЕ - Сохранить ---
    "Сохранить", "Save",
    "Кнопка становится доступна при присоединённом внешнем ЗУ. Позволяет сохранить сигнал на внешем ЗУ",
    "Click this button to save the signal on the external FLASH",
    mspMemLast, FuncActive, PressSB_MemLast_SaveToFlash, DrawSB_MemLast_SaveToFlash
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON(   sbSetNameSave,                                                                                      //--- ИМЯ ФАЙЛА - Сохранить ---
    "Сохранить", "Save",
    "Сохранение на флеш под заданным именем",
    "Saving to flashdrive with the specified name",
    mpSetName, FuncActive, PressSB_MemExtSetNameSave, DrawSB_MemExtSetNameSave
);

void OnMemExtSetMaskNameRegSet(int angle, int maxIndex)
{   
    static const pFuncVpI8I8I8 func[3] =
    {
        CircleDecrease<int8>,
        CircleDecrease<int8>,
        CircleIncrease<int8>
    };

    Painter::ResetFlash();
    if (INDEX_SYMBOL > maxIndex)
    {
        INDEX_SYMBOL = maxIndex - 1;
    }
    func[Sign(angle) + 1]((int8 *)&INDEX_SYMBOL, 0, maxIndex - 1);
    Sound::RegulatorSwitchRotate();

}

static void OnMemExtSetMaskRegSet(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(symbolsAlphaBet) / 4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void PressSB_SetMask_Backspace()
{
    int size = strlen(FILE_NAME_MASK);
    if (size > 0)
    {
        if (size > 1 && FILE_NAME_MASK[size - 2] == 0x07)
        {
            FILE_NAME_MASK[size - 2] = '\0';
        }
        else
        {
            FILE_NAME_MASK[size - 1] = '\0';
        }
    }
}

void DrawSB_SetMask_Backspace(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbSetMaskBackspace,                                                                                    //--- МАСКА - Backspace ---
    "Backspace", "Backspace",
    "Удаляет последний введённый символ",
    "Deletes the last entered symbol",
    mspSetMask, FuncActive, PressSB_SetMask_Backspace, DrawSB_SetMask_Backspace
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_SetMask_Delete()
{
    FILE_NAME_MASK[0] = '\0';
}

static void DrawSB_SetMask_Delete(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbSetMaskDelete,                                                                                         //--- МАСКА - Удалить ---
    "Удалить", "Delete",
    "Удаляет все введённые символы",
    "Deletes all entered symbols",
    mspSetMask, FuncActive, PressSB_SetMask_Delete, DrawSB_SetMask_Delete
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_SetMask_Insert()
{
    int index = INDEX_SYMBOL;
    int size = strlen(FILE_NAME_MASK);
    if (size == MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        return;
    }
    if (index < 0x41)
    {
        FILE_NAME_MASK[size] = symbolsAlphaBet[index][0];
        FILE_NAME_MASK[size + 1] = '\0';
    }
    else
    {
        index -= 0x40;
        if (index == 0x07)  // Для %nN - отдельный случай
        {
            if (size < MAX_SYMBOLS_IN_FILE_NAME - 2 && size > 0)
            {
                if (FILE_NAME_MASK[size - 1] >= 0x30 && FILE_NAME_MASK[size - 1] <= 0x39) // Если ранее введено число
                {
                    FILE_NAME_MASK[size] = FILE_NAME_MASK[size - 1] - 0x30;
                    FILE_NAME_MASK[size - 1] = 0x07;
                    FILE_NAME_MASK[size + 1] = '\0';
                }
            }
        }
        else
        {
            FILE_NAME_MASK[size] = index;
            FILE_NAME_MASK[size + 1] = '\0';
        }
    }
}

static void DrawSB_SetMask_Insert(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_INSERT);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbSetMaskInsert,                                                                                        //--- МАСКА - Вставить ---
    "Вставить", "Insert",
    "Вставляет выбранный символ",
    "Inserts the chosen symbol",
    mspSetMask, FuncActive, PressSB_SetMask_Insert, DrawSB_SetMask_Insert
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_SetName_Backspace()
{
    int size = strlen(FILE_NAME);
    if (size > 0)
    {
        FILE_NAME[size - 1] = '\0';
    }
}

static void DrawSB_SetName_Backspace(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbSetNameBackspace,                                                                                //--- ИМЯ ФАЙЛА - Backspace ---
    "Backspace", "Backspace",
    "Удаляет последний символ",
    "Delete the last character",
    mpSetName, FuncActive, PressSB_SetName_Backspace, DrawSB_SetName_Backspace
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_SetName_Delete()
{
    FILE_NAME[0] = '\0';
}

static void DrawSB_SetName_Delete(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbSetNameDelete,                                                                                     //--- ИМЯ ФАЙЛА - Удалить ---
    "Удалить", "Delete",
    "Удаляет все введённые символы",
    "Deletes all entered characters",
    mpSetName, FuncActive, PressSB_SetName_Delete, DrawSB_SetName_Delete
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_SetName_Insert()
{
    int size = strlen(FILE_NAME);
    if (size < MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        FILE_NAME[size] = symbolsAlphaBet[INDEX_SYMBOL][0];
        FILE_NAME[size + 1] = '\0';
    }
}

static void DrawSB_SetName_Insert(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x26');
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbSetNameInsert,                                                                                    //--- ИМЯ ФАЙЛА - Вставить ---
    "Вставить", "Insert",
    "Вводит очередной символ",
    "Print the next character",
    mpSetName, FuncActive, PressSB_SetName_Insert, DrawSB_SetName_Insert
);

void DrawSB_MemExtNewFolder(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 1, y, '\x46');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_FM_LevelDown(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_FM_LevelUp(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    Painter::SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSB_FM_Tab(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x6e');
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbFileManagerTab,                                                                                          //--- КАТАЛОГ - Tab ---
    "Tab", "Tab",
    "Переход между каталогами и файлами",
    "The transition between the directories and files",
    mspFileManager, FuncActive, FileManager::PressSB_Tab, DrawSB_FM_Tab
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON(   sbFileManagerLevelDown,                                                                                  //--- КАТАЛОГ - Войти ---
    "Войти", "Enter",
    "Переход в выбранный каталог",
    "Transition to the chosen catalog",
    mspFileManager, FuncActive, FileManager::PressSB_LevelDown, DrawSB_FM_LevelDown
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_SMALL_BUTTON(   sbFileManagerLevelUp,                                                                                   //--- КАТАЛОГ - Выйыти ---
    "Выйти", "Leave",
    "Переход в родительский каталог",
    "Transition to the parental catalog",
    mspFileManager, FuncActive, FileManager::PressSB_LevelUp, DrawSB_FM_LevelUp
);

static void SaveSignalToIntMemory()
{
    if (gMemory.exitFromIntToLast == 1)
    {
        if  (gDSmemLast != 0)
        {
            FLASHMem::SaveData(gMemory.currentNumIntSignal, gDSmemLast, gData0memLast, gData1memLast);
            FLASHMem::GetData(gMemory.currentNumIntSignal, &gDSmemInt, &gData0memInt, &gData1memInt);
            Display::ShowWarningGood(SignalIsSaved);
        }
    }
    else
    {
        if (gDSet != 0)
        {
            FLASHMem::SaveData(gMemory.currentNumIntSignal, gDSet, gData0, gData1);
            FLASHMem::GetData(gMemory.currentNumIntSignal, &gDSet, &gData0memInt, &gData1memInt);
            Display::ShowWarningGood(SignalIsSaved);
        }
    }
}

static void DrawMemoryWave(int num, bool exist)
{
    char buffer[20];
    
    int x = Grid::Left() + 2 + num * 12;
    int y = Grid::FullBottom() - 10;
    int width = 12;
    Painter::FillRegion(x, y, width, 10, num == gMemory.currentNumIntSignal ? Color::FLASH_10 : Color::BACK);
    Painter::DrawRectangle(x, y, width, 10, Color::FILL);
    Painter::SetColor(num == gMemory.currentNumIntSignal ? Color::FLASH_01 : Color::FILL);
    if (exist)
    {
        Painter::DrawText(x + 2, y + 1, Int2String(num + 1, false, 2, buffer));
    }
    else
    {
        Painter::DrawText(x + 3, y + 1, "\x88");
    }
}

static void FuncAdditionDrawingSPageMemoryInt()
{
    // Теперь нарисуем состояние памяти

    bool exist[MAX_NUM_SAVED_WAVES] = {false};

    FLASHMem::GetDataInfo(exist);

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        DrawMemoryWave(i, exist[i]);
    }
}

static void FuncOnRegSetMemInt(int delta)
{
    Sound::RegulatorSwitchRotate();
    if (delta < 0)
    {
        CircleDecrease<int8>(&gMemory.currentNumIntSignal, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    else if (delta > 0)
    {
        CircleIncrease<int8>(&gMemory.currentNumIntSignal, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    FLASHMem::GetData(gMemory.currentNumIntSignal, &gDSmemInt, &gData0memInt, &gData1memInt);
    Painter::ResetFlash();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_MemInt_ShowSignalAlways()
{
    gMemory.showAlways = (gMemory.showAlways == 0) ? 1 : 0;
}

static void DrawSB_MemInt_ShowSignalAllways_Yes(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x66');
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ShowSignalAllways_No(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x68');
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ShowSignalAlways(int x, int y)
{
    if (gMemory.showAlways == 0)
    {
        DrawSB_MemInt_ShowSignalAllways_No(x, y);
    }
    else
    {
        DrawSB_MemInt_ShowSignalAllways_Yes(x, y);
    }
}

DEF_SMALL_BUTTON_HINTS_2(   sbMemIntShowSignalAlways,                                                  //--- ПАМЯТЬ - ВНУТР ЗУ - Показывать всегда ---
    "Показывать всегда", "To show always",
    "Позволяет всегда показывать выбранный сохранённый сигнал поверх текущего",
    "Allows to show always the chosen kept signal over the current",
    mspMemInt, FuncActive, PressSB_MemInt_ShowSignalAlways, DrawSB_MemInt_ShowSignalAlways,
    DrawSB_MemInt_ShowSignalAllways_Yes, "показывать выбранный сигнал из внутренней памяти поверх текущего",
                                         "to show the chosen signal from internal memory over the current",
    DrawSB_MemInt_ShowSignalAllways_No,  "сигнал из внутренней памяти виден только в режиме работы с внутренним запоминающим устройством",
                                         "the signal from internal memory is visible only in an operating mode with an internal memory"
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSB_MemInt_ModeShow_Direct(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_MEM_INT_SHOW_DIRECT);
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ModeShow_Saved(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_MEM_INT_SHOW_SAVED);
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ModeShow_Both(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_MEM_INT_SHOW_BOTH);
    Painter::SetFont(TypeFont_8);
}

static void PressSB_MemInt_ModeShow()
{
    CircleIncrease<int8>((int8 *)&MODE_SHOW_MEMINT, 0, 2);
}

static void DrawSB_MemInt_ModeShow(int x, int y)
{
    ModeShowIntMem mode = MODE_SHOW_MEMINT;

    if (mode == ModeShowIntMem_Direct)
    {
        DrawSB_MemInt_ModeShow_Direct(x, y);
    }
    else if (mode == ModeShowIntMem_Saved)
    {
        DrawSB_MemInt_ModeShow_Saved(x, y);
    }
    else if (mode == ModeShowIntMem_Both)
    {
        DrawSB_MemInt_ModeShow_Both(x, y);
    }
}

DEF_SMALL_BUTTON_HINTS_3(   sbMemIntModeShow,                                                                //--- ПАМЯТЬ - ВНУТР ЗУ - Вид сигнала ---
    "Вид сигнала", "Type of a signal",
    "Показывать записанный или текущий сигнал в режиме ВНУТР ЗУ",
    "Show recorded or current signal in mode Internal Memory",
    mspMemInt, FuncActive, PressSB_MemInt_ModeShow, DrawSB_MemInt_ModeShow,
    DrawSB_MemInt_ModeShow_Direct, "на дисплее текущий сигнал",     "on the display current signal",
    DrawSB_MemInt_ModeShow_Saved,  "на дисплее сохранённый сигнал", "on the display the kept signal",
    DrawSB_MemInt_ModeShow_Both,   "на дисплее оба сигнала",        "on the display the both signals"
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void PressSB_MemInt_SaveToIntMemory()
{
    SaveSignalToIntMemory();
}

void DrawSB_MemInt_SaveToIntMemory(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter::SetFont(TypeFont_8);
}

DEF_SMALL_BUTTON(   sbMemIntSave,                                                                              //--- ПАМЯТЬ - ВНУТР ЗУ - Сохранить ---
    "Сохранить", "Save",
    "Сохранить сигнал во внутреннем запоминующем устройстве",
    "To keep a signal in an internal memory",
    mspMemInt, FuncActive, PressSB_MemInt_SaveToIntMemory, DrawSB_MemInt_SaveToIntMemory
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void PressSB_MemInt_SaveToFlashDrive()
{
    gMemory.exitFromModeSetNameTo = RETURN_TO_INT_MEM;
    Memory_SaveSignalToFlashDrive();
}

void DrawSB_MemInt_SaveToFlashDrive(int x, int y)
{
    if (gBF.flashDriveIsConnected == 1)
    {
        Painter::SetFont(TypeFont_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter::SetFont(TypeFont_8);
    }
}

DEF_SMALL_BUTTON(   sbMemIntSaveToFlash,                                                                      //--- ПАМЯТЬ - ВНУТР ЗУ - Сохранить ---
    "Сохранить", "Save",
    "Сохраняет сигнал на флешку",
    "Save signal to flash drive",
    mspMemInt, FuncActive, PressSB_MemInt_SaveToFlashDrive, DrawSB_MemInt_SaveToFlashDrive
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_SetName_Exit()
{
    Display::RemoveAddDrawFunction();
    if (gMemory.exitFromModeSetNameTo == RETURN_TO_DISABLE_MENU)
    {
        Menu::ShortPressOnPageItem(Menu::PagePointerFromName(PageSB_Memory_SetName), 0);
    }
    else if (gMemory.exitFromModeSetNameTo == RETURN_TO_LAST_MEM)
    {
        Menu::OpenPageAndSetItCurrent(PageSB_Memory_Last);
    }
    else if (gMemory.exitFromModeSetNameTo == RETURN_TO_INT_MEM)
    {
        Menu::OpenPageAndSetItCurrent(PageSB_Memory_Internal);
    }
    gMemory.exitFromModeSetNameTo = RETURN_TO_DISABLE_MENU;
}

DEF_SMALL_BUTTON(   sbExitSetName,                                                                                         //--- ИМЯ ФАЙЛА - Выход ---
    "Выход", "Exit",
    "Отказ от сохранения",
    "Failure to save",
    mpSetName, FuncActive, PressSB_SetName_Exit, DrawSB_Exit
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void PressSB_MemInt_Exit()
{
    FLASHMem::GetData(gMemory.currentNumIntSignal, &gDSmemInt, &gData0memInt, &gData1memInt);
    if (gMemory.exitFromIntToLast == 1)
    {
        Menu::OpenPageAndSetItCurrent(PageSB_Memory_Last);
        MODE_WORK = ModeWork_RAM;
        gMemory.exitFromIntToLast = 0;
    }
    else
    {
        Menu::ShortPressOnPageItem(Menu::PagePointerFromName(PageSB_Memory_Internal), 0);
    }
}

DEF_SMALL_BUTTON_EXIT(  sbExitMemInt,                                                                              //--- ПАМЯТЬ - ВНУТР ЗУ - Выход ---
    mspMemInt, FuncActive, PressSB_MemInt_Exit, DrawSB_Exit
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnPressMemoryExtMask()
{
    Menu::OpenPageAndSetItCurrent(PageSB_Memory_Drive_Mask);
    Display::SetAddDrawFunction(DrawSetMask);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool FuncActiveMemoryNumPoinst()
{
    return PEAKDET_IS_DISABLE;
}

DEF_CHOICE_3(       mcMemoryNumPoints,                                                                                        //--- ПАМЯТЬ - Точки ---
    "Точки", "Points"
    ,
    "Выбор количества отсчётов для сохраняемых сигналов. "
    "При увеличении количества отсчётов уменьшается количество сохранённых в памяти сигналов."
    ,
    "Choice of number of counting for the saved signals. "
    "At increase in number of counting the quantity of the signals kept in memory decreases.",
    "281",  "281",
    "512",  "512",
    "1024", "1024",
    ENUM_POINTS, pMemory, FuncActiveMemoryNumPoinst, ChangeC_Memory_NumPoints, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMemoryExtName,                                                                           //--- ПАМЯТЬ - ВНЕШН ЗУ - Имя файла ---
    "Имя файла", "File name"
    ,
    "Задаёт режим наименования файлов при сохранении на внешний накопитель:\n"
    "\"По маске\" - файлы именуются автоматически по заранее введённой маске (след. пункт меню),\n"
    "\"Вручную\" - каждый раз имя файла нужно задавать вручную"
    ,
    "Sets the mode name when saving files to an external drive:\n"
    "\"By mask\" - files are named automatically advance to the entered mask(seq.Menu),\n"
    "\"Manual\" - each time the file name must be specified manually"
    ,
    "По маске", "Mask",
    "Вручную",  "Manually",
   FILE_NAMING_MODE, mspMemoryExt, FuncActive, FuncChangedChoice, FuncDraw
);
    
//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMemoryExtAutoConnectFlash,                                                         //--- ПАМЯТЬ - ВНЕШН ЗУ - Автоподключение ---
    "Автоподкл.", "AutoConnect",
    "Eсли \"Вкл\", при подключении внешнего накопителя происходит автоматический переход на страницу ПАМЯТЬ - Внешн ЗУ",
    "If \"Enable\", when you connect an external drive is automatically transferred to the page MEMORY - Ext.Storage",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    FLASH_AUTOCONNECT, mspMemoryExt, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMemoryExtModeBtnMemory,                                                              //--- ПАМЯТЬ - ВНЕШН ЗУ - Реж кн ПАМЯТЬ ---
    "Реж кн ПАМЯТЬ", "Mode btn MEMORY",
    "\"Meню\" - нажатие кнопки ПАМЯТЬ открывает страницу ПАМЯТЬ\n\"Сохранение\" - нажатие кнопки ПАМЯТЬ сохраняет файл на внешнее ЗУ",
    "\"Menu\" - pressing the ПАМЯТЬ button opens the MEMORY page\n\"Save\"- pressing the MEMORY button saves the file to an external memory",
    "Меню",       "Menu",
    "Сохранение", "Save",
    MODE_BTN_MEMORY, mspMemoryExt, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       mcMemoryExtModeSave,                                                                   //--- ПАМЯТЬ - ВНЕШН ЗУ - Сохранять как ---
    "Сохранять как", "Save as"
    ,
    "Если выбран вариант \"Изображение\", сигнал будет сохранён в текущем каталоге в графическом файле с расширением BMP\n"
    "Если выбран вариант \"Текст\", сигнал будет сохранён в текущем каталоге в текстовом виде в файле с раширением TXT"
    ,
    "If you select \"Image\", the signal will be stored in the current directory in graphic file with the extension BMP\n"
    "If you select \"Text\", the signal will be stored in the current directory as a text file with the extension TXT"
    ,
    "Изображение", "Image",
    "Текст",       "Text",
    MODE_SAVE_SIGNAL, mspMemoryExt, FuncActive, FuncChangedChoice, FuncDraw
);

//----------------------------------------------------------------------------------------------------------------------------------------------------   
void DrawSetName()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 60;
    int width = Grid::Width() - 80;
    int height = 80;

    Painter::DrawRectangle(x0, y0, width, height, Color::FILL);
    Painter::FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, Color::BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while (symbolsAlphaBet[index][0] != ' ')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // Теперь рисуем цифры и пробел
    position = 0;
    while (symbolsAlphaBet[index][0] != 'a')
    {
        DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while (symbolsAlphaBet[index][0] != '%')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    int x = Painter::DrawText(x0 + deltaX, y0 + 65, FILE_NAME, Color::FILL);
    Painter::FillRegion(x, y0 + 65, 5, 8, Color::FLASH_10);
}

static void DrawFileMask(int x, int y)
{
    char *ch = FILE_NAME_MASK;

    Painter::SetColor(Color::FILL);
    while (*ch != '\0')
    {
        if (*ch >= 32)
        {
            x = Painter::DrawChar(x, y, *ch);
        }
        else
        {
            if (*ch == 0x07)
            {
                x = Painter::DrawChar(x, y, '%');
                x = Painter::DrawChar(x, y, (char)(0x30 | *(ch + 1)));
                x = Painter::DrawChar(x, y, 'N');
                ch++;
            }
            else
            {
                x = Painter::DrawText(x, y, symbolsAlphaBet[*ch + 0x40]);
            }
        }
        ch++;
    }
    Painter::FillRegion(x, y, 5, 8, Color::FLASH_10);
}

void DrawSetMask()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Painter::DrawRectangle(x0, y0, width, height, Color::FILL);
    Painter::FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, Color::BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while(symbolsAlphaBet[index][0] != ' ')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }
    
    // Теперь рисуем цифры и пробел
    position = 0;
    while(symbolsAlphaBet[index][0] != 'a')
    {
        DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while(symbolsAlphaBet[index][0] != '%')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    // Теперь рисуем спецсимволы
    position = 0;
    while (index < (sizeof(symbolsAlphaBet) / 4))
    {
        DrawStr(index, x0 + deltaX + 26 + position * 20, y0 + deltaY0 + deltaY * 3);
        index++;
        position++;
    }

    DrawFileMask(x0 + deltaX, y0 + 65);

    static const char* strings[] =
    {
        "%y - год, %m - месяц, %d - день",
        "%H - часы, %M - минуты, %S - секунды",
        "%nN - порядковый номер, где",
        "n - минимальное количество знаков для N"
    };

    deltaY--;
    Painter::SetColor(Color::FILL);
    for(int i = 0; i < sizeof(strings) / 4; i++)
    {
        Painter::DrawText(x0 + deltaX, y0 + 100 + deltaY * i, strings[i]);
    }
}

void Memory_SaveSignalToFlashDrive()
{
    if (gBF.flashDriveIsConnected == 1)
    {
        if (FILE_NAMING_MODE_IS_HAND)
        {
            Menu::OpenPageAndSetItCurrent(PageSB_Memory_SetName);
            Display::SetAddDrawFunction(DrawSetName);
        }
        else
        {
            gMemory.needForSaveToFlashDrive = 1;
        }
    }
    else
    {
        gMemory.exitFromModeSetNameTo = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_MemLast_Exit()
{
    MODE_WORK = ModeWork_Dir;
    if (gMemory.runningFPGAbeforeSmallButtons == 1)
    {
        FPGA::Start();
        gMemory.runningFPGAbeforeSmallButtons = 0;
    }
    Display::RemoveAddDrawFunction();
}

DEF_SMALL_BUTTON_EXIT(  sbExitMemLast,                                                                            //--- ПАМЯТЬ - ПОСЛЕДНИЕ - Выход ---
    mspMemLast, FuncActive, PressSB_MemLast_Exit, DrawSB_Exit
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnPressMemoryLatest()
{
    gMemory.currentNumLatestSignal = 0;
    gMemory.runningFPGAbeforeSmallButtons = FPGA::IsRunning() ? 1 : 0;
    FPGA::Stop(false);
    MODE_WORK = ModeWork_RAM;
}

static void FuncDrawingAdditionSPageMemoryLast()
{
    char buffer[20];

    int width = 40;
    int height = 10;
    Painter::FillRegion(Grid::Right() - width, GRID_TOP, width, height, Color::BACK);
    Painter::DrawRectangle(Grid::Right() - width, GRID_TOP, width, height, Color::FILL);
    Painter::DrawText(Grid::Right() - width + 2, GRID_TOP + 1, Int2String(gMemory.currentNumLatestSignal + 1, false, 3, buffer));
    Painter::DrawText(Grid::Right() - width + 17, GRID_TOP + 1, "/");
    Painter::DrawText(Grid::Right() - width + 23, GRID_TOP + 1, Int2String(Storage::AllDatas(), false, 3, buffer));
}

DEF_PAGE_SB(        mspMemLast,                                                                                              // ПАМЯТЬ - ПОСЛЕДНИЕ ///
    "ПОСЛЕДНИЕ", "LATEST",
    "Переход в режим работы с последними полученными сигналами",
    "Transition to an operating mode with the last received signals",
    &sbExitMemLast,
    0,
    &sbMemLastNext,
    &sbMemLastPrev,
    &sbMemLastIntEnter,
    &sbMemLastSaveToFlash,
    PageSB_Memory_Last, &pMemory, FuncActive, OnPressMemoryLatest, FuncDrawingAdditionSPageMemoryLast, RotateSB_MemLast
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_SetMask_Exit()
{
    Display::RemoveAddDrawFunction();
}

DEF_SMALL_BUTTON_EXIT(  sbExitSetMask,                                                                                         //--- МАСКА - Выход ---
    mspSetMask, FuncActive, PressSB_SetMask_Exit, DrawSB_Exit
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_SB(        mspSetMask,                                                                                       // Память - ВНЕШН ЗУ - МАСКА ///
    "МАСКА", "MASK",
    "Режим ввода маски для автоматического именования файлов",
    "Input mode mask for automatic file naming",
    &sbExitSetMask,
    &sbSetMaskDelete,
    0,
    0,
    &sbSetMaskBackspace,
    &sbSetMaskInsert,
    PageSB_Memory_Drive_Mask, &mspMemoryExt, IsActiveMemoryExtSetMask, OnPressMemoryExtMask, FuncDrawPage, OnMemExtSetMaskRegSet
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_FM_Exit()
{
    Display::SetDrawMode(DrawMode_Auto, 0);
    Display::RemoveAddDrawFunction();
}

DEF_SMALL_BUTTON(   sbExitFileManager,                                                                                 // ПАМЯТЬ - КАТАЛОГ - Выход ///
    "Выход", "Exit",
    "Сохранить выбранный каталог в качестве текущего и выйти",
    "Save the selected directory as the current one and exit",
    mspFileManager, FuncActive, PressSB_FM_Exit, DrawSB_Exit
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OnPress_Drive_Manager()
{
    Menu::OpenPageAndSetItCurrent(PageSB_Memory_Drive_Manager);
    Display::SetDrawMode(DrawMode_Hand, FileManager::Draw);
    gBF.needRedrawFileManager = 1;
}

static bool FuncOfActiveExtMemFolder()
{
    return gBF.flashDriveIsConnected == 1;
}

DEF_PAGE_SB(        mspFileManager,                                                                                            // ПАМЯТЬ - КАТАЛОГ ///
    "КАТАЛОГ", "DIRECTORY",
    "Открывает доступ к файловой системе подключенного накопителя",
    "Provides access to the file system of the connected drive",
    &sbExitFileManager,
    &sbFileManagerTab,
    0,
    0,
    &sbFileManagerLevelUp,
    &sbFileManagerLevelDown,
    PageSB_Memory_Drive_Manager, &mspMemoryExt, FuncOfActiveExtMemFolder, OnPress_Drive_Manager, FuncDrawPage, FileManager::RotateRegSet
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_6(         mspMemoryExt,                                                                                             // ПАМЯТЬ - ВНЕШН ЗУ ///
    "ВНЕШН ЗУ", "EXT STORAGE",
    "Работа с внешним запоминающим устройством.",
    "Work with external storage device.",
    mspFileManager,
    mcMemoryExtName,
    mspSetMask,
    mcMemoryExtModeSave,
    mcMemoryExtModeBtnMemory,
    mcMemoryExtAutoConnectFlash,
    Page_Memory_Drive, &pMemory, FuncActive, FuncPress
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnPressMemoryInt()
{
    Menu::OpenPageAndSetItCurrent(PageSB_Memory_Internal);
    MODE_WORK = ModeWork_ROM;
    FLASHMem::GetData(gMemory.currentNumIntSignal, &gDSmemInt, &gData0memInt, &gData1memInt);
}

DEF_PAGE_SB(        mspMemInt,                                                                                                // ПАМЯТЬ - ВНУТР ЗУ ///
    "ВНУТР ЗУ", "INT STORAGE",
    "Переход в режим работы с внутренней памятью",
    "Transition to an operating mode with internal memory",
    &sbExitMemInt,
    &sbMemIntShowSignalAlways,
    &sbMemIntModeShow,
    0,
    &sbMemIntSave,
    &sbMemIntSaveToFlash,
    PageSB_Memory_Internal, &pMemory, FuncActive, OnPressMemoryInt, FuncAdditionDrawingSPageMemoryInt, FuncOnRegSetMemInt
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnMemExtSetNameRegSet(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(symbolsAlphaBet) / 4 - 7);
}

// Страница вызывается при выбранном ручном режиме задания имени файла перед сохранением на флешку
DEF_PAGE_SB(        mpSetName,                                                                                                        // ИМЯ ФАЙЛА ///
    "", "", "", "",
    &sbExitSetName,
    &sbSetNameDelete,
    0,
    &sbSetNameBackspace,
    &sbSetNameInsert,
    &sbSetNameSave,
    PageSB_Memory_SetName, 0, FuncActive, FuncPress, FuncDrawPage, OnMemExtSetNameRegSet
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_4(         pMemory,                                                                                                             // ПАМЯТЬ ///
    "ПАМЯТЬ", "MEMORY",
    "Работа с внешней и внутренней памятью.",
    "Working with external and internal memory.",
    mcMemoryNumPoints,
    mspMemLast,
    mspMemInt,
    mspMemoryExt,
    Page_Memory, &mainPage, FuncActive, FuncPress
);


/** @}  @}
 */
