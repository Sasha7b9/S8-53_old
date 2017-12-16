#pragma once

static const int    DEFAULT_PORT = 7,
                    POLICY_PORT = 843;


class SocketTCP
{
public:
    static bool Init(void (*funcConnect)(), void (*funcReciever)(const char *buffer, uint length));

    static bool Send(const char *buffer, uint length);

    static void SendFormatString(char *format, ...);
};

void ETH_SendFormatString(char *format, ...);


extern bool gEthIsConnected;
