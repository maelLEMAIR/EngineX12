#ifndef NETWORK_SOCKET_CPP_INCLUDED
#define NETWORK_SOCKET_CPP_INCLUDED

#include "NetworkSocket.h"

#include <iostream>

bool NetworkSocket::Bind(uint16_t port)
{
#ifdef _WIN32
    WSADATA wsa;
    int wsaResult = WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCK) return false;

    // Mode non bloquant — après création, avant bind
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(m_socket, FIONBIO, &mode);
#endif

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int result = bind(m_socket, (sockaddr*)&addr, sizeof(addr));

    if (result != 0)
        std::cout << "[SOCKET] Erreur: " << WSAGetLastError() << "\n";

    return result == 0;
}

bool NetworkSocket::SendTo(const uint8_t* data, size_t size, const sockaddr_in& dest)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &dest.sin_addr, ip, sizeof(ip));

    int result = sendto(m_socket,
        reinterpret_cast<const char*>(data), (int)size,
        0, (const sockaddr*)&dest, sizeof(dest));
    return result != -1;
}

int NetworkSocket::RecvFrom(uint8_t* buffer, size_t bufSize, sockaddr_in& from)
{
    socklen_t fromLen = sizeof(from);
    return recvfrom(m_socket,
        reinterpret_cast<char*>(buffer), (int)bufSize,
        0, (sockaddr*)&from, &fromLen);
}

void NetworkSocket::Close()
{
#ifdef _WIN32
    closesocket(m_socket);
    WSACleanup();
#else
    close(m_socket);
#endif
}

sockaddr_in NetworkSocket::MakeAddress(const std::string& ip, uint16_t port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    int result = inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    return addr;
}

#endif