#pragma once
#include "defines.h"
#include "Hardware/Controls.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Panel
{
public:

    static void Init();

    static void EnableLEDRegSet(bool enable);
    /// Включить/выключить светодиод КАНАЛ 1.
    static void EnableLEDChannelA(bool enable);
    /// Включить/выключить светодиод КАНАЛ 2.
    static void EnableLEDChannelB(bool enable);
    /// Включить/выключить светодиод СИНХР.
    static void EnableLEDTrig(bool enable);
    /// Передать даннные в мк панели управления.
    static void TransmitData(uint16 data);
    /// В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
    static void Disable();

    static void Enable();
    /// Ожидать нажатие клавиши.
    static PanelButton WaitPressingButton();

    static bool ProcessingCommandFromPIC(uint16 command);

    static uint16 NextData();

    static bool IsFunctionalButton(PanelButton button);

    static void Long_Help();

    static uint8 dataSPIfromPanel;

private:
    /// B_Start
    static void StartDown();
    /// B_Power
    static void PowerDown();

    static void(*funcOnKeyDown[B_NumButtons])();

    static void(*funcOnLongPressure[B_NumButtons])();

    static void(*funculatorLeft[R_Set + 1])();

    static void(*funculatorRight[R_Set + 1])();

    static void ChannelALong();

    static void ChannelBLong();

    static void TimeLong();

    static void TrigLong();

    static void MenuLong();

    static void F1Long();

    static void F2Long();

    static void F3Long();

    static void F4Long();

    static void F5Long();

    static void RangeALeft();

    static void RangeARight();

    static void RShiftALeft();

    static void RShiftARight();

    static void RangeBLeft();

    static void RangeBRight();

    static void RShiftBLeft();

    static void RShiftBRight();

    static void TBaseLeft();

    static void TBaseRight();

    static void TShiftLeft();

    static void TShiftRight();

    static void TrigLevLeft();

    static void TrigLevRight();

    static void SetLeft();

    static void SetRight();

    static void OnTimerPressedKey();
};
