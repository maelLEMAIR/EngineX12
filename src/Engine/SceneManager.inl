#ifndef SCENE_MANAGER_INL_DEFINED
#define SCENE_MANAGER_INL_DEFINED 


template <typename SceneType>
SceneType* SceneManager::CreateSceneType(String const& _name, int32 _id)
{
	assert(_name.size() < 25 && "Scene name is too big");
	
	SceneType* pNewScene = new SceneType();

	uint32 id;
	if (_id == -1)
		id = s_pSceneManager->m_scenes.size();
	else
		id = (uint32)_id;

	s_pSceneManager->m_sceneIds[_name] = id;
	s_pSceneManager->m_scenes.push_back(pNewScene);

	pNewScene->LoadRessources();
	pNewScene->Init(_name, id);

	
	return pNewScene;
}

#endif