#include "Material.h"

#include "Shader.h"
#include "ShaderFormat.h"

MaterialBase::MaterialBase(ShaderBase* _pShader)
{
    m_pShaderBase = _pShader;
    for (auto [name, property] : m_pShaderBase->GetFormat().GetProperties())
    {
        m_properties[name] = property.defaultValue;
    }
    for (auto [name, index] : m_pShaderBase->GetFormat().GetTextures())
    {
        m_textures[name] = nullptr;
    }
}

void MaterialBase::CopyToBuffer(void* _dest)
{
    for (auto& [name, desc] : m_pShaderBase->GetFormat().GetProperties())
    {
        auto it = m_properties.find(name);
        if (it == m_properties.end()) continue;
        CopyProperty(_dest, it->second, desc);
    }
}

void MaterialBase::SetTexture(const String& _name, Texture* _pTexture)
{
    assert(m_textures.count(_name) > 0 && "Texture not found in shader");
    m_textures[_name] = _pTexture;
    String propertyName = "Use";
    propertyName.append(_name);
    propertyName.append("Map");

    SetInt(propertyName, 1);
}


void MaterialBase::SetProperty(const String& _name, Any _value)
{
    assert(m_properties.count(_name) > 0 && "Property not found in shader");
    m_properties[_name] = _value;
}

void MaterialBase::CopyProperty(void* _dest, Any& _value, const PropertyDescriptor& desc)
{
    char* ptr = static_cast<char*>(_dest) + desc.offset;
    switch (desc.type)
    {
    case PropertyType::Float: {float v = std::any_cast<float>(_value); memcpy(ptr, &v, desc.size); break;}
    case PropertyType::Float2: {Vect2f32 v = std::any_cast<Vect2f32>(_value); memcpy(ptr, &v, desc.size); break;}
    case PropertyType::Float3: {Vect3f32 v = std::any_cast<Vect3f32>(_value); memcpy(ptr, &v, desc.size); break;}
    case PropertyType::Float4: {Vect4f32 v = std::any_cast<Vect4f32>(_value); memcpy(ptr, &v, desc.size); break;}
    case PropertyType::Int: {int v = std::any_cast<int>(_value); memcpy(ptr, &v, desc.size); break;}
    }
}

Material::Material(Shader* _pShader) : MaterialBase(_pShader)
{
    m_pShader = _pShader;
}

UiMaterial::UiMaterial(UiShader* _pShader) : MaterialBase(_pShader)
{
    m_pShader = _pShader;
}


