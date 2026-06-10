#include "D3D12ComputeShader.h"

#include "../Base/D3D12RenderTarget.h"
#include "../Base/D3D12Context.h"

#include "../Buffers/UAVBuffer.hpp"
#include "../Buffers/UploadBufferBase.hpp"

D3D12ComputeShader::~D3D12ComputeShader()
{
    if (m_pso)
    {
        m_pso->Release();
        m_pso = nullptr;
    }
    if (m_rootSignature)
    {
        m_rootSignature->Release();
        m_rootSignature = nullptr;
    }
    if (m_shaderBlob)
    {
        m_shaderBlob->Release();
        m_shaderBlob = nullptr;
    }
}

void D3D12ComputeShader::SetRTV(String const& _bindingName, RenderTarget* _pRenderTarget)
{
    assert(m_resourceBindings.count(_bindingName) && "Binding doesn't exist in selected CS");
    assert(_pRenderTarget != nullptr && "Can't assign empty RenderTarget to CS binding");

    D3D12RenderTarget* rt = dynamic_cast<D3D12RenderTarget*>(_pRenderTarget);
    assert(rt != nullptr && "Can't cast to D3D12RenderTarget");
    
    m_outputRtvs[_bindingName] = _pRenderTarget;
    m_resourceBindings[_bindingName].heapIndex = rt->GetUAVHeapIndex();
}

void D3D12ComputeShader::SetSRV(String const& _bindingName, RenderTarget* _pRenderTarget)
{
    assert(m_resourceBindings.count(_bindingName) && "Binding doesn't exist in selected CS");
    assert(_pRenderTarget != nullptr && "Can't assign empty RenderTarget to CS binding");

    D3D12RenderTarget* rt = dynamic_cast<D3D12RenderTarget*>(_pRenderTarget);
    assert(rt != nullptr && "Can't cast to D3D12RenderTarget");
    
    m_inputRtvs[_bindingName] = _pRenderTarget;
    m_resourceBindings[_bindingName].heapIndex = rt->GetSRVHeapIndex();
}

void D3D12ComputeShader::Bind()
{
    assert(m_pso != nullptr && "CS Bind : PSO not compiled");
    assert(m_rootSignature != nullptr && "CS Bind : Root signature missing");

    ID3D12GraphicsCommandList* cmdList = m_pContext->GetCommandList();
    
    cmdList->SetComputeRootSignature(m_rootSignature);
    cmdList->SetPipelineState(m_pso);

    ID3D12DescriptorHeap* heaps[] = { m_pContext->GetCbvSrvUavHeap() };
    cmdList->SetDescriptorHeaps(1, heaps);

    BindRootDescriptors();
}

void D3D12ComputeShader::Dispatch()
{
    ID3D12GraphicsCommandList* cmdList = m_pContext->GetCommandList();
    cmdList->Dispatch(m_threadGroupSizeX, m_threadGroupSizeY, m_threadGroupSizeZ);
}

void D3D12ComputeShader::FlushUAVWrites()
{
    ID3D12GraphicsCommandList* cmdList = m_pContext->GetCommandList();

    for (auto [name, pBuf] : m_uavs)
    {
        if (pBuf)
            pBuf->FlushWrites();
    }

    for (auto [name, pRT] : m_outputRtvs)
    {
        if (!pRT) continue;
        D3D12RenderTarget* rt = dynamic_cast<D3D12RenderTarget*>(pRT);
        rt->SetToWrite();
    }
}

void D3D12ComputeShader::CalculateGroupSizes()
{
    uint32 width = 1, height = 1;

    for (auto [name, rb] : m_resourceBindings)
    {
        if (rb.type == ResBindingType::RTV)
        {
            D3D12RenderTarget* pRT = dynamic_cast<D3D12RenderTarget*>(m_outputRtvs[name]);
            if (pRT != nullptr)
            {
                width  = pRT->GetWidth();
                height = pRT->GetHeight();
                break;
            }
        }
    }

    m_threadGroupSizeX = (width  + m_threadGroupSizeX - 1) / m_threadGroupSizeX;
    m_threadGroupSizeY = (height + m_threadGroupSizeY - 1) / m_threadGroupSizeY;
    m_threadGroupSizeZ = 1;
}

void D3D12ComputeShader::BindRootDescriptors()
{
    ID3D12GraphicsCommandList* cmdList = m_pContext->GetCommandList();
    
    for (auto [name, rb] : m_resourceBindings)
    {
        switch (rb.type)
        {
        case ResBindingType::CBV:
            {
                UploadBufferBase* pBuf = m_cbvs[name];
                assert(pBuf != nullptr && "CS Bind : CBV not set");
                cmdList->SetComputeRootConstantBufferView(rb.rootIndex, pBuf->GetGPUAddress(0));
                break;
            }
        case ResBindingType::SRV:
            {
                D3D12RenderTarget* pRT = dynamic_cast<D3D12RenderTarget*>(m_inputRtvs[name]);
                assert(pRT != nullptr && "CS Bind : Texture not set");
                pRT->SetInput(); 
                cmdList->SetComputeRootDescriptorTable(rb.rootIndex, pRT->GetSrvHandle());
                break;
            }
        case ResBindingType::UAV:
            {
                UAVBuffer* pBuf = m_uavs[name];
                assert(pBuf != nullptr && "CS Bind : UAV not set");
                cmdList->SetComputeRootUnorderedAccessView(rb.rootIndex, pBuf->GetGPUAddress());
                break;
            }
        case ResBindingType::RTV:
            {
                D3D12RenderTarget* pRT = dynamic_cast<D3D12RenderTarget*>(m_outputRtvs[name]);
                assert(pRT != nullptr && "CS Bind : RenderTarget not set");
                pRT->SetComputeOutput();
                cmdList->SetComputeRootDescriptorTable(rb.rootIndex, pRT->GetUavHandle());
                break;
            }
        default:
            assert(false && "Unknown Binding resource type");
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////
/// CREATION
//////////////////////////////////////////////////////////////////

D3D12ComputeShader::D3D12ComputeShader(D3D12Context* _pContext)
{
    assert(_pContext != nullptr && "Can't create Compute Shader withg Null context");
    
    m_pContext = _pContext;
}

void D3D12ComputeShader::CopyIntoCB(String const& _bindingName, const void* _data, uint32 _elementSize)
{
    assert(m_resourceBindings.count(_bindingName) && "Binding doesn't exist in selected CS");

    if (m_cbvs[_bindingName] != nullptr)
        delete m_cbvs[_bindingName];
    
    UploadBufferBase* uBuffer = new UploadBufferBase(m_pContext, _elementSize, 1, true);
    uBuffer->CopyRawData(0, _data);
    m_cbvs[_bindingName] = uBuffer;
}

void D3D12ComputeShader::CopyIntoUA(String const& _bindingName, const void* _data, uint32 _elementSize,
    uint32 _elementCount)
{
    assert(m_resourceBindings.count(_bindingName) && "Binding doesn't exist in selected CS");

    if (m_uavs[_bindingName] != nullptr)
        delete m_uavs[_bindingName];

    UAVBuffer* uBuffer = new UAVBuffer(m_pContext, _elementCount, _data, _elementSize);
    uBuffer->SetOutput();
    m_uavs[_bindingName] = uBuffer;
}

void D3D12ComputeShader::CopyFromUA(String const& _bindingName, void* _data, uint32 _elementSize)
{
    assert(m_resourceBindings.count(_bindingName) && "Binding doesn't exist in selected CS");
    assert(m_uavs[_bindingName] != nullptr && "Binding has not yet been set");

    UAVBuffer* uBuffer = m_uavs[_bindingName];

    Vector<uint8> data;
    data.resize(uBuffer->GetByteSize());
    
    uBuffer->SetCopy();
    uBuffer->ReadbackToCPU(data);
    uBuffer->SetOutput();

    memcpy(_data, data.data(), data.size());
}

void D3D12ComputeShader::Compile(WString const& _shaderPath)
{
    m_shaderBlob = CompileShader(_shaderPath, "CSMain", "cs_5_0");
    ExtractRootSignature();
    BuildPSO();
}

ID3DBlob* D3D12ComputeShader::CompileShader(WString const& _shaderPath, String const& _entryPoint,
                                            String const& _target)
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    

    ID3D10Blob* byteCode = nullptr;
    ID3D10Blob* error = nullptr;

    WString filepath = _shaderPath;
    
    HRESULT hr = D3DCompileFromFile(filepath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        _entryPoint.c_str(), _target.c_str(), compileFlags, 0, &byteCode, &error);

    if(error != nullptr && FAILED(hr))
    {
        MessageBoxA(nullptr, (char*)error->GetBufferPointer(), "Shader Error", MB_OK);
        error->Release();
    }

    if (FAILED(hr))
    {
        char msg[512];
        // Convert and show the full path being used
        WCHAR fullPath[512];
        GetFullPathNameW(filepath.c_str(), 512, fullPath, nullptr);
        wsprintfA(msg, "HRESULT: 0x%08X\nAttempted path:\n%ls\nFull resolved path:\n%ls", 
            hr, filepath.c_str(), fullPath);
        MessageBoxA(nullptr, msg, "Shader Error", MB_OK);
    }

    ThrowIfFailed(hr);
    
    return byteCode;
}

void D3D12ComputeShader::ExtractRootSignature()
{
    assert(m_shaderBlob && "ExtractRootSignature: shader blob is null");

    ID3DBlob* rootSigBlob = nullptr;

    HRESULT hr = D3DGetBlobPart(
        m_shaderBlob->GetBufferPointer(),
        m_shaderBlob->GetBufferSize(),
        D3D_BLOB_ROOT_SIGNATURE,
        0,
        &rootSigBlob);

    assert(SUCCEEDED(hr) &&
        "ExtractRootSignature: no root signature found in shader blob. " &&
        "Make sure your HLSL entry point has a [RootSignature(...)] attribute.");

    hr = m_pContext->GetDevice()->CreateRootSignature(
        0,
        rootSigBlob->GetBufferPointer(),
        rootSigBlob->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature));

    rootSigBlob->Release();

    assert(SUCCEEDED(hr) && "ExtractRootSignature: CreateRootSignature failed");
}

void D3D12ComputeShader::BuildPSO()
{
    assert(m_shaderBlob != nullptr && "BuildPSO: missing blob");
    assert(m_rootSignature != nullptr && "BuildPSO: missing root signature");

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature     = m_rootSignature;
    psoDesc.CS.pShaderBytecode = m_shaderBlob->GetBufferPointer();
    psoDesc.CS.BytecodeLength  = m_shaderBlob->GetBufferSize();

    ThrowIfFailed(m_pContext->GetDevice()->CreateComputePipelineState(
        &psoDesc, IID_PPV_ARGS(&m_pso)))
}

void D3D12ComputeShader::AddBinding(String const& _bindingName, ResBindingType _type, uint32 _rootIndex, uint32 _shaderSlot)
{
    ComputeShader::AddBinding(_bindingName, _type, _rootIndex, _shaderSlot);

    switch (_type)
    {
    case ResBindingType::UAV:
        m_uavs[_bindingName] = nullptr;
        break;
    case ResBindingType::SRV:
        m_inputRtvs[_bindingName] = nullptr;
        break;
    case ResBindingType::RTV:
        m_outputRtvs[_bindingName] = nullptr;
        break;
    case ResBindingType::CBV:
        m_cbvs[_bindingName] = nullptr;
        break;
    default: break;
    }
}





