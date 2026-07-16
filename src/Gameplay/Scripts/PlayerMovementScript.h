#pragma once
#include "../Engine/Engine.h"

class PlayerMovementScript : public Script
{
	void Update(World& world, EntityId self, float deltaTime) override;

public:
	void Move(World& world, EntityId self, float deltaTime, const XMFLOAT3& direction, float speed);

private:
	float m_yaw = 0.0f;
};

