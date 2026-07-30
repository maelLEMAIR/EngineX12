#ifndef ENGINE_H_DEFINED
#define ENGINE_H_DEFINED

// -------------- ECS ---------------
#include "ECS/World.h"
#include "ECS/EntityManager.h"

// ------------- ENGINE -------------
#include "EngineManager.h"
#include "RessourceManager.h"
#include "SceneManager.h"
#include "Scene.h"

// ----------- COMPONENTS -----------
#include "Components/MeshRenderer.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/RigidBodyComponent.hpp"

// ------------ SYSTEMS -------------
#include "Systems/MeshRendererSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/ScriptSystem.h"
#include "Systems/PhysicsSystem.h"

#endif