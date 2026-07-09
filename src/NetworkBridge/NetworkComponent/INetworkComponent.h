#ifndef INETWORK_COMPONENT_H_INCLUDED
#define INETWORK_COMPONENT_H_INCLUDED

#include "../Network/Serialization/Serialization.h"
#include "../Network/Serialization/Deserialization.h"

struct INetworkComponent
{
    virtual void Serialize  (Serialization::Serializer&      s) const = 0;
    virtual void Deserialize(Serialization::Deserializeration& d)     = 0;
    virtual ~INetworkComponent() = default;
};

#endif