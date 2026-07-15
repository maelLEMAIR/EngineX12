#include "PlayerInputScript.h"
#include "../Gameplay/Components/CharacterController.h"
#include "../Engine/InputManager.h"
#include "../Gameplay/Scripts/PlayerMovementScript.h"

void PlayerInputScript::Update(World& world, EntityId self, float deltaTime)
{
	if (world.GetComponent<CharacterController>(self) == nullptr || world.GetComponent<TransformComponent>(self) == nullptr)
		return;

	CharacterController* controller = world.GetComponent<CharacterController>(self);
	TransformComponent* transform = world.GetComponent<TransformComponent>(self);
	PlayerMovementScript* movementScript = world.GetScript<PlayerMovementScript>(self);

	if (InputManager::IsKeyPressed(Q))
		movementScript->Move(world, self, deltaTime, transform->local.right, -controller->walkSpeed);
	if (InputManager::IsKeyPressed(Q) && InputManager::IsKeyPressed((LSHIFT)))
		movementScript->Move(world, self, deltaTime, transform->local.right, -controller->runSpeed);

	if (InputManager::IsKeyPressed(D))
		movementScript->Move(world, self, deltaTime, transform->local.right, controller->walkSpeed);
	if (InputManager::IsKeyPressed(D) && InputManager::IsKeyPressed((LSHIFT)))
		movementScript->Move(world, self, deltaTime, transform->local.right, controller->runSpeed);

	if (InputManager::IsKeyPressed(Z))
		movementScript->Move(world, self, deltaTime, transform->local.forward, controller->walkSpeed);
	if (InputManager::IsKeyPressed(Z) && InputManager::IsKeyPressed((LSHIFT)))
		movementScript->Move(world, self, deltaTime, transform->local.forward, controller->runSpeed);
		
	if (InputManager::IsKeyPressed(S))
		movementScript->Move(world, self, deltaTime, transform->local.forward, -controller->walkSpeed);
	if (InputManager::IsKeyPressed(S) && InputManager::IsKeyPressed((LSHIFT)))
		movementScript->Move(world, self, deltaTime, transform->local.forward, -controller->runSpeed);
		

	if (InputManager::IsKeyDown((ESC)))
	{
		if (isCursorLocked)
		{
			InputManager::UnlockMouseCursor();
			InputManager::ShowMouseCursor();
			isCursorLocked = false;
		}
		else
		{
			InputManager::LockMouseCursor();
			InputManager::HideMouseCursor();
			isCursorLocked = true;
		}
	}

	
}
