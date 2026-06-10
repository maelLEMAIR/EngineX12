#ifndef RESSOURCE_MANAGER_CPP_DEFINED
#define RESSOURCE_MANAGER_CPP_DEFINED

#include "RessourceManager.h"

RessourceManager* RessourceManager::s_pInstance = nullptr;

RessourceManager::RessourceManager()
{
    s_pInstance = this;
}

RessourceManager::~RessourceManager()
{
}

uint32 RessourceManager::AddGeometry(String _name, Geometry* _pGeo)
{
    assert(s_pInstance->m_mGeometryIds.contains(_name) == false && "Geometry already exists");
    
    s_pInstance->m_vGeometries.push_back(_pGeo);
    s_pInstance->m_mGeometryIds[_name] = s_pInstance->m_vGeometries.size() - 1;
    return s_pInstance->m_mGeometryIds[_name];
}

Geometry* RessourceManager::GetGeometry(String _name)
{
    if (s_pInstance->m_mGeometryIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vGeometries[s_pInstance->m_mGeometryIds[_name]];
}

uint32 RessourceManager::AddSprite(String _name, Sprite* _pSprite)
{
    assert(s_pInstance->m_mSpriteIds.contains(_name) == false && "Sprite already exists");
    
    s_pInstance->m_vSprites.push_back(_pSprite);
    s_pInstance->m_mSpriteIds[_name] = s_pInstance->m_vSprites.size() - 1;
    return s_pInstance->m_mSpriteIds[_name];
}

Sprite* RessourceManager::GetSprite(String _name)
{
    if (s_pInstance->m_mSpriteIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vSprites[s_pInstance->m_mSpriteIds[_name]];
}

uint32 RessourceManager::AddShader(String _name, Shader* _pShader)
{
    assert(s_pInstance->m_mShaderIds.contains(_name) == false && "Shader already exists");
    
    s_pInstance->m_vShaders.push_back(_pShader);
    s_pInstance->m_mShaderIds[_name] = s_pInstance->m_vShaders.size() - 1;
    return s_pInstance->m_mShaderIds[_name];
}

Shader* RessourceManager::GetShader(String _name)
{
    if (s_pInstance->m_mShaderIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vShaders[s_pInstance->m_mShaderIds[_name]];
}

uint32 RessourceManager::AddUiMaterial(String _name, UiMaterial* pMat)
{
    assert(s_pInstance->m_mUiMaterialIds.contains(_name) == false && "Material already exists");
    
    s_pInstance->m_vUiMaterials.push_back(pMat);
    s_pInstance->m_mUiMaterialIds[_name] = s_pInstance->m_vUiMaterials.size() - 1;
    return s_pInstance->m_mUiMaterialIds[_name];
}

UiMaterial* RessourceManager::GetUiMaterial(String _name)
{
    if (s_pInstance->m_mUiMaterialIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vUiMaterials[s_pInstance->m_mUiMaterialIds[_name]];
}

uint32 RessourceManager::AddUiShader(String _name, UiShader* _pUiShader)
{
    assert(s_pInstance->m_mUiShaderIds.contains(_name) == false && "UiShader already exists");
    
    s_pInstance->m_vUiShaders.push_back(_pUiShader);
    s_pInstance->m_mUiShaderIds[_name] = s_pInstance->m_vUiShaders.size() - 1;
    return s_pInstance->m_mUiShaderIds[_name];
}

UiShader* RessourceManager::GetUiShader(String _name)
{
    if (s_pInstance->m_mUiShaderIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vUiShaders[s_pInstance->m_mUiShaderIds[_name]];
}

uint32 RessourceManager::AddFont(String _name, RenderFont* _pFont)
{
    assert(s_pInstance->m_mFontIds.contains(_name) == false && "Font already exists");
    
    s_pInstance->m_vFonts.push_back(_pFont);
    s_pInstance->m_mFontIds[_name] = s_pInstance->m_vFonts.size() - 1;
    return s_pInstance->m_mFontIds[_name];
}

RenderFont* RessourceManager::GetFont(String _name)
{
    if (s_pInstance->m_mFontIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vFonts[s_pInstance->m_mFontIds[_name]];
}

uint32 RessourceManager::AddText(String _name, Text* _pText)
{
    assert(s_pInstance->m_mTextIds.contains(_name) == false && "Text already exists");
    
    s_pInstance->m_vTexts.push_back(_pText);
    s_pInstance->m_mTextIds[_name] = s_pInstance->m_vTexts.size() - 1;
    return s_pInstance->m_mTextIds[_name];
}

Text* RessourceManager::GetText(String _name)
{
    if (s_pInstance->m_mTextIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vTexts[s_pInstance->m_mTextIds[_name]];
}

uint32 RessourceManager::AddTexture(String _name, Texture* _pTexture)
{
    assert(s_pInstance->m_mTextureIds.contains(_name) == false && "Texture already exists");
    
    s_pInstance->m_vTextures.push_back(_pTexture);
    s_pInstance->m_mTextureIds[_name] = s_pInstance->m_vTextures.size() - 1;
    return s_pInstance->m_mTextureIds[_name];
}

Texture* RessourceManager::GetTexture(String _name)
{
    if (s_pInstance->m_mTextureIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vTextures[s_pInstance->m_mTextureIds[_name]];
}

uint32 RessourceManager::AddCamera(String _name)
{
    assert(s_pInstance->m_mCameraIds.contains(_name) == false && "Camera already exists");
    
    s_pInstance->m_vCameras.push_back(new Camera());
    s_pInstance->m_mCameraIds[_name] = s_pInstance->m_vCameras.size() - 1;
    return s_pInstance->m_mCameraIds[_name];
}

Camera* RessourceManager::GetCamera(String _name)
{
    if (s_pInstance->m_mCameraIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vCameras[s_pInstance->m_mCameraIds[_name]];
}

uint32 RessourceManager::AddMaterial(String _name, Material* pMat)
{
    assert(s_pInstance->m_mMaterialIds.contains(_name) == false && "Material already exists");
    
    s_pInstance->m_vMaterials.push_back(pMat);
    s_pInstance->m_mMaterialIds[_name] = s_pInstance->m_vMaterials.size() - 1;
    return s_pInstance->m_mMaterialIds[_name];
}

Material* RessourceManager::GetMaterial(String _name)
{
    if (s_pInstance->m_mMaterialIds.contains(_name) == false) return nullptr;

    return s_pInstance->m_vMaterials[s_pInstance->m_mMaterialIds[_name]];
}

#endif