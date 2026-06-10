#ifndef SCENE_MANAGER_H_DEFINED
#define SCENE_MANAGER_H_DEFINED

#include "define.h"
#include "EngineManager.h"

class Scene;

class SceneManager
{
public:
	SceneManager();
	static SceneManager& GetInstance() { return *s_pSceneManager; }

	static Scene* GetCurrentScene() { return s_pSceneManager->m_pCurrentScene; }

	static Scene* GetSceneWithName(String const& _name);
	static Scene* GetSceneWithId(uint32 _id);

	static Scene* CreateScene(String const& _name, int32 _id = -1);

	template <typename SceneType>
	static SceneType* CreateSceneType(String const& _name, int32 _id = -1);

	static Scene* SetCurrentScene(Scene* _pScene);
	static Scene* SetCurrentScene(String const& _name);
	static Scene* SetCurrentScene(uint32 _id);

	UnorderedMap<String, uint32>& GetSceneInfos() { return m_sceneIds; }

private:
	inline static SceneManager* s_pSceneManager = nullptr;

	Vector<Scene*> m_scenes;
	UnorderedMap<String, uint32> m_sceneIds;
	
	Scene* m_pCurrentScene = nullptr;
};

#include "SceneManager.inl"

#endif

