#ifndef NETWORK_SOCKET_H_INCLUDED
#define NETWORK_SOCKET_H_INCLUDED

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SocketHandle = SOCKET;
#define INVALID_SOCK INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using SocketHandle = int;
#define INVALID_SOCK -1
#endif

#include <string>
#include <cstdint>

class NetworkSocket
{
public:
    bool     Bind(uint16_t port);
    bool     SendTo(const uint8_t* data, size_t size, const sockaddr_in& dest);
    int      RecvFrom(uint8_t* buffer, size_t bufSize, sockaddr_in& from);
    void     Close();

    static sockaddr_in MakeAddress(const std::string& ip, uint16_t port);

private:
    SocketHandle m_socket = INVALID_SOCK;
};

#endif