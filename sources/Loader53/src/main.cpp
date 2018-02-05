/*
*************** Описание внизу ****************************
*/
#include "main.h"
#include "globals.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/Hardware.h"
#include "Settings/Settings.h"
#include "Display/Painter.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Hardware/FLASH.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FILE_NAME "S8-53.bin"

typedef void(*pFunction)();


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MainStruct *ms;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Upgrade();


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    pFunction JumpToApplication;

    ms = (MainStruct *)malloc(sizeof(MainStruct));
    ms->percentUpdate = 0.0f;

    Hardware::Init();

    Settings::Load();

    Timer::PauseOnTime(250);
    ms->state = State_Start;

    Display::Init();

    Timer::SetAndEnable(kTemp, Display::Update, 10);

    uint timeStart = gTimeMS;

    FDrive::Init();

    while (gTimeMS - timeStart < TIME_WAIT && !FDrive::Update())
    {
    }

    if ((ms->drive.connection && ms->drive.active == 0) ||  // Если флеша подключена, но в активное состояние почему-то не перешла
        (ms->drive.active && ms->state != State_Mount))     // или перешла в активное состояние, по почему-то не запустился процесс монтирования
    {
        free(ms);
        NVIC_SystemReset();
    }

    if (ms->state == State_Mount)                           // Это означает, что диск удачно примонтирован
    {
        if (FDrive::FileExist(FILE_NAME))                    // Если на диске обнаружена прошивка
        {
            ms->state = State_RequestAction;

            while (1)
            {
                PanelButton button = Panel::PressedButton();
                if (button == B_F1)
                {
                    ms->state = State_Upgrade;
                    Upgrade();
                    break;
                }
                else if (button == B_F5)
                {
                    ms->state = State_Ok;
                    break;
                }
            }
        }
        else
        {
            ms->state = State_NotFile;
        }
    }
    else if (ms->state == State_WrongFlash) // Диск не удалось примонтировать
    {
        Timer::PauseOnTime(5000);
    }

    ms->state = State_Ok;

    Panel::DeInit();

    Timer::Disable(kTemp);

    while (Display::IsRun())
    {
    }

    Display::Update();

    HAL_DeInit();

    free(ms);

    Hardware::DeInit();

    __disable_irq();
    // Теперь переходим на основную программу
    JumpToApplication = (pFunction)(*(__IO uint*)(MAIN_PROGRAM_START_ADDRESS + 4));
    __set_MSP(*(__IO uint*)MAIN_PROGRAM_START_ADDRESS);
    __enable_irq();
    JumpToApplication();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Upgrade()
{
    const int sizeSector = 1 * 1024;

    uint8 buffer[sizeSector];

    FLASH_Prepare();

    int size = FDrive::OpenFileForRead(FILE_NAME);
    int fullSize = size;
    uint address = ADDR_SECTOR_PROGRAM_0;

    while (size)
    {
        int readedBytes = FDrive::ReadFromFile(sizeSector, buffer);
        FLASH_WriteData(address, buffer, readedBytes);
        size -= readedBytes;
        address += (uint)readedBytes;

        ms->percentUpdate = 1.0f - (float)size / fullSize;
    }

    FDrive::CloseOpenedFile();
}


/*! \page page1 Алгоритм работы загрузчика
    \verbatim
    По адресу 0x08000000 находится загрузчик.
    После сброса он проверяет наличие флешки.
        Если флешка есть:
            проверяет наличие файла S8-53.bin. Если таковой имеется, выводит сообщение "На USB-диске обнаружено новоое программное обеспечение. Установить?"
            Если нажато "Да":
                1. Стираются сектора:
                    5 - 0x08020000
                    6 - 0x08040000
                    7 - 0x08060000
                2. На их место записывается содержимое файла S8-53.bin
        Если флешку примонтировать не удалось:
            Вывести сообщение "Не удалось примонтировать флешку. Убедитесь, что на ней файловая система fat32"
    Далее выполняется переход по адресу, указанному в 0x0802004
    \endverbatim
*/
