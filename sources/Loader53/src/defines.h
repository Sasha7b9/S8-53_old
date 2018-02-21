#pragma once

#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wchar-subscripts"
#pragma clang diagnostic ignored "-Winvalid-source-encoding"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#pragma clang diagnostic ignored "-Wwritable-strings"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wfloat-equal"


#include <stm32f2xx.h>


typedef signed   char           int8;
typedef unsigned char           uint8;
typedef unsigned char           uchar;
typedef signed   short int      int16;
typedef unsigned short int      uint16;
typedef unsigned int            uint;
typedef const    char *const    pString;


#pragma clang diagnostic ignored "-Wstrict-prototypes"

typedef void  (*pFuncVV)();

#pragma clang diagnostic warning "-Wstrict-prototypes"


#define ERROR_VALUE_FLOAT   1.111e29f
#define ERROR_VALUE_UINT8   255
#define ERROR_STRING_VALUE  "--.--"

#define _GET_BIT(value, bit)                    (((value) >> bit) & 0x01)
#define _SET_BIT(value, bit)                    ((value) |= (1 << bit))
#define _CLEAR_BIT(value, bit)                  ((value) &= (~(1 << bit)))
#define _SET_BIT_VALUE(value, numBit, bitValue) (value |= (bitValue << numBit))

// Для определения задания буферов лучше пользоваться этими макросами, чтобы потом легче было отследить, где можно памяти освободить
#define CHAR_BUF(name, size)            char name[size]
#define CHAR_BUF2(name, size1, size2)   char name[size1][size2]

#define ERROR_HANDLER() _Error_Handler(__FILE__, __LINE__);
void _Error_Handler(char *, int);

#define DEBUG
