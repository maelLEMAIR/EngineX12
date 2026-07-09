#ifndef SCENE_CPP_DEFINED
#define SCENE_CPP_DEFINED

#include "Scene.h"
#include "Engine.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Init(String const& _name, uint32 _id)
{
    m_name = _name;
    m_id = _id;

    OnInit();
}

void Scene::Update(float _dt)
{
    OnUpdate(_dt);
    
    Window* window = EngineManager::GetInstance().GetWindow();
    
    if (window != nullptr)
        window->Clear();
    
    world.Update(_dt);
    
    if (window != nullptr)
        window->Display();
}

#endif
