#ifndef SHADER_FACTORY_HPP_DEFINED
#define SHADER_FACTORY_HPP_DEFINED

#include "../Base/Device.h"
#include "../Shader-Mat/Shader.h"
#include "../Shader-Mat/Material.h"

class ShaderFactory
{
public:
    static Shader* CreateUnlitColored(Device* _pDevice)
    {
        ShaderFormat coloredF;
        coloredF.AddProperty("Color", PropertyType::Float4, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
        
        Shader* coloredS = _pDevice->CreateShader(RES("/Shaders/colored-unlit.hlsl"), coloredF);
        
        return coloredS;
    }

    static Shader* CreateLitColored(Device* _pDevice)
    {
        ShaderFormat litColoredF;
        litColoredF.AddProperty("DiffuseAlbedo", PropertyType::Float4, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        litColoredF.AddProperty("FresnelR0", PropertyType::Float3, XMFLOAT3(0.01f, 0.01f, 0.01f));
        litColoredF.AddProperty("Roughness", PropertyType::Float, 0.25f);

        Shader* litColoredS = _pDevice->CreateShader(RES("/Shaders/colored-lit.hlsl"), litColoredF, true);

        return litColoredS;
    }

    static Shader* CreateUnlitTextured(Device* _pDevice)
    {
        ShaderFormat texturedF;
        texturedF.AddTexture("Albedo");

        Shader* texturedS = _pDevice->CreateShader(RES("/Shaders/textured-unlit.hlsl"), texturedF);

        return texturedS;
    }

    static Shader* CreateLitTextured(Device* _pDevice)
    {
        ShaderFormat texturedF;
        texturedF.AddTexture("Albedo");
        texturedF.AddTexture("Roughness");
        texturedF.AddTexture("Normal");
        texturedF.AddTexture("Ambient");

        texturedF.AddProperty("FresnelR0", PropertyType::Float3, XMFLOAT3(0.01f, 0.01f, 0.01f));
        texturedF.AddProperty("Roughness", PropertyType::Float, 0.25f);
        
        Shader* texturedS = _pDevice->CreateShader(RES("/Shaders/textured-lit.hlsl"), texturedF, true);

        return texturedS;
    }

    static UiShader* CreateUIBasic(Device* _pDevice)
    {
        ShaderFormat uiF;
        uiF.AddProperty("Color", PropertyType::Float4, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        uiF.AddProperty("TextureRect", PropertyType::Float4, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
        uiF.AddTexture("Image");
        
        UiShader* uiS = _pDevice->CreateUiShader(RES("/Shaders/ui-basic.hlsl"), uiF);

        return uiS;
    }

    static UiShader* CreateBlitShader(Device* _pDevice)
    {
        ShaderFormat blitF;
        blitF.AddTexture("Input");

        UiShader* blitS = _pDevice->CreateUiShader(RES("/Shaders/blit.hlsl"), blitF);

        return blitS;
    }
};

#endif