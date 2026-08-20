#ifndef NETWORK_BRIDGE_INIT_H_INCLUDED
#define NETWORK_BRIDGE_INIT_H_INCLUDED

#include "ComponentDispatcher.h"
#include "NetworkComponentIndex.h"
#include "Engine/Components/TransformComponent.hpp"
#include "Network/Serialization/Deserialization.h"

namespace NetworkBridge
{
    inline void RegisterComponents()
    {
        NetworkComponentIndex::Get().Register(0x01);
        ComponentDispatcher::Get().Register(0x01,
            [](Serialization::Deserializeration& d, World& world, EntityId id)
            {
                TransformComponent* t = world.GetComponent<TransformComponent>(id);
                if (!t) return;

                Vect3f32 pos, scale;
                Quaternion quat;

                d.read(pos.x);
                d.read(pos.y);
                d.read(pos.z);

                d.read(scale.x);
                d.read(scale.y);
                d.read(scale.z);

                d.read(quat.x);
                d.read(quat.y);
                d.read(quat.z);
                d.read(quat.w);

                t->local.SetPosition(pos);
                t->local.SetScale(scale);
                t->local.SetRotationQuaternion(quat);
            }
        );
    }
}

#endif