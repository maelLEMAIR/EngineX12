#ifndef FIRST_PERSON_VIEW_SCRIPT_H
#define FIRST_PERSON_VIEW_SCRIPT_H

#include "../Engine/Engine.h"

class FirstPersonViewScript : public Script
{
	void Start(World& world, EntityId self) override;
	void Update(World& world, EntityId self, float deltaTime) override;

private:
	float m_pitch;
	bool m_cursorLocked;
	XMINT2 m_lastMousePos = { 0, 0 };
};

#endif // FIRST_PERSON_VIEW_SCRIPT_H

