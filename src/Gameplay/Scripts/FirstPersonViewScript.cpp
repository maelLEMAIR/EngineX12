#include "FirstPersonViewScript.h"
#include "../Engine/InputManager.h"
#include "../Gameplay/Components/CharacterController.h"

void FirstPersonViewScript::Start(World& world, EntityId self)
{
	m_pitch = 0.0f;
	m_cursorLocked = InputManager::IsMouseCursorLocked();
	m_lastMousePos = InputManager::GetMousePosition();
}

void FirstPersonViewScript::Update(World& world, EntityId self, float deltaTime)
{
	m_cursorLocked = InputManager::IsMouseCursorLocked();
	TransformComponent* transform = world.GetComponent<TransformComponent>(self);
	float mouseSensitivity = 0.01f;
	XMINT2 mousePos = InputManager::GetMousePosition();
	float deltaY = static_cast<float>(mousePos.y - m_lastMousePos.y);
	m_lastMousePos = mousePos;

	m_pitch += deltaY * mouseSensitivity;

	m_pitch = std::clamp(m_pitch, -59.0f * (XM_PI / 180.0f), 89.0f * (XM_PI / 180.0f));

	if(m_cursorLocked)
	{
		XMFLOAT3 ypr(0.0f, m_pitch, 0.0f);
		transform->local.SetYPR(ypr);
	}
}
