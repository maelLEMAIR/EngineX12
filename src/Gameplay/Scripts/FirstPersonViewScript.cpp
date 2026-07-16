#include "FirstPersonViewScript.h"
#include "../Engine/InputManager.h"
#include "../Gameplay/Components/CharacterController.h"

void FirstPersonViewScript::Start(World& world, EntityId self)
{
	m_pitch = 0.0f;
	m_cursorLocked = InputManager::IsMouseCursorLocked();
}

void FirstPersonViewScript::Update(World& world, EntityId self, float deltaTime)
{
	m_cursorLocked = InputManager::IsMouseCursorLocked();
	TransformComponent* transform = world.GetComponent<TransformComponent>(self);
	float mouseSensitivity = 0.01f;

	if (m_cursorLocked)
	{
		XMINT2 delta = InputManager::GetMouseDelta();

		m_pitch += static_cast<float>(delta.y) * mouseSensitivity;
		m_pitch = std::clamp(m_pitch, -59.0f * (XM_PI / 180.0f), 89.0f * (XM_PI / 180.0f));

		XMFLOAT3 ypr(0.0f, m_pitch, 0.0f);
		transform->local.SetYPR(ypr);
	}
}
