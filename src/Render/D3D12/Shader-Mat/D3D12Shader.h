#ifndef D3D12SHADER_H_DEFINED
#define D3D12SHADER_H_DEFINED

#include "../../Generic/Shader-Mat/Shader.h"
#include "../define.h"

class D3D12Context;

class D3D12Shader : public Shader
{
public:
    void Bind() override;

    Material* CreateMaterial() override;

    ~D3D12Shader() override;
    
private:
    D3D12Context* m_pContext = nullptr;

    ID3DBlob* m_pVsBlob = nullptr;
    ID3DBlob* m_pPsBlob = nullptr;
    
    ID3D12RootSignature* m_pRootSig = nullptr;
    ID3D12PipelineState* m_pPso = nullptr;

    WString m_shaderPath;

    D3D12Shader(D3D12Context* _pContext, ShaderFormat _format, bool _isLit = false);
    
    void Compile(WString const& _shaderPath);
    ID3DBlob* CompileShader(WString _shaderPath, const String& _entryPoint, const String& _target);
    void BuildPSO();

    friend class D3D12Device;
};

#endif
