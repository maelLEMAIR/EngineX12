#ifndef SHADERFORMAT_H_DEFINED
#define SHADERFORMAT_H_DEFINED

#include "../../Common/Common.h"

class Texture;

enum class PropertyType
{
    Float, Float2, Float3, Float4, Int
};

struct PropertyDescriptor
{
    PropertyType type;
    Any defaultValue;
    uint32 size;
    uint32 offset;
};

struct ShaderDescriptor
{
    bool isLit = false;
    bool isWireFrame = false;
};

class ShaderFormat
{
public:
    void AddProperty(String _name, PropertyType _type, Any _defaultValue);
    void AddTexture(String _name);
    
    const UnorderedMap<String, PropertyDescriptor>& GetProperties() const { return m_properties; }
    const UnorderedMap<String, uint32>& GetTextures() const { return m_textures; }
    
    uint32 GetTotalSize() { return m_currentSize; }
    
private:
    UnorderedMap<String, PropertyDescriptor> m_properties;
    UnorderedMap<String, uint32> m_textures;
    uint32 m_currentSize = 0;
    uint32 m_currentTextureIndex = 0;

    uint32 GetTypeSize(PropertyType _type);
    
};

#endif
