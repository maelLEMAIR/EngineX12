#ifndef D3D12COMPUTESHADER_H_DEFINED
#define D3D12COMPUTESHADER_H_DEFINED

#include "../define.h"
#include "../../Generic/Shader-Mat/ComputeShader.h"

class D3D12Texture;
class Texture;
class UAVBuffer;
class D3D12RenderTarget;
class D3D12Device;
class D3D12Context;
class UploadBufferBase;

class D3D12ComputeShader : public ComputeShader
{
public:
    ~D3D12ComputeShader() override;
    void SetRTV(String const& _bindingName, RenderTarget* _pRenderTarget);
    void SetSRV(String const& _bindingName, RenderTarget* _pRenderTarget);

    void Bind() override;
    
    void Dispatch() override;
    void FlushUAVWrites() override;
    void CalculateGroupSizes() override;

    void AddBinding(String const& _bindingName, ResBindingType _type, uint32 _rootIndex, uint32 _shaderSlot) override;
    
private:
    
    ID3D12RootSignature* m_rootSignature = nullptr;
    ID3D12PipelineState* m_pso           = nullptr;
    ID3DBlob*            m_shaderBlob    = nullptr;
    
    UnorderedMap<String, UploadBufferBase*>  m_cbvs;
    UnorderedMap<String, UAVBuffer*>         m_uavs;

    D3D12Context* m_pContext = nullptr;
    
    D3D12ComputeShader(D3D12Context* _pContext);

    void CopyIntoCB(String const& _bindingName, const void* _data, uint32 _elementSize) override;
    void CopyIntoUA(String const& _bindingName, const void* _data, uint32 _elementSize, uint32 _elementCount = 1) override;
    void CopyFromUA(String const& _bindingName, void* _data, uint32 _elementSize) override;
    
    void      Compile(WString const& _shaderPath);

    ID3DBlob* CompileShader(WString const& _shaderPath, String  const& _entryPoint, String  const& _target);

    void ExtractRootSignature();
    void BuildPSO();
    void BindRootDescriptors();
    

    friend class D3D12Device;
};

#endif
