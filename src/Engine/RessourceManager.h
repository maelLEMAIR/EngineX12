#ifndef RESSOURCE_MANAGER_H_DEFINED
#define RESSOURCE_MANAGER_H_DEFINED

#include "define.h"
#include "../Render/Generic/Render.h"

class RessourceManager
{
public:
	RessourceManager();
	~RessourceManager();

	// TODO RAW DATA STORAGE
	
	static uint32 AddGeometry(String _name, Geometry* _pGeo);
	static uint32 GetGeometryId(String _name) { return s_pInstance->m_mGeometryIds[_name]; };
	static Geometry* GetGeometry(String _name);
	static Geometry* GetGeometry(uint32 _id) { return s_pInstance->m_vGeometries[_id]; }
	
	static uint32 AddSprite(String _name, Sprite* _pSprite);
	static uint32 GetSpriteId(String _name) { return s_pInstance->m_mSpriteIds[_name]; };
	static Sprite* GetSprite(String _name);
	static Sprite* GetSprite(uint32 _id) { return s_pInstance->m_vSprites[_id]; }

	static uint32 AddMaterial(String _name, Material* pMat);
	static uint32 GetMaterialId(String _name) { return s_pInstance->m_mMaterialIds[_name]; };
	static Material* GetMaterial(String _name);
	static Material* GetMaterial(uint32 _id) { return s_pInstance->m_vMaterials[_id]; }

	static uint32 AddShader(String _name, Shader* _pShader);
	static uint32 GetShaderId(String _name) { return s_pInstance->m_mShaderIds[_name]; };
	static Shader* GetShader(String _name);
	static Shader* GetShader(uint32 _id) { return s_pInstance->m_vShaders[_id]; }
	
	static uint32 AddUiMaterial(String _name, UiMaterial* pMat);
	static uint32 GetUiMaterialId(String _name) { return s_pInstance->m_mUiMaterialIds[_name]; };
	static UiMaterial* GetUiMaterial(String _name);
	static UiMaterial* GetUiMaterial(uint32 _id) { return s_pInstance->m_vUiMaterials[_id]; }

	static uint32 AddUiShader(String _name, UiShader* _pUiShader);
	static uint32 GetUiShaderId(String _name) { return s_pInstance->m_mUiShaderIds[_name]; };
	static UiShader* GetUiShader(String _name);
	static UiShader* GetUiShader(uint32 _id) { return s_pInstance->m_vUiShaders[_id]; }

	static uint32 AddFont(String _name, RenderFont* _pFont);
	static uint32 GetFontId(String _name) { return s_pInstance->m_mFontIds[_name]; };
	static RenderFont* GetFont(String _name);
	static RenderFont* GetFont(uint32 _id) { return s_pInstance->m_vFonts[_id]; }
	
	static uint32 AddText(String _name, Text* _pText);
	static uint32 GetTextId(String _name) { return s_pInstance->m_mTextIds[_name]; };
	static Text* GetText(String _name);
	static Text* GetText(uint32 _id) { return s_pInstance->m_vTexts[_id]; }

	static uint32 AddTexture(String _name, Texture* _pTexture);
	static uint32 GetTextureId(String _name) { return s_pInstance->m_mTextureIds[_name]; };
	static Texture* GetTexture(String _name);
	static Texture* GetTexture(uint32 _id) { return s_pInstance->m_vTextures[_id]; }

	static uint32 AddCamera(String _name);
	static uint32 GetCameraId(String _name) { return s_pInstance->m_mCameraIds[_name]; };
	static Camera* GetCamera(String _name);
	static Camera* GetCamera(uint32 _id) { return s_pInstance->m_vCameras[_id]; }

private:
	static RessourceManager* s_pInstance;

	Vector<Geometry*> m_vGeometries;
	Vector<Sprite*> m_vSprites;
	Vector<Shader*> m_vShaders;
	Vector<UiShader*> m_vUiShaders;
	Vector<Material*> m_vMaterials;
	Vector<UiMaterial*> m_vUiMaterials;
	Vector<RenderFont*> m_vFonts;
	Vector<Text*> m_vTexts;
	Vector<Texture*> m_vTextures;
	Vector<Camera*> m_vCameras;
	
	UnorderedMap<String, uint32> m_mGeometryIds;
	UnorderedMap<String, uint32> m_mSpriteIds;
	UnorderedMap<String, uint32> m_mShaderIds;
	UnorderedMap<String, uint32> m_mUiShaderIds;
	UnorderedMap<String, uint32> m_mMaterialIds;
	UnorderedMap<String, uint32> m_mUiMaterialIds;
	UnorderedMap<String, uint32> m_mFontIds;
	UnorderedMap<String, uint32> m_mTextIds;
	UnorderedMap<String, uint32> m_mTextureIds;
	UnorderedMap<String, uint32> m_mCameraIds;
};

#endif