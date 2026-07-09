#ifndef NETWORK_FLAG_H_INCLUDED
#define NETWORK_FLAG_H_INCLUDED

#include "Core/define.h"
#include <bitset>

constexpr size_t MAX_NETWORK_COMPONENTS = 32;

struct DirtyFlag
{
    std::bitset<MAX_NETWORK_COMPONENTS> bits;

    void Mark  (uint32 componentIndex)          { bits.set(componentIndex); }
    void Clear (uint32 componentIndex)          { bits.reset(componentIndex); }
    void ClearAll()                             { bits.reset(); }
    bool IsDirty(uint32 componentIndex) const   { return bits.test(componentIndex); }
    bool AnyDirty() const                       { return bits.any(); }
};

#endif