#ifndef CONVERSION_H_INCLUDED
#define CONVERSION_H_INCLUDED

#include "Core/define.h"

namespace Serialization
{
    namespace Conversion
    {
        void ToNetwork(uint16 in, uint16& out);
        void ToNetwork(uint32 in, uint32& out);
        void ToNetwork(float32 in, uint32& out);

        void ToLocal(uint16 in, uint16& out);
        void ToLocal(uint32 in, uint32& out);
        void ToLocal(uint32 in, float32& out);
    }
}

#endif