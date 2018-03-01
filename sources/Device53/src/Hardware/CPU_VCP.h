#pragma once
#include <usbd_def.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VCP_FLUSH()                         CPU::VCP::Flush()
#define VCP_SEND_DATA_SYNCH(buffer, size)   CPU::VCP::SendDataSynch(buffer, size)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class VCP
{
public:
    static void Init();
    static void SendDataAsinch(uint8 *buffer, int size);
    static void SendDataSynch(const uint8 *data, int size);
    static void SendStringSynch(char *data);                   ///< Передаётся строка без завершающего нуля.
    static void SendFormatStringAsynch(char *format, ...);     ///< Эта строка передаётся с завершающим символом \n.
    static void SendFormatStringSynch(char *format, ...);      ///< Эта строка передаётся с завершающим символом \n.
    static void SendByte(uint8 data);
    static void Flush();
    static void SendStringAsinch(char *data);

    static USBD_HandleTypeDef handleUSBD;
    static PCD_HandleTypeDef  handlePCD;

private:
    static bool PrevSendingComplete();
};
