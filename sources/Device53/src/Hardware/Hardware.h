#pragma once


class Hardware
{
public:
    static void Init();
    static uint CalculateCRC32(uint address = 0x08020000, uint numBytes = 128 * 1024 * 3 / 4);
};
