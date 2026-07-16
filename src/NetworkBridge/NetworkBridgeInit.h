#ifndef NETWORK_BRIDGE_INIT_H_INCLUDED
#define NETWORK_BRIDGE_INIT_H_INCLUDED

#include "NetworkComponentIndex.h"
#include "Engine/Components/TransformComponent.hpp"
#include "Network/Serialization/Deserialization.h"

namespace NetworkBridge
{
    /*inline void RegisterComponents()
    {
        NetworkComponentIndex::Get().Register(0x01);
        ComponentDispatcher::Get().Register(0x01,
            [](Serialization::Deserializeration& d, World& world, EntityId id)
            {
                TransformComponent* t = world.GetComponent<TransformComponent>(id);
                if (!t) return;

                d.read(t->local.pos.x);
                d.read(t->local.pos.y);
                d.read(t->local.pos.z);

                d.read(t->local.scale.x);
                d.read(t->local.scale.y);
                d.read(t->local.scale.z);

                d.read(t->local.quat.x);
                d.read(t->local.quat.y);
                d.read(t->local.quat.z);
                d.read(t->local.quat.w);

                t->local.UpdateRotationFromQuaternion();
                t->local.UpdateMatrix();
            }
        );
    }*/
}

#endif