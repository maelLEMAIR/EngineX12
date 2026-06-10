#ifndef ENGINE_MANAGER_CPP_DEFINED
#define ENGINE_MANAGER_CPP_DEFINED

#include "EngineManager.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"

#include "../Render/Generic/Render.h"
#include "../Render/Generic/Factories/ShaderFactory.hpp"
#include "Systems/TransformSystem.h"


EngineManager* EngineManager::s_pInstance = nullptr;

EngineManager::EngineManager()
{
    s_pInstance = this;
    m_chrono = Chrono();
}

EngineManager::~EngineManager()
{
    s_pInstance->Exit();
}

EngineManager& EngineManager::GetInstance()
{
    if (s_pInstance == nullptr)
        s_pInstance = new EngineManager();

    return *s_pInstance;
}

void EngineManager::Exit()
{
    
}

void EngineManager::Initialize(UINT _width, UINT _height, WString _title, bool _fullscreen)
{
    if (m_pWindow == nullptr)
    {
        m_pWindow = new Window((int)_width, (int)_height, _title);
        m_pWindow->InitD3D12();
        m_pDevice = m_pWindow->GetDevice();
        if ( _fullscreen)
            m_pWindow->ToggleFullScreen();
    }

    m_pDevice->SetClearColor(ToColor(87, 185, 255));
    m_pRessourceManager = new RessourceManager;

    Shader* coloredS = ShaderFactory::CreateLitColored(m_pDevice);
    RessourceManager::AddShader("Color", coloredS);

    Material* white = coloredS->CreateMaterial();
    white->SetFloat4("DiffuseAlbedo", {1.0f, 1.0f, 1.0f, 1.0f});
    RessourceManager::AddMaterial("Default", white);

    InputManager::Initialize(m_pWindow->GetHWND());
    if (m_pSceneManager == nullptr)
        m_pSceneManager = new SceneManager;
}

void EngineManager::Run()
{
    m_chrono.Start();
    
    while ( m_pWindow->IsOpen() )
    {
        m_deltaTime = m_chrono.Reset();

        m_pWindow->Update();
        
        InputManager::Update(m_deltaTime);
        m_pSceneManager->GetCurrentScene()->Update(m_deltaTime);
    }
}

#endif
