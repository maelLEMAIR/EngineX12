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

// ------------ SYSTEMS -------------
#include "Systems/MeshRendererSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/ScriptSystem.h"

#endif