#pragma once

/// Здесь ID всех представленных в меню страниц
typedef enum
{
    Page_Display,                   ///< ДИСПЕЙ
    Page_Display_Accum,             ///< ДИСПЛЕЙ - НАКОПЛЕНИЕ
    Page_Display_Average,           ///< ДИСПЛЕЙ - УСРЕДНЕНИЕ
    Page_Display_Grid,              ///< ДИСПЛЕЙ - СЕТКА
    Page_DisplayRefresh,            ///< ДИСПЛЕЙ - ОБНОВЛЕНИЕ
    Page_Main,                      ///< Главная страница меню. Из неё растут все остальные
    Page_Debug,                     ///< ОТЛАДКА
    Page_DebugConsole,              ///< ОТЛАДКА - КОНСОЛЬ
    Page_DebugADC,                  ///< ОТЛАДКА - АЦП
    Page_DebugADCbalance,           ///< ОТЛАДКА - АЦП - БАЛАНС
    Page_DebugADCstretch,           ///< ОТЛАДКА - АЦП - РАСТЯЖКА
    Page_DebugADCrShift,            ///< ОТЛАДКА - АЦП - ДОП. СМЕЩЕНИЕ
    Page_DebugADCaltShift,          ///< ОТЛАДКА - АЦП - ДОП. СМЕЩ. ПАМ.
    Page_DebugShowRegisters,        ///< ОТЛАДКА - РЕГИСТРЫ
    Page_DebugRandomizer,           ///< ОТЛАДКА - РАНД-ТОР
    Page_DebugInformation,          ///< ОТЛАДКА - ИНФОРМАЦИЯ
    Page_DebugSound,                ///< ОТЛАДКА - ЗВУК
    Page_DebugAltera,               ///< ОТЛАДКА - АЛЬТЕРА
    Page_ChannelA,                  ///< КАНАЛ 1
    Page_ChannelB,                  ///< КАНАЛ 2
    Page_Cursors,                   ///< КУРСОРЫ
    Page_Trig,                      ///< СИНХР.
    Page_Memory,                    ///< ПАМЯТЬ
    Page_Memory_Drive,              ///< ПАМЯТЬ - ВНЕШН. ЗУ
    Page_Measures,                  ///< ИЗМЕРЕНИЯ
    Page_MeasuresField,             ///< ИЗМЕРЕНИЯ - ЗОНА
    Page_Service,                   ///< СЕРВИС
    Page_Service_Display,           ///< СЕРВИС - ДИСПЛЕЙ
    Page_Service_Display_Colors,    ///< СЕРВИС - ДИСПЛЕЙ - ЦВЕТА
    Page_Service_Calibrator,        ///< СЕРВИС - КАЛИБРАТОР
    Page_Service_Ethernet,          ///< СЕРВИС - ETHERNET
    Page_Help,                      ///< ПОМОЩЬ
    Page_Time,                      ///< РАЗВЕРТКА
    Page_Math,                      ///< СЕРВИС - МАТЕМАТИКА
    Page_MathFFT,
    Page_Trig_Search,               ///< СИНХР - АВТ ПОИСК
    PageSB_Cursors_Set,             ///< Открыта страница КУРСОРЫ - УСТАНОВИТЬ
    PageSB_Measures_Tune,           ///< Открыта страница ИЗМЕРЕНИЯ - Настроить
    Page_SB_MeasTuneField,          ///< Открыта страница ИЗМЕРЕНИЯ - ЗОНА - Задать
    PageSB_Memory_Last,             ///< Открыта страница ПАМЯТЬ - Последние
    PageSB_Memory_Internal,         ///< Открыта страница ПАМЯТЬ - Внутр ЗУ
    PageSB_Memory_Drive_Mask,       ///< Открыта страница ПАМЯТЬ - ВНЕШН ЗУ - МАСКА
    PageSB_Memory_SetName,          ///< Появляется перед сохранением файла на флешку для задания имени файла при соответствующей опции ВНЕШН ЗУ - Имя файла
    PageSB_Memory_Drive_Manager,    ///< Открыта страница ПАМЯТЬ - ВНЕШН ЗУ - КАTAЛОГ
    PageSB_Help,                    ///< Открыта страница HELP
    PageSB_Service_FFT_Cursors,     ///< Открыта страница СЕРВИС - Математика - Курсоры
    PageSB_Service_Function,        ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ
    PageSB_Service_Information,     ///< СЕРВИС - ИНФОРМАЦИЯ
    PageSB_Debug_SerialNumber,      ///< ОТАДКА - С/Н
    Page_NumPages,
    Page_NoPage
} NamePage;             // При добавлении страниц нужно изменять инициализацию SettingsMenu в SSettings.c
