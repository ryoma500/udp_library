#include "udp_library.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

static SOCKET g_sock = INVALID_SOCKET;
static sockaddr_in g_target;
static int g_initialized = 0;

static SOCKET g_recvSock = INVALID_SOCKET;
static char* g_recvBuffer = nullptr;
static int g_recvBufferSize = 0;
static udp_receive_callback g_cb = nullptr;
static int g_recvRunning = 0;

int udp_library_init(const char* ip, int port)
{
    if (g_initialized) return 1;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 0;

    g_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_sock == INVALID_SOCKET)
        return 0;

    g_target.sin_family = AF_INET;
    g_target.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &g_target.sin_addr) != 1)
        return 0;

    g_initialized = 1;
    return 1;
}

void udp_library_send(const void* data, int size)
{
    if (!g_initialized) return;

    sendto(
        g_sock,
        (const char*)data,
        size,
        0,
        (sockaddr*)&g_target,
        sizeof(g_target)
    );
}

void udp_library_close()
{
    if (!g_initialized) return;

    closesocket(g_sock);
    WSACleanup();

    g_sock = INVALID_SOCKET;
    g_initialized = 0;
}

unsigned __stdcall recv_thread(void*)
{
    sockaddr_in from;
    int fromlen = sizeof(from);

    while (g_recvRunning)
    {
        int len = recvfrom(
            g_recvSock,
            g_recvBuffer,
            g_recvBufferSize,
            0,
            (sockaddr*)&from,
            &fromlen
        );

        if (!g_recvRunning) break;

        if (len > 0)
        {
            if (len > g_recvBufferSize)
            {
                printf("[UDP WARNING] buffer overflow: recv=%d buffer=%d\n",
                       len, g_recvBufferSize);

                len = g_recvBufferSize;
            }

            // callbackは任意
            if (g_cb)
            {
                g_cb(g_recvBuffer, len);
            }
        }
    }

    return 0;
}

int udp_library_start_receive(int port,
                              char* buffer,
                              int buffer_size,
                              udp_receive_callback cb)
{
    if (g_recvRunning) return 1;

    g_recvBuffer = buffer;
    g_recvBufferSize = buffer_size;
    g_cb = cb;

    g_recvSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_recvSock == INVALID_SOCKET)
        return 0;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(g_recvSock, (sockaddr*)&addr, sizeof(addr)) != 0)
    {
        closesocket(g_recvSock);
        g_recvSock = INVALID_SOCKET;
        return 0;
    }

    g_recvRunning = 1;

    _beginthreadex(NULL, 0, recv_thread, NULL, 0, NULL);

    return 1;
}

void udp_library_stop_receive()
{
    if (!g_recvRunning) return;

    g_recvRunning = 0;

    closesocket(g_recvSock);
    g_recvSock = INVALID_SOCKET;

    g_cb = nullptr;
    g_recvBuffer = nullptr;
}