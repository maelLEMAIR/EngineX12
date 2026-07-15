#pragma once
struct CharacterController
{
	float walkSpeed = 3.0f;
	float runSpeed = 6.0f;
	float jumpHeight = 2.0f;
	float crouchHeight = 1.0f;
	float crounchSpeed = 1.5f;

	bool isCrouching = false;
	bool isRunning = false;

	float mouseSensitivity = 0.01f;
};

