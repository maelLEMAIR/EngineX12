#ifndef TEST_WORLD_HPP_DEFINED
#define TEST_WORLD_HPP_DEFINED

#include "Test.h"
#include "../Engine/Engine.h"
#include "ECS/World.h"

struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Health   { int hp; };

class MovementSystem : public System
{
public:
    void Update(World& world, float deltaTime) override
    {
        world.Query<Position, Velocity>([&](Position& pos, Velocity& vel)
        {
            pos.x += vel.dx * deltaTime;
            pos.y += vel.dy * deltaTime;
        });
    }
};

class CombatSystem : public System
{
public:
    void Update(World& world, float deltaTime) override
    {
        world.QueryWithEntity<Health>([&](EntityId id, Health& hp)
        {
            hp.hp -= 10;
            if (hp.hp <= 0)
            {
                world.DestroyEntity(id);
            }
        });
    }
};

class SpawnSystem : public System
{
public:
    int  m_spawnCount = 0;
    bool m_shouldSpawn = false;

    void Update(World& world, float deltaTime) override
    {
        if (!m_shouldSpawn) return;
        m_shouldSpawn = false;

        world.Query<Position>([&](Position& pos)
        {
            EntityId newE = world.CreateEntity();
            world.AddComponent<Position>(newE) = { pos.x + 1.0f, pos.y };
            m_spawnCount++;
        });
    }
};

class TestWorld : public Test
{
public: 
    static void Run()
    {
        World world;

        world.RegisterSystem<MovementSystem>(0);
        world.RegisterSystem<CombatSystem>(1);
        SpawnSystem* spawnSys = world.RegisterSystem<SpawnSystem>(2);

        int positionsAdded   = 0;
        int positionsRemoved = 0;
        int entitiesCreated  = 0;
        int entitiesDestroyed = 0;

        ObserverId obsAdd = world.OnComponentAdded<Position>(
            [&](World& w, EntityId id) {
                positionsAdded++;
            }
        );

        ObserverId obsRem = world.OnComponentRemoved<Health>(
            [&](World& w, EntityId id) {
                positionsRemoved++;
            }
        );

        ObserverId obsCreate = world.OnEntityCreated(
            [&](World& w, EntityId id) {
                entitiesCreated++;
            }
        );

        ObserverId obsDestroy = world.OnEntityDestroyed(
            [&](World& w, EntityId id) {
                entitiesDestroyed++;
            }
        );

        EntityId e1 = world.CreateEntity();
        world.AddComponent<Position>(e1) = { 0.0f, 0.0f };
        world.AddComponent<Velocity>(e1) = { 1.0f, 0.5f };
        world.AddComponent<Health>(e1)   = { 30 };

        EntityId e2 = world.CreateEntity();
        world.AddComponent<Position>(e2) = { 5.0f, 5.0f };
        world.AddComponent<Velocity>(e2) = { -1.0f, 0.0f };
        world.AddComponent<Health>(e2)   = { 50 };

        EntityId e3 = world.CreateEntity();
        world.AddComponent<Position>(e3) = { 2.0f, 2.0f };

        assert(entitiesCreated == 3);
        assert(positionsAdded  == 3);

        for (int i = 0; i < 3; i++)
            world.Update(0.016f);

        assert(!world.m_entityManager.IsAlive(e1));
        assert(entitiesDestroyed == 1);

        assert(world.m_entityManager.IsAlive(e2));
        Health* hp2 = world.GetComponent<Health>(e2);
        assert(hp2 != nullptr && hp2->hp == 20);

        EntityId e4 = world.CreateEntity();
        world.AddComponent<Position>(e4) = { 99.0f, 0.0f };

        assert(GetEntityIndex(e4) == GetEntityIndex(e1));
        assert(GetEntityVersion(e4) != GetEntityVersion(e1));

        assert(world.GetComponent<Position>(e1) == nullptr);

        int healthRemovedBefore = positionsRemoved;
        world.RemoveComponent<Health>(e2);
        assert(positionsRemoved == healthRemovedBefore + 1);
        assert(world.GetComponent<Health>(e2) == nullptr);

        world.RemoveObserver(obsAdd);
        int positionsAddedBefore = positionsAdded;

        EntityId e5 = world.CreateEntity();
        world.AddComponent<Position>(e5) = { 1.0f, 1.0f };

        assert(positionsAdded == positionsAddedBefore);

        spawnSys->m_shouldSpawn = true;

        world.Update(0.016f);

        assert(spawnSys->m_spawnCount > 0);

        world.SetSystemActive<MovementSystem>(false);
        world.Update(0.016f);

        world.SetSystemActive<MovementSystem>(true);
    }
};

#endif