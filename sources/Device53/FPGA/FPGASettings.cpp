#include "defines.h"
#include "FPGATypes.h"
#include "FPGA.h"
#include "FPGA/FPGAMath.h"
#include "Settings/Settings.h"
#include "Hardware/Hardware.h"
#include "Hardware/FSMC.h"
#include "Display/Display.h"
#include "Utils/Math.h"
#include "Utils/StringUtils.h"
#include "Log.h"
#include "Hardware/Timer.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const uint8 masksRange[RangeSize] =
{
    BIN_U8(0000000),
    BIN_U8(1000000),
    BIN_U8(1100000),
    BIN_U8(1010000),
    BIN_U8(1000100),
    BIN_U8(1100100),
    BIN_U8(1010100),
    BIN_U8(1000010),
    BIN_U8(1100010),
    BIN_U8(1010010),
    BIN_U8(1000110),
    BIN_U8(1100110),
    BIN_U8(1010110)
};


// Добавочные смещения по времени для разверёток режима рандомизатора.
static int16 deltaTShift[TBaseSize] = {505, 489, 464, 412, 258};

typedef struct
{
    uint8 maskNorm;         // Маска. Требуется для записи в аппаратную часть при выключенном режиме пикового детектора.
    uint8 maskPeackDet;     // Маска. Требуется для записи в аппаратную часть при включенном режиме пикового детектора.
} TBaseMaskStruct;

static const TBaseMaskStruct masksTBase[TBaseSize] =
{
    {BIN_U8(00000000), BIN_U8(00000000)},
    {BIN_U8(00000000), BIN_U8(00000000)},
    {BIN_U8(00000000), BIN_U8(00000000)},
    {BIN_U8(00000000), BIN_U8(00000000)},
    {BIN_U8(00000000), BIN_U8(00000000)},
    {BIN_U8(00000000), BIN_U8(00000000)},
    {BIN_U8(00100010), BIN_U8(00000000)},
    {BIN_U8(00100001), BIN_U8(00100011)},
    {BIN_U8(01000000), BIN_U8(00100001)},
    {BIN_U8(01000010), BIN_U8(01000000)},
    {BIN_U8(01000001), BIN_U8(01000011)},
    {BIN_U8(01000100), BIN_U8(01000001)},
    {BIN_U8(01000110), BIN_U8(01000100)},
    {BIN_U8(01000101), BIN_U8(01000111)},
    {BIN_U8(01001000), BIN_U8(01000101)},
    {BIN_U8(01001010), BIN_U8(01001000)},
    {BIN_U8(01001001), BIN_U8(01001011)},
    {BIN_U8(01001100), BIN_U8(01001001)},
    {BIN_U8(01001110), BIN_U8(01001100)},
    {BIN_U8(01001101), BIN_U8(01001111)},
    {BIN_U8(01010000), BIN_U8(01001101)},
    {BIN_U8(01010010), BIN_U8(01010000)},
    {BIN_U8(01010001), BIN_U8(01010011)},
    {BIN_U8(01010100), BIN_U8(01010001)},
    {BIN_U8(01010110), BIN_U8(01010100)},
    {BIN_U8(01010101), BIN_U8(01010111)},
    {BIN_U8(01011000), BIN_U8(01010101)},
    {BIN_U8(01011010), BIN_U8(01011000)},
    {BIN_U8(01011001), BIN_U8(01011011)},
    {BIN_U8(01011100), BIN_U8(01011001)}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FPGA::LoadSettings()
{
    LoadKoeffCalibration(A);
    LoadKoeffCalibration(B);
    SetAttribChannelsAndTrig(TypeWriteAnalog_All);
    LoadTBase();
    LoadTShift();
    LoadRange(A);
    LoadRShift(A);
    LoadRange(B);
    LoadRShift(B);
    LoadTrigLev();
    LoadTrigPolarity();
    LoadRegUPR();

    switch(BALANCE_ADC_TYPE) 
    {
        case BalanceADC_Settings:
            WriteToHardware(WR_ADD_RSHIFT_DAC1, SET_BALANCE_ADC_A, false);
            WriteToHardware(WR_ADD_RSHIFT_DAC2, SET_BALANCE_ADC_B, false);
            break;
        case BalanceADC_Hand:
            SetPeackDetMode(SET_PEAKDET);
            SetTBase(SET_TBASE);
            if (SET_PEAKDET)
            {
                WriteToHardware(WR_ADD_RSHIFT_DAC1, 3, false);     // Почему-то при пиковом детекторе смещение появляется. Вот его и компенсируем.
                WriteToHardware(WR_ADD_RSHIFT_DAC2, 3, false);
            }
            else
            {
                WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)BALANCE_ADC_A, false);
                WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)BALANCE_ADC_B, false);
            }
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog type) 
{
    uint data = 0;

    /*
    b0...b7 - ChannelA
    b8...b15 - ChannelB
    b16...b23 - Trig
    Передаваться биты должны начиная с b0
    */

    // RangeA, RangeB
    data |= masksRange[SET_RANGE_A];
    data |= (masksRange[SET_RANGE_B] << 8);

    // Параметры каналов
    static const uint maskCouple[2][3] = 
    {
        {0x0008, 0x0000, 0x0030},
        {0x0800, 0x0000, 0x3000}
    };

    data |= maskCouple[A][SET_COUPLE_A];
    data |= maskCouple[B][SET_COUPLE_B];

    static const uint maskFiltr[2][2] = 
    {
        {0x0000, 0x0080},
        {0x0000, 0x0100}
    };

    data |= maskFiltr[A][FILTR_A];
    data |= maskFiltr[B][FILTR_B];


    // Параметры синхронизации
    static const uint maskSource[3] = {0x000000, 0x800000, 0x400000};
    data |= maskSource[TRIGSOURCE];

    static const uint maskInput[4] = {0x000000, 0x030000, 0x020000, 0x010000};
    data |= maskInput[TRIG_INPUT];

    WriteToAnalog(type, data);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetRange(Channel chan, Range range)
{
    if (!sChannel_Enabled(chan))
    {
        return;
    }
    if (range < RangeSize && (int)range >= 0)
    {
        float rShiftAbs = RSHIFT_2_ABS(SET_RSHIFT(chan), SET_RANGE(chan));
        float trigLevAbs = RSHIFT_2_ABS(TRIG_LEVEL(chan), SET_RANGE(chan));
        sChannel_SetRange(chan, range);
        if (LINKING_RSHIFT_IS_VOLTAGE)
        {
            SET_RSHIFT(chan) = RSHIFT_2_REL(rShiftAbs, range);
            TRIG_LEVEL(chan) = RSHIFT_2_REL(trigLevAbs, range);
        }
        LoadRange(chan);
    }
    else
    {
        Display::ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadRange(Channel chan)
{
    SetAttribChannelsAndTrig(TypeWriteAnalog_RangeA);
    LoadRShift(chan);
    if (chan == (Channel)TRIGSOURCE)
    {
        LoadTrigLev();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTBase(TBase tBase)
{
    if (!sChannel_Enabled(A) && !sChannel_Enabled(B))
    {
        return;
    }
    if (tBase < TBaseSize && (int)tBase >= 0)
    {
        float tShiftAbsOld = TSHIFT_2_ABS(TSHIFT, SET_TBASE);
        sTime_SetTBase(tBase);
        LoadTBase();
        FPGA::SetTShift(TSHIFT_2_REL(tShiftAbsOld, SET_TBASE));
        Display::Redraw();
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTBase()
{
    TBase tBase = SET_TBASE;
    uint8 mask = SET_PEAKDET ? masksTBase[tBase].maskPeackDet : masksTBase[tBase].maskNorm;
    FPGA::WriteToHardware(WR_RAZVERTKA, mask, true);
    ADD_SHIFT_T0 = deltaTShift[tBase];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::TBaseDecrease()
{
    if (SET_PEAKDET && SET_TBASE <= MIN_TBASE_PEC_DEAT)
    {
        Display::ShowWarningBad(LimitSweep_Time);
        Display::ShowWarningBad(EnabledPeakDet);
        return;
    }

    if ((int)SET_TBASE > 0)
    {
        if (SELFRECORDER && SET_TBASE == MIN_TBASE_P2P)
        {
            Display::ShowWarningBad(TooFastScanForSelfRecorder);
        }
        else
        {
            TBase base = (TBase)((int)SET_TBASE - 1);
            FPGA::SetTBase(base);
        }
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::TBaseIncrease()
{
    if (SET_TBASE < (TBaseSize - 1))
    {
        TBase base = (TBase)(SET_TBASE + 1);
        FPGA::SetTBase(base);
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetRShift(Channel chan, int16 rShift)
{
    if (!sChannel_Enabled(chan))
    {
        return;
    }
    Display::ChangedRShiftMarkers();

    if (rShift > RShiftMax || rShift < RShiftMin)
    {
        Display::ShowWarningBad(chan == A ? LimitChan1_RShift : LimitChan2_RShift);
    }

    Limitation<int16>(&rShift, RShiftMin, RShiftMax);

    if (rShift > RShiftZero)
    {
        rShift &= 0xfffe;                                            // Делаем кратным двум, т.к. у нас 800 значений на 400 точек
    }
    else if (rShift < RShiftZero)
    {
        rShift = (rShift + 1) & 0xfffe;
    }
    SET_RSHIFT(chan) = rShift;
    LoadRShift(chan);
    Display::RotateRShift(chan);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadRShift(Channel chan)
{
    static const uint16 mask[2] = {0x2000, 0x6000};

    Range range = SET_RANGE(chan);
    ModeCouple mode = SET_COUPLE(chan);
    static const int index[3] = {0, 1, 1};
    int16 rShiftAdd = set.chan[chan].rShiftAdd[range][index[mode]];

    uint16 rShift = SET_RSHIFT(chan) + (SET_INVERSE(chan) ? -1 : 1) * rShiftAdd;

    int16 delta = -(rShift - RShiftZero);
    if (SET_INVERSE(chan))
    {
        delta = -delta;
    }
    rShift = delta + RShiftZero;

    rShift = RShiftMax + RShiftMin - rShift;
    WriteToDAC(chan == A ? TypeWriteDAC_RShiftA : TypeWriteDAC_RShiftB, mask[chan] | (rShift << 2));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigLev(TrigSource chan, int16 trigLev)
{
    Display::ChangedRShiftMarkers();
    if (trigLev < TrigLevMin || trigLev > TrigLevMax)
    {
        Display::ShowWarningBad(LimitSweep_Level);
    }

    Limitation<int16>(&trigLev, TrigLevMin, TrigLevMax);

    if (trigLev > TrigLevZero)
    {
        trigLev &= 0xfffe;
    }
    else if (trigLev < TrigLevZero)
    {
        trigLev = (trigLev + 1) & 0xfffe;
    }

    if (TRIG_LEVEL(chan) != trigLev)
    {
        TRIG_LEVEL(chan) = trigLev;
        LoadTrigLev();
        Display::RotateTrigLev();
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTrigLev()
{
    uint16 data = 0xa000;
    uint16 trigLev = TRIG_LEVEL_SOURCE;
    trigLev = TrigLevMax + TrigLevMin - trigLev;
    data |= trigLev << 2;
    // FPGA_WriteToHardware(WR_DAC_LOW, data.byte[0], true);
    // FPGA_WriteToHardware(WR_DAC_HI, data.byte[1], true);
    WriteToDAC(TypeWriteDAC_TrigLev, data);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTShift(int tShift)
{
    if (!sChannel_Enabled(A) && !sChannel_Enabled(B))
    {
        return;
    }

    if (tShift < sTime_TShiftMin() || tShift > TShiftMax)
    {
        Limitation<int>(&tShift, sTime_TShiftMin(), TShiftMax);
        Display::ShowWarningBad(LimitSweep_TShift);
    }

    sTime_SetTShift(tShift);
    LoadTShift();
    Display::Redraw();
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetDeltaTShift(int16 shift)
{
    deltaTShift[SET_TBASE] = shift;
    LoadTShift();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetPeackDetMode(PeackDetMode peackDetMode)
{
    SET_PEAKDET = peackDetMode;
    LoadRegUPR();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetCalibratorMode(CalibratorMode calibratorMode)
{
    CALIBRATOR = calibratorMode;
    LoadRegUPR();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadRegUPR()
{
    uint8 data = 0;
    if (sTime_RandomizeModeEnabled())
    {
        _SET_BIT(data, 0);
    }
    if (!PEAKDET_IS_DISABLE)
    {
        _SET_BIT(data, 1);
    }
    if (CALIBRATOR_IS_FREQ)
    {
        _SET_BIT(data, 2);
    }
    else if (CALIBRATOR_IS_DC)
    {
        _SET_BIT(data, 3);
    }

    FPGA::WriteToHardware(WR_UPR, data, true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadKoeffCalibration(Channel chan)
{
    FPGA::WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, (uint8)(STRETCH_ADC(chan) * 0x80), false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTShift()
{
    static const int16 k[TBaseSize] = {50, 20, 10, 5, 2};
    int16 tShift = TSHIFT - sTime_TShiftMin() + 1;
    int16 tShiftOld = tShift;
    TBase tBase = SET_TBASE;
    if (tBase < TBase_100ns)
    {
        tShift = tShift / k[tBase] + deltaTShift[tBase];
    }
    int additionShift = (tShiftOld % k[tBase]) * 2;
    FPGA::SetAdditionShift(additionShift);
    uint16 post = tShift;
    post = (~post);
    WriteToHardware(WR_POST_LOW, (uint8)post, true);
    WriteToHardware(WR_POST_HI, (uint8)(post >> 8), true);
    uint16 pred = (tShift > 511) ? 1023 : (511 - post);
    pred = (~(pred - 1)) & 0x1ff;
    WriteToHardware(WR_PRED_LOW, (uint8)pred, true);
    WriteToHardware(WR_PRED_HI, (uint8)(pred >> 8), true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *FPGA::GetTShiftString(int16 tShiftRel, char buffer[20])
{
    float tShiftVal = TSHIFT_2_ABS(tShiftRel, SET_TBASE);
    return Time2String(tShiftVal, true, buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::RangeIncrease(Channel chan)
{
    bool retValue = false;
    if (SET_RANGE(chan) < RangeSize - 1)
    {
        SetRange(chan, (Range)(SET_RANGE(chan) + 1));
        retValue = true;
    }
    else
    {
       Display::ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display::Redraw();
    return retValue;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::RangeDecrease(Channel chan)
{
    bool retValue = false;
    if (SET_RANGE(chan) > 0)
    {
        SetRange(chan, (Range)(SET_RANGE(chan) - 1));
        retValue = true;
    }
    else
    {
        Display::ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display::Redraw();
    return retValue;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigSource(TrigSource trigSource)
{
    TRIGSOURCE = trigSource;
    SetAttribChannelsAndTrig(TypeWriteAnalog_TrigParam);
    if (!TRIGSOURCE_IS_EXT)
    {
        SetTrigLev(TRIGSOURCE, TRIG_LEVEL_SOURCE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigPolarity(TrigPolarity polarity)
{
    TRIG_POLARITY = polarity;
    LoadTrigPolarity();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTrigPolarity()
{
    WriteToHardware(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x01 : 0x00, true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigInput(TrigInput trigInput)
{
    TRIG_INPUT = trigInput;
    SetAttribChannelsAndTrig(TypeWriteAnalog_TrigParam);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetModeCouple(Channel chan, ModeCouple modeCoupe)
{
    SET_COUPLE(chan) = modeCoupe;
    SetAttribChannelsAndTrig(chan == A ? TypeWriteAnalog_ChanParamA : TypeWriteAnalog_ChanParamB);
    SetRShift(chan, SET_RSHIFT(chan));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::EnableChannelFiltr(Channel chan, bool enable)
{
    FILTR(chan) = enable;
    SetAttribChannelsAndTrig(chan == A ? TypeWriteAnalog_ChanParamA : TypeWriteAnalog_ChanParamB);
}
