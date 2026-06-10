#include "ShaderFormat.h"

void ShaderFormat::AddProperty(String _name, PropertyType _type, Any _defaultValue)
{
    PropertyDescriptor pDesc;
    pDesc.type = _type;
    pDesc.defaultValue = _defaultValue;
    pDesc.size = GetTypeSize(_type);

    int alignment = m_currentSize % 16 + pDesc.size;
    if (alignment > 16)
        m_currentSize += 16 - (alignment % 16);
    
    pDesc.offset = m_currentSize;
    
    m_currentSize += pDesc.size;

    m_properties[_name] = pDesc;
}

void ShaderFormat::AddTexture(String _name)
{
    m_textures[_name] = m_currentTextureIndex;
    m_currentTextureIndex++;
    String propertyName = "Use";
    propertyName.append(_name);
    propertyName.append("Map");
    AddProperty(propertyName, PropertyType::Int, 0);
}

uint32 ShaderFormat::GetTypeSize(PropertyType _type)
{
    switch (_type)
    {
    case PropertyType::Float    : return 4;
    case PropertyType::Float2   : return 8;
    case PropertyType::Float3   : return 12;
    case PropertyType::Float4   : return 16;
    case PropertyType::Int      : return 4;
    default                     : return 0;
    }
}
