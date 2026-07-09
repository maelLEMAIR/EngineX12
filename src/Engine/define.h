#ifndef ENGINE_DEFINE_H_DEFINED
#define ENGINE_DEFINE_H_DEFINED

#include "Core/define.h"

/*#include <WS2tcpip.h>
#include <winsock2.h>*/

#include <bitset>
#include <cassert>

using namespace std;

#define MAX_COMPONENTS 256


using ComponentMask     = std::bitset<MAX_COMPONENTS>;
using ComponentId       = uint32_t;
using ColumnIndex       = uint32_t;
using ObserverId        = uint32_t;
using EntityId          = uint64_t;

inline uint32_t GetEntityIndex  (EntityId id) { return (uint32_t)(id >> 32); }
inline uint32_t GetEntityVersion(EntityId id) { return (uint32_t)(id & 0xFFFFFFFF); }
inline EntityId MakeEntityId(uint32_t index, uint32_t version)
{
    return ((uint64_t)index << 32) | (uint64_t)version;
}

constexpr EntityId INVALID_ENTITY = (std::numeric_limits<EntityId>::max)();

#endif
