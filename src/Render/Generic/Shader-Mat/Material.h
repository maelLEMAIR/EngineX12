#ifndef MATERIAL_H_DEFINED
#define MATERIAL_H_DEFINED

#include "ShaderFormat.h"
#include "Shader.h"
#include "../../Common/Common.h"

class MaterialBase
{
public:
    virtual void Bind() = 0;

    MaterialBase(ShaderBase* _pShader);
    virtual ~MaterialBase() = default;

    void SetFloat(const String&     _name, float _value)    { SetProperty(_name, _value); }
    void SetFloat2(const String&    _name, Vect2f32 _value) { SetProperty(_name, _value); }
    void SetFloat3(const String&    _name, Vect3f32 _value) { SetProperty(_name, _value); }
    void SetFloat4(const String&    _name, Vect4f32 _value) { SetProperty(_name, _value); }
    void SetInt(const String&       _name, int _value)      { SetProperty(_name, _value); }

    void SetTexture(const String& _name, Texture* _pTexture);
    
    void CopyToBuffer(void* _dest);

protected:
    ShaderBase* m_pShaderBase = nullptr;
    
    UnorderedMap<String, Any> m_properties;
    UnorderedMap<String, Texture*> m_textures;

private:
    void SetProperty(const String& _name, Any _value);
    void CopyProperty(void* _dest, Any& _value, const PropertyDescriptor& desc);
};

class Material : public MaterialBase
{
public:
    Material(Shader* _pShader);
    
    bool IsLit() { return m_pShader->IsLit(); }

protected:
    Shader* m_pShader = nullptr;
};

class UiMaterial : public MaterialBase
{
public:
    UiMaterial(UiShader* _pShader);

protected:
    UiShader* m_pShader = nullptr;
};

#endif
