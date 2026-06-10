#ifndef TEST_SCRIPT_HPP_DEFINED
#define TEST_SCRIPT_HPP_DEFINED

#include "Test.h"
#include "../Engine/Engine.h"
#include "ECS/World.h"

class PlayerController : public Script
{
public:
    void Start(World& world, EntityId self) override
    {
        std::cout << "PlayerController Start sur e"
                  << GetEntityIndex(self) << "\n";
        world.AddComponent<Velocity>(self) = { 1.0f, 0.0f };
    }

    void Update(World& world, EntityId self, float deltaTime) override
    {
        Position* pos = world.GetComponent<Position>(self);
        Velocity* vel = world.GetComponent<Velocity>(self);
        if (pos && vel)
            std::cout << "Player e" << GetEntityIndex(self)
                      << " pos(" << pos->x << ", " << pos->y << ")\n";
    }

    void OnDestroy(World& world, EntityId self) override
    {
        std::cout << "PlayerController OnDestroy sur e"
                  << GetEntityIndex(self) << "\n";
    }
};

class HealthRegen : public Script
{
public:
    void Update(World& world, EntityId self, float deltaTime) override
    {
        Health* hp = world.GetComponent<Health>(self);
        if (hp)
            hp->hp += 1;
    }
};


class TestScript : public Test
{
public: 
    static void Run()
    {
        World world;
        world.RegisterSystem<MovementSystem>(0);

        EntityId e1 = world.CreateEntity();
        world.AddComponent<Position>(e1) = { 0.0f, 0.0f };
        world.AddComponent<Health>(e1)   = { 50 };
        world.AddScript<PlayerController>(e1);
        world.AddScript<HealthRegen>(e1);

        world.Update(0.016f);
        world.Update(0.016f);

        world.SetScriptActive<HealthRegen>(e1, false);
        world.Update(0.016f);

        Health* hp = world.GetComponent<Health>(e1);
        std::cout << "HP après 3 frames (2 regens) : " << hp->hp << " (attendu 52)\n";
        assert(hp->hp == 52);
        
        world.DestroyEntity(e1);

        std::cout << "\n✅ Scripts OK\n";
    }
};

#endif