#include "defines.h" 
#include "globals.h"
#include <usbh_def.h>
#include <ff_gen_drv.h>

#ifdef STM32F207xx
#include <usbh_diskio.h>
#endif

#ifdef STM32F746xx
#include "usbh_diskio.h"
#endif

#include <usbh_core.h>
#include <usbh_msc.h>
#include "ffconf.h"
#include "FlashDrive.h"
#include "main.h"
#include <ctype.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    char nameDir[_MAX_LFN + 1];
    char lfn[(_MAX_LFN + 1)];
    FILINFO fno;
    DIR dir;
} StructForReadDir;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool GetNameFile(const char *fullPath, int numFile, char *nameFileOut, StructForReadDir *s);
static bool GetNextNameFile(char *nameFileOut, StructForReadDir *s);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FDrive::Init()
{
    ms->drive.state = StateDisk_Idle;
    ms->drive.connection = 0;
    ms->drive.active = 0;

    if (FATFS_LinkDriver(&USBH_Driver, ms->drive.USBDISKPath) == FR_OK)
    {
        USBH_StatusTypeDef res = USBH_Init(&handleUSBH, USBH_UserProcess, 0);
        res = USBH_RegisterClass(&handleUSBH, USBH_MSC_CLASS);
        res = USBH_Start(&handleUSBH);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void USBH_UserProcess(USBH_HandleTypeDef *, uint8 id)
{
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_CLASS_ACTIVE:
            ms->drive.active++;
            ms->drive.state = StateDisk_Start;
            break;

        case HOST_USER_CLASS_SELECTED:
            break;

        case HOST_USER_CONNECTION:
            ms->drive.connection++;
            ms->state = State_Mount;
            f_mount(NULL, (TCHAR const*)"", 0);
            break;

        case HOST_USER_DISCONNECTION:
            break;

        default:
            break;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive::Update()
{
    USBH_Process(&handleUSBH);
    if (ms->drive.state == StateDisk_Start)
    {
        if (f_mount(&(ms->drive.USBDISKFatFS), (TCHAR const*)ms->drive.USBDISKPath, 0) == FR_OK)
        {
            return true;
        }
        else
        {
            ms->state = State_WrongFlash;
            return false;
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void ToLower(char *str)
{
    while (*str)
    {
        *str = (char)tolower(*str);
        str++;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive::FileExist(char *fileName)
{
    char nameFile[255];
    char file[255];
    strcpy(file, fileName);
    ToLower(file);
    StructForReadDir strd;

    if (GetNameFile("\\", 0, nameFile, &strd))
    {
        ToLower(nameFile);
        if (strcmp(file, nameFile) == 0)
        {
            return true;
        }
        while (GetNextNameFile(nameFile, &strd))
        {
            ToLower(nameFile);
            if (strcmp(file, nameFile) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool GetNameFile(const char *fullPath, int numFile, char *nameFileOut, StructForReadDir *s)
{
    memcpy(s->nameDir, fullPath, strlen(fullPath));
    s->nameDir[strlen(fullPath)] = '\0';

#ifdef STM32F207xx
    s->fno.lfname = s->lfn;
    s->fno.lfsize = sizeof(s->lfn);
#else
    strcpy(s->fno.fname, s->lfn);
    s->fno.fsize = strlen(s->lfn);
#endif

    DIR *pDir = &s->dir;
    FILINFO *pFNO = &s->fno;
    if (f_opendir(pDir, s->nameDir) == FR_OK)
    {
        int numFiles = 0;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameFileOut = '\0';
                f_closedir(pDir);
                return false;
            }
            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameFileOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }
#ifdef STM32F207xx
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
#else
            char *fn = (char *)&pFNO->fname;
#endif
            if (numFile == numFiles && (pFNO->fattrib & AM_DIR) == 0)
            {
                strcpy(nameFileOut, fn);
                return true;
            }
            if ((pFNO->fattrib & AM_DIR) == 0 && (pFNO->fname[0] != '.'))
            {
                numFiles++;
            }
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool GetNextNameFile(char *nameFileOut, StructForReadDir *s)
{
    FILINFO *pFNO = &s->fno;
    bool alreadyNull = false;
    while (true)
    {
        if (f_readdir(&s->dir, &s->fno) != FR_OK)
        {
            *nameFileOut = '\0';
            f_closedir(&s->dir);
            return false;
        }
        if (s->fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameFileOut = '\0';
                f_closedir(&s->dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
#ifdef STM32F207xx
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
#else
            char *fn = (char *)&pFNO->fname;
#endif
            if ((pFNO->fattrib & AM_DIR) == 0 && pFNO->fname[0] != '.')
            {
                strcpy(nameFileOut, fn);
                return true;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int FDrive::OpenFileForRead(char *fileName)
{
    if (f_open(&ms->drive.file, fileName, FA_READ) == FR_OK)
    {
#ifdef STM32F207xx
        return (int)ms->drive.file.fsize;
#else
        return (int)f_size(&ms->drive.file);
#endif
    }
    return -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int FDrive::ReadFromFile(int numBytes, uint8 *buffer)
{
    uint readed = 0;
    if (f_read(&ms->drive.file, buffer, (uint)numBytes, &readed) == FR_OK)
    {
        return (int)readed;
    }
    return -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FDrive::CloseOpenedFile()
{
    f_close(&ms->drive.file);
}
