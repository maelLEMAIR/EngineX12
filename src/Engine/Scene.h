#ifndef SCENE_H_DEFINED
#define SCENE_H_DEFINED

#include "define.h"
#include "ECS/World.h"

class Scene
{
public:
	Scene();
	
	String GetName() { return m_name; }
	uint32 GetId() { return m_id; }

	virtual ~Scene();
	
protected:
	virtual void OnInit() {}
	virtual void OnUpdate(float _dt) {}
	virtual void OnStart() {}
	virtual void OnEnd() {}
	virtual void LoadRessources() {}

	World world;
	
private:
	String m_name;
	uint32 m_id;

	
	void Init(String const& _name, uint32 _id);
	void Update(float _dt);

	friend class EngineManager;
	friend class SceneManager; 
};

#endif