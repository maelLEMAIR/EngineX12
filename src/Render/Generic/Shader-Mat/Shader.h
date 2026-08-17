#ifndef SHADER_H_DEFINED
#define SHADER_H_DEFINED

#include "../../Common/Common.h"
#include "ShaderFormat.h"

class Material;
class UiMaterial;

class ShaderBase
{
public:
    virtual ~ShaderBase() = default;

    virtual void Bind() = 0;

    ShaderFormat& GetFormat()           { return m_format; }
    ShaderDescriptor& GetDescriptor()   { return m_desc; }
    
protected:
    ShaderFormat m_format;
    ShaderDescriptor m_desc;
    
    ShaderBase(ShaderDescriptor _desc) : m_desc(_desc) {}
    
private:
    friend class MaterialBase;
};

class Shader : public ShaderBase
{
public:
    virtual Material* CreateMaterial() = 0;

    bool IsLit() { return m_desc.isLit; }
    
protected:
    Shader(ShaderDescriptor _desc) : ShaderBase(_desc) {};

private:
    friend class Material;
};

class UiShader : public ShaderBase
{
public:
    virtual UiMaterial* CreateMaterial() = 0;

    bool IsLit() { return m_desc.isLit; }
    
protected:
    UiShader(ShaderDescriptor _desc) : ShaderBase(_desc) {};

private:
    friend class Material;
};

#endif
