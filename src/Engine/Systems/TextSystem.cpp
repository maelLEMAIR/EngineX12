#include "TextSystem.h"

#include "EngineManager.h"
#include "RessourceManager.h"

#include "ECS/World.h"
#include "../Components/TransformComponent.hpp"
#include "Components/TextComponent.h"

void TextSystem::Update(World& world, float deltaTime)
{
    Device* pDevice = EngineManager::GetDevice();
    world.Query<TextComponent>([&](TextComponent& _text)
    {
        Text* txt = RessourceManager::GetText(_text.textId);
       if (txt == nullptr) return;

       XMFLOAT4X4 t = _text.transform.GetMatrix();
        
       if (pDevice == nullptr) return;

       pDevice->DrawRenderText(txt, t);
    });
}
