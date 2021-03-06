#pragma once   
#include "defines.h"


#define LOG_WRITE(...)  Log_Write(__VA_ARGS__)
#define LOG_ERROR(...)  Log_Error(__MODULE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_FUNC_ENTER  Log_Write("%s enter", __FUNCTION__);
#define LOG_FUNC_LEAVE  Log_Write("%s leave", __FUNCTION__);
#define LOG_TRACE       Log_Write("%s : %d", __MODULE__, __LINE__);

void Log_Error(const char *module, const char *func, int numLine, char *format, ...);
void Log_Write(char *format, ...);
void Log_DisconnectLoggerUSB(void);
void Log_EnableLoggerUSB(bool enable);
