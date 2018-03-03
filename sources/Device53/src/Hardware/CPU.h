#pragma once
#include "Hardware/stm32/2XX/stm207.h"
#include "globals.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ADDR_BANK           0x60000000
#define ADDR_CDISPLAY       ((uint8*)(ADDR_BANK + 0x00800000))
#define ADDR_FPGA           ((uint8*)(ADDR_BANK + 0x00c80000))  // Адрес записи в аппаратные регистры.

#define FSMC_SET_MODE(x)
#define FSMC_RESTORE_MODE()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPU : public STM207
{
public:
    
    static void Init();

    static uint CalculateCRC32(uint address = 0x08020000, uint numBytes = 128 * 1024 * 3 / 4);

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class RTC_
    {
    friend class CPU;
        
    private:
        
        static void Init();

    public:

        static bool SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);

        static PackedTime GetPackedTime();
    };

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class ADC_
    {
    friend class CPU;
    
    private:
        static void Init();

    public:

        static uint16 value;
    };

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class FLASH_
    {
    /// Пока ограничено количеством квадратиков, которые можно вывести в одну линию внизу сетки
    #define MAX_NUM_SAVED_WAVES 23

    public:
        static void LoadSettings();

        static void SaveSettings(bool verifyLoaded = false);
        /// Если даннные есть, соответствующий элемент массива равен true/.
        static void GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES]);

        static bool ExistData(int num);

        static void SaveData(int num, DataSettings *ds, uint8 *data0, uint8 *data1);

        static bool GetData(int num, DataSettings **ds, uint8 **data0, uint8 **data1);

        static void DeleteData(int num);
    };

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class OTP
    {
    public:
        
        static bool SaveSerialNumber(char *serialNumber);
        /// Функция возвращает число свободных мест для записи. Если 0, то места в OTP уже не осталось. 
        static int GetSerialNumber(char buffer[17]);
    };

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class FSMC
    {
    friend class CPU;

    private:

        static void Init();

    public:

        static uint8 Read(uint8* address);

        static void Write(uint8 *address, uint8 value);
    };

private:

    static void EnablePeriphery();
};


#ifdef __cplusplus
extern "C" {
#endif

    /// Для рандомизатора
    void ADC_IRQHandler();
    /// Программный NSS для SPI (смотри Hardware::SPIforPanel.c::PanelInit() and HAL_GPIO_EXTI_Callback().
    void EXTI0_IRQHandler();
    /// See Hardware::SPIforPanel.c::HAL_SPI_RxCpltCallback().
    void SPI1_IRQHandler();

    void OTG_HS_IRQHandler();

    void DMA1_Stream5_IRQHandler();

#ifdef __cplusplus
}
#endif
