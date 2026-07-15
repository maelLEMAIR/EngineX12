#pragma once
#include "../Engine/Engine.h"

class PlayerInputScript : public Script
{
	void Update(World& world, EntityId self, float deltaTime) override;

private:

	bool isCursorLocked = false;
};

