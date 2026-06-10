#include "SceneManager.h"

#include "EngineManager.h"
#include "Scene.h"

SceneManager::SceneManager()
{
	s_pSceneManager = this;

	Scene* pDefaultScene = CreateScene("Default");

	m_pCurrentScene = pDefaultScene;
}

Scene* SceneManager::GetSceneWithName(String const& _name)
{
	if (s_pSceneManager->m_sceneIds.contains(_name))
	{
		return s_pSceneManager->m_scenes[s_pSceneManager->m_sceneIds[_name]];
	}

	return nullptr;
}

Scene* SceneManager::GetSceneWithId(uint32 _id)
{
	return s_pSceneManager->m_scenes[_id];
}

Scene* SceneManager::CreateScene(String const& _name, int32 _id)
{
	assert(_name.size() < 25 && "Scene name is too big");

	if (GetSceneWithName(_name) != nullptr)
		return GetSceneWithName(_name);
	
	Scene* pNewScene = new Scene();

	uint32 id;
	if (_id == -1)
		id = (uint32)s_pSceneManager->m_scenes.size();
	else
		id = (uint32)_id;
	
	s_pSceneManager->m_sceneIds[_name] = id;
	s_pSceneManager->m_scenes.push_back(pNewScene);

	pNewScene->LoadRessources();
	pNewScene->Init(_name, id);
	
	return pNewScene;
}

Scene* SceneManager::SetCurrentScene(Scene* _pScene)
{
	if (_pScene == nullptr)
		return s_pSceneManager->m_pCurrentScene;
	if (s_pSceneManager->GetSceneWithName(_pScene->GetName()) == nullptr)
		s_pSceneManager->m_pCurrentScene;

	s_pSceneManager->m_pCurrentScene->OnEnd();
	s_pSceneManager->m_pCurrentScene = _pScene;
	s_pSceneManager->m_pCurrentScene->OnStart();

	return s_pSceneManager->m_pCurrentScene;
}

Scene* SceneManager::SetCurrentScene(String const& _name)
{
	return SetCurrentScene(s_pSceneManager->GetSceneWithName(_name));
}

Scene* SceneManager::SetCurrentScene(uint32 _id)
{
	return SetCurrentScene(s_pSceneManager->GetSceneWithId(_id));
}

