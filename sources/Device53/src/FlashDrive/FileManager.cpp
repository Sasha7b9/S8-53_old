#include "FileManager.h"
#include "Menu/Menu.h"
#include "Menu/Pages/PageMemory.h"
#include "Settings/Settings.h"
#include "Settings/SettingsMemory.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Grid.h"
#include "Utils/Math.h"
#include "Utils/StringUtils.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/RTC.h"
#include "FlashDrive/FlashDrive.h"
#include "Panel/Panel.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RECS_ON_PAGE 23
#define WIDTH_COL 135


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static char currentDir[255] = "\\";
static int numFirstDir = 0;         // ����� ������� ����������� �������� � ����� ������. ����� ����� ���� �������� RECS_ON_PAGE ���������
static int numCurDir = 0;           // ����� ������������� ��������
static int numFirstFile = 0;        // ����� ������� ����������� ����� � ������ ������. ����� ����� ���� �������� RECS_ON_PAGE ������.
static int numCurFile = 0;          // ����� ������������� �����
static int numDirs = 0;
static int numFiles = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FileManager::Init()
{
    strcpy(currentDir, "\\");
    numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawLongString(int x, int y, char *string, bool hightlight)
{
    int length = Font_GetLengthText(string);

    Color color = Color::FILL;
    if (hightlight)
    {
        Painter::FillRegion(x - 1, y, WIDTH_COL + 9, 8, color);
        color = Color::BACK;
    }

    if (length <= WIDTH_COL)
    {
        Painter::DrawText(x, y, string, color);
    }
    else
    {
        Painter::DrawTextWithLimitationC(x, y, string, color, x, y, WIDTH_COL, 10);
        Painter::DrawText(x + WIDTH_COL + 3, y, "...");
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawHat(int x, int y, char *string, int num1, int num2)
{
    Painter::FillRegion(x - 1, y, WIDTH_COL + 9, RECS_ON_PAGE * 9 + 11, Color::BACK);
    Painter::SetColor(Color::FILL);
    Painter::DrawFormatText(x + 60, y, string, num1, num2);
    Painter::DrawHLine(y + 10, x + 2, x + 140);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDirs(int x, int y)
{
    FDrive::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
    DrawHat(x, y, "������� : %d/%d", numCurDir + ((numDirs == 0) ? 0 : 1), numDirs);
    char nameDir[255];
    StructForReadDir sfrd;
    y += 12;
    if (FDrive::GetNameDir(currentDir, numFirstDir, nameDir, &sfrd))
    {
        int  drawingDirs = 0;
        DrawLongString(x, y, nameDir, gBF.cursorInDirs == 1 && ( numFirstDir + drawingDirs == numCurDir));
        while (drawingDirs < (RECS_ON_PAGE - 1) && FDrive::GetNextNameDir(nameDir, &sfrd))
        {
            drawingDirs++;
            DrawLongString(x, y + drawingDirs * 9, nameDir, gBF.cursorInDirs == 1 && ( numFirstDir + drawingDirs == numCurDir));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawFiles(int x, int y)
{
    DrawHat(x, y, "���� : %d/%d", numCurFile + ((numFiles == 0) ? 0 : 1), numFiles);
    char nameFile[255];
    StructForReadDir sfrd;
    y += 12;
    if (FDrive::GetNameFile(currentDir, numFirstFile, nameFile, &sfrd))
    {
        int drawingFiles = 0;
        DrawLongString(x, y, nameFile, gBF.cursorInDirs == 0 && (numFirstFile + drawingFiles == numCurFile));
        while (drawingFiles < (RECS_ON_PAGE - 1) && FDrive::GetNextNameFile(nameFile, &sfrd))
        {
            drawingFiles++;
            DrawLongString(x, y + drawingFiles * 9, nameFile, gBF.cursorInDirs == 0 && (numFirstFile + drawingFiles == numCurFile));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawNameCurrentDir(int left, int top)
{
    Painter::SetColor(Color::FILL);
    int length = Font_GetLengthText(currentDir);
    if (length < 277)
    {
        Painter::DrawText(left + 1, top + 1, currentDir);
    }
    else
    {
        char *pointer = currentDir + 2;
        while (length > 277)
        {
            while (*pointer != '\\' && pointer < currentDir + 255)
            {
                pointer++;
            }
            if (pointer >= currentDir + 255)
            {
                return;
            }
            length = Font_GetLengthText(++pointer);
        }
        Painter::DrawText(left + 1, top + 1, pointer);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FileManager::Draw()
{
    if (gBF.needRedrawFileManager == 0)
    {
        return;
    }

    int left = 1;
    int top = 1;
    int width = 297;
    int left2col = width / 2;

    if (gBF.needRedrawFileManager == 1)
    {
        Painter::BeginScene(Color::BACK);
        Menu::Draw();
        Painter::DrawRectangle(0, 0, width, 239, Color::FILL);
        Painter::FillRegion(left, top, Grid::Width() - 2, Grid::FullHeight() - 2, Color::BACK);
        FDrive::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
        DrawNameCurrentDir(left, top + 2);
        Painter::DrawVLine(left2col, top + 16, 239, Color::FILL);
        Painter::DrawHLine(top + 15, 0, width);
    }

    if (gBF.needRedrawFileManager != 3)
    {
        DrawDirs(left + 2, top + 18);
    }

    if (gBF.needRedrawFileManager != 2)
    {
        DrawFiles(left2col + 3, top + 18);
    }

    Painter::EndScene();

    //Painter::SaveScreenToFlashDrive();

    gBF.needRedrawFileManager = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FileManager::PressSB_Tab()
{
    gBF.needRedrawFileManager = 1;

    if (gBF.cursorInDirs == 1)
    {
        if (numFiles != 0)
        {
            gBF.cursorInDirs = 0;
        }
    }
    else
    {
        if (numDirs != 0)
        {
            gBF.cursorInDirs = 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FileManager::PressSB_LevelDown()
{
    gBF.needRedrawFileManager = 1;
    if (gBF.cursorInDirs == 0)
    {
        return;
    }
    char nameDir[100];
    StructForReadDir sfrd;
    if (FDrive::GetNameDir(currentDir, numCurDir, nameDir, &sfrd))
    {
        if (strlen(currentDir) + strlen(nameDir) < 250)
        {
            FDrive::CloseCurrentDir(&sfrd);
            strcat(currentDir, "\\");
            strcat(currentDir, nameDir);
            numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
        }

    }
    FDrive::CloseCurrentDir(&sfrd);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FileManager::PressSB_LevelUp()
{
    gBF.needRedrawFileManager = 1;
    if (strlen(currentDir) == 1)
    {
        return;
    }
    char *pointer = currentDir + strlen(currentDir);
    while (*pointer != '\\')
    {
        pointer--;
    }
    *pointer = '\0';
    numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
    gBF.cursorInDirs = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void IncCurrentDir()
{
    if (numDirs > 1)
    {
        numCurDir++;
        if (numCurDir > numDirs - 1)
        {
            numCurDir = 0;
            numFirstDir = 0;
        }
        if (numCurDir - numFirstDir > RECS_ON_PAGE - 1)
        {
            numFirstDir++;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DecCurrentDir()
{
    if (numDirs > 1)
    {
        numCurDir--;
        if (numCurDir < 0)
        {
            numCurDir = numDirs - 1;
            numFirstDir = numDirs - RECS_ON_PAGE;
            Limitation<int>(&numFirstDir, 0, numCurDir);
        }
        if (numCurDir < numFirstDir)
        {
            numFirstDir = numCurDir;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void IncCurrentFile()
{
    if (numFiles > 1)
    {
        numCurFile++;
        if (numCurFile > numFiles - 1)
        {
            numCurFile = 0;
            numFirstFile = 0;
        }
        if (numCurFile - numFirstFile > RECS_ON_PAGE - 1)
        {
            numFirstFile++;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DecCurrentFile()
{
    if (numFiles > 1)
    {
        numCurFile--;
        if (numCurFile < 0)
        {
            numCurFile = numFiles - 1;
            numFirstFile = numFiles - RECS_ON_PAGE;
            Limitation<int>(&numFirstFile, 0, numCurFile);
        }
        if (numCurFile < numFirstFile)
        {
            numFirstFile = numCurFile;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FileManager::RotateRegSet(int angle)
{
    Sound::RegulatorSwitchRotate();
    if (gBF.cursorInDirs == 1)
    {
        angle > 0 ? DecCurrentDir() : IncCurrentDir();
        gBF.needRedrawFileManager = 2;
    }
    else
    {
        angle > 0 ? DecCurrentFile() : IncCurrentFile();
        gBF.needRedrawFileManager = 3;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FileManager::GetNameForNewFile(char name[255])
{
    char buffer[20];

    static int number = 0;

    strcpy(name, currentDir);
    strcat(name, "\\");

    int size = strlen(FILE_NAME);
    if (size == 0)
    {
        return false;
    }

    if (FILE_NAMING_MODE_IS_HAND)
    {
        Limitation<int>(&size, 1, 95);
        strcat(name, FILE_NAME);
        strcat(name, ".");
        strcat(name, MODE_SAVE_SIGNAL_IS_BMP ? "bmp" : "txt");
        return true;
    }
    else
    {
        PackedTime time = RTClock::GetPackedTime();
                           //  1          2           3         4           5             6
        uint values[] = {0, time.year, time.month, time.day, time.hours, time.minutes, time.seconds};

        char *ch = FILE_NAME_MASK;
        char *wr = name;

        while (*wr != '\0')
        {
            wr++;
        }

        while (*ch)
        {
            if (*ch >= 0x30)
            {
                *wr = *ch;
                wr++;
            }
            else
            {
                if (*ch == 0x07)
                {
                    number++;
                    strcpy(wr, Int2String(number, false, *(ch + 1), buffer));
                    wr += strlen(buffer);
                    ch++;
                }
                else
                {
                    if (*ch >= 0x01 && *ch <= 0x06)
                    {
                        strcpy(wr, Int2String(values[*ch], false, 2, buffer));
                        wr += strlen(buffer);
                    }
                }
            }
            ch++;
        }

        *wr = '.';
        *(wr + 1) = '\0';

        strcat(name, MODE_SAVE_SIGNAL_IS_BMP ? "bmp" : "txt");

        return true;
    }
}