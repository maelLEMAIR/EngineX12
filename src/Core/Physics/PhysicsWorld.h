#ifndef PHYSICS_WORLD_H_INCLUDED
#define PHYSICS_WORLD_H_INCLUDED

#include <thread>
#include <atomic>

#include "../define.h"
#include "../Chrono.h"
#include "RigidBody.hpp"
#include "../Math/Geometry/Manifold.hpp"

class AABB;

// Thread-safety : CreateBody/DestroyBody/GetSnapshot verrouillent directement
// le pool de corps (opérations peu fréquentes). ApplyForce/ApplyImpulse/
// SetLinearVelocity/SetPosition passent par une file de commandes 
// drainée en début de Step() sur le thread physique, pour ne
// jamais bloquer le thread appelant sur le verrou du pool 
// pendant une étape de simulation.

class PhysicsWorld
{
public:
    struct BodyHandle
    {
        uint32 index      = 0;
        uint32 generation = 0;

        bool IsValid() const { return generation != 0; }
        bool operator==(BodyHandle const& _o) const { return index == _o.index && generation == _o.generation; }
        bool operator!=(BodyHandle const& _o) const { return !(*this == _o); }
    };

    struct BodySnapshot
    {
        BodyHandle handle;
        Vect3f32   position;
        Vect3f32   linearVelocity;
    };

    ~PhysicsWorld();

    void Start(float _tickRate = 60.0f);
    void Stop();
    bool IsRunning() const { return m_running; }

    BodyHandle CreateBody(RigidBody const& _desc);
    void       DestroyBody(BodyHandle _handle);

    void ApplyForce(BodyHandle _handle, Vect3f32 const& _force);
    void ApplyImpulse(BodyHandle _handle, Vect3f32 const& _impulse);
    void SetLinearVelocity(BodyHandle _handle, Vect3f32 const& _velocity);
    void SetPosition(BodyHandle _handle, Vect3f32 const& _position);

    void SetGravity(Vect3f32 const& _gravity);

    // Copie {handle, position, vitesse} de tous les corps vivants sous verrou.
    // A appeler une fois par frame depuis le thread principal.
    void GetSnapshot(Vector<BodySnapshot>& _out);

private:
    struct BodySlot
    {
        RigidBody body;
        uint32    generation = 1;
        bool      alive      = false;
    };

    enum class CommandType
    {
        ApplyForce,
        ApplyImpulse,
        SetVelocity,
        SetPosition
    };

    struct Command
    {
        CommandType type;
        BodyHandle  handle;
        Vect3f32    value;
    };

    void PhysicsLoop();
    void Step(float _dt);
    void DrainCommands();
    void PushCommand(CommandType _type, BodyHandle _handle, Vect3f32 const& _value);

    RigidBody* GetBodyUnlocked(BodyHandle _handle);

    bool NarrowPhase(RigidBody const& _a, RigidBody const& _b, Manifold& _outManifold) const;
    void ResolveContact(RigidBody& _a, RigidBody& _b, Manifold const& _manifold) const;
    AABB MakeBroadphaseAABB(RigidBody const& _body) const;

    Vector<BodySlot> m_bodies;
    Vector<uint32>   m_freeList;
    Mutex            m_bodiesMutex;

    Queue<Command> m_commands;
    Mutex          m_commandMutex;

    Vect3f32 m_gravity = Vect3f32(0.0f, -9.81f, 0.0f);

    std::thread       m_thread;
    std::atomic<bool> m_running = false;
    float             m_tickRate = 60.0f;
};

#endif
