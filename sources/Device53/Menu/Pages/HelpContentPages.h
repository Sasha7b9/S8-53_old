#pragma once


typedef enum
{
    TypePage_Content,       // �� ���� �������� ����������� ������ �� ������ ��������
    TypePage_Description    // �� ���� �������� ��� ������, ������ ��������
} TypePageHelp;


#define MAX_PAGES 3

/** @todo ���������� �� ��������� funcNotUsed */

typedef struct  
{
    TypePageHelp    type;
    void*           parent;             ///< ����� ������������ ��������
    pFuncBV         funcNotUsed;        ///< ��������� ��� ������������� � ����� Page - ����� ��� ���������.
    char*           titleHint[4];       ///< �������� �������� �� ������� � ���������� ������, � ����� ���������� �� ������� � ���������� (��� ������ TypePage_Description)
    void*           pages[MAX_PAGES];   ///< ������ �������� ������ ��������� ������� � ������ TypePage_Content
} PageHelp;

#define TITLE(page) page->titleHint[LANG]
#define HINT(page)  page->titleHint[LANG * 2]

extern const PageHelp helpMenu;

static const PageHelp helpMenuCommon =
{
    TypePage_Description, (void*)&helpMenu, 0,
    {
        "����� �������� ��������� ����", "General description of the principles of the menu"
        ,
        "������ �� ������ ���������� ����� ��� ���� ������� - ��������, ������������� ����� 0.5 ��� � �������, ������������� ����� 0.5 ���. "
        "���� ������������ ����� ����������� ��������� ���������. "
        "������� �������� ���� ����������� �������� ���� ������� �������� ������ ����. "
        "�� ���� ��������� ������� �������� ������� ���������������� ������ � ������� ������� ����� �� ������ �� ������ ����������:\n"
        "\"�������\" - �������\n"
        "\"����� 1\" - ����� 1\n"
        "\"����� 2\" - ����� 2\n"
        "\"�����.\" - �����\n"
        "\"����E����\" - ����\n"
        "\"�������\" - �������\n"
        "\"������\" - ������\n"
        "\"���������\" - �����\n"
        "\"������\" - ������\n"
        "\"������\" - ������\n"
        "� ������ �������� ������� �������� ������� ��������� ��������������� ���������"
        ,
        ""
    }
};

static const PageHelp helpMenuControls =
{
    TypePage_Description, (void*)&helpMenu, 0,
    {
        "�������� ������� �����������", "Description of the controls"
        ,
        "",
        ""
    }
};

extern const PageHelp helpMain;

static const PageHelp helpSCPI =
{
    TypePage_Description, (void*)&helpMain, 0,
    {
        "������ � SCPI", "Working with SCPI",
        "",
        ""
    }
};

static const PageHelp helpMenu =
{
    TypePage_Content, (void*)&helpMain, 0,
    {
        "������ � ����", "Working with menus",
        "",
        ""
    },
    {
        (void*)&helpMenuCommon,
        (void*)&helpMenuControls
    }
};

static const PageHelp helpMain =
{
    TypePage_Content,  0, 0,
    {
        "������", "HELP",
        "",
        ""
    },
    {
        (void*)&helpMenu,
        (void*)&helpSCPI
    }
};
