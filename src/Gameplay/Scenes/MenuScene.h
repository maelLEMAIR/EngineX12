#pragma once
#include "../Engine/Engine.h"

class MenuScene : public Scene
{

public:
	void OnInit() override;
	void OnUpdate(float _dt) override;
	void OnStart() override;
	void OnEnd() override;
	void LoadRessources() override;

private:
	vector<EntityId> m_entities;
};

