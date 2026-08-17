#ifndef D3D12UISHADER_H_DEFINED
#define D3D12UISHADER_H_DEFINED

#include "../define.h"

#include "../../Generic/Shader-Mat/Shader.h"

class D3D12Context;

class D3D12UiShader : public UiShader
{
public:
    void Bind() override;

    UiMaterial* CreateMaterial() override;
    
    ~D3D12UiShader() override = default;

private:
    D3D12Context* m_pContext = nullptr;

    ID3DBlob* m_pVsBlob = nullptr;
    ID3DBlob* m_pPsBlob = nullptr;
    
    ID3D12RootSignature* m_pRootSig = nullptr;
    ID3D12PipelineState* m_pPso = nullptr;

    WString m_shaderPath;
    
    D3D12UiShader(D3D12Context* _pContext, ShaderFormat _format, ShaderDescriptor _desc);

    void Compile(WString const& _shaderPath);
    ID3DBlob* CompileShader(WString _shaderPath, const String& _entryPoint, const String& _target);
    void BuildUiPSO();

    friend class D3D12Device;
};

#endif
