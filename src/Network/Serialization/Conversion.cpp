#ifndef CONVERSION_CPP_INCLUDED
#define CONVERSION_CPP_INCLUDED


#ifdef _WIN32
    #pragma comment(lib, "Ws2_32.lib")
    #define NOMINMAX
    #include <WinSock2.h>
#else
    #include <arpa/inet.h>
#endif

#include "Conversion.h"

namespace Serialization
{
    namespace Conversion
    {
        void ToNetwork(uint16 from, uint16& to)
        {
            to = htons(from);
        }
        void ToNetwork(uint32 from, uint32& to)
        {
            to = htonl(from);
        }

        void ToLocal(uint16 from, uint16& to)
        {
            to = ntohs(from);
        }
        void ToLocal(uint32 from, uint32& to)
        {
            to = ntohl(from);
        }

        void ToNetwork(float in, uint32_t& out)
        {
            uint32_t bits;
            memcpy(&bits, &in, sizeof(bits));
            ToNetwork(bits, out);
        }

        void ToLocal(uint32_t in, float& out)
        {
            uint32_t bits;
            ToLocal(in, bits);
            memcpy(&out, &bits, sizeof(out));
        }
    }
}

#endif