#include "PlayerMovementScript.h"
#include "../Gameplay/Components/CharacterController.h"
#include "../Engine/InputManager.h"

void PlayerMovementScript::Update(World& world, EntityId self, float deltaTime)
{
	if (world.GetComponent<CharacterController>(self) == nullptr || world.GetComponent<TransformComponent>(self) == nullptr)
		return;

	float mouseSensitivity = world.GetComponent<CharacterController>(self)->mouseSensitivity;
	XMINT2 mousePos = InputManager::GetMousePosition();
	float deltaX = static_cast<float>(mousePos.x - m_lastMousePos.x);
	m_lastMousePos = mousePos;

	m_yaw += deltaX * mouseSensitivity;
	if (InputManager::IsMouseCursorLocked() == true)
		world.GetComponent<TransformComponent>(self)->local.SetYPR(XMFLOAT3(m_yaw, 0.0f, 0.0f));

}

void PlayerMovementScript::Move(World& world, EntityId self, float deltaTime, const XMFLOAT3& direction, float speed)
{
	world.GetComponent<TransformComponent>(self)->local.Move(direction, speed * deltaTime);
}
