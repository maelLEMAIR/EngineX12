#include "D3D12UiShader.h"

#include "../Base/D3D12Context.h"
#include "D3D12UiMaterial.h"

void D3D12UiShader::Bind()
{
    assert(m_pContext != nullptr && "Can't bind UiShader with Null Context");
    assert(m_pPso     != nullptr && "Can't bind UiShader with Null PSO");
    assert(m_pRootSig != nullptr && "Can't bind UiShader with Null Root Signature");

    ID3D12DescriptorHeap* heaps[] = { m_pContext->GetCbvSrvUavHeap() };
    m_pContext->GetCommandList()->SetDescriptorHeaps(1, heaps);

    m_pContext->GetCommandList()->SetPipelineState(m_pPso);
    m_pContext->GetCommandList()->SetGraphicsRootSignature(m_pRootSig);
}

UiMaterial* D3D12UiShader::CreateMaterial()
{
    D3D12UiMaterial* mat = new D3D12UiMaterial(m_pContext, this);
    return mat;
}

D3D12UiShader::D3D12UiShader(D3D12Context* _pContext, ShaderFormat _format, ShaderDescriptor _desc)
    : m_pContext(_pContext), UiShader(_desc)
{
    m_format = _format;
}

void D3D12UiShader::Compile(WString const& _shaderPath)
{
    m_pVsBlob = CompileShader(_shaderPath, "VSMain", "vs_5_0");
    m_pPsBlob = CompileShader(_shaderPath, "PSMain", "ps_5_0");
    
    ID3DBlob* rootSigBlob;
    HRESULT hr = D3DGetBlobPart(
        m_pVsBlob->GetBufferPointer(),
        m_pVsBlob->GetBufferSize(),
        D3D_BLOB_ROOT_SIGNATURE,
        0,
        &rootSigBlob);

    if (FAILED(hr))
        throw std::runtime_error("Failed to extract root signature from shader blob. "
                                 "Make sure the HLSL uses the [RootSignature(...)] attribute.");

    hr = m_pContext->GetDevice()->CreateRootSignature(
        0,
        rootSigBlob->GetBufferPointer(),
        rootSigBlob->GetBufferSize(),
        IID_PPV_ARGS(&m_pRootSig));

    if (FAILED(hr))
        throw std::runtime_error("ID3D12Device::CreateRootSignature failed.");

    BuildUiPSO();
}

ID3DBlob* D3D12UiShader::CompileShader(WString _shaderPath, const String& _entryPoint, const String& _target)
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

    // assert(hr == S_OK);
    if(error != nullptr )
    {
        OutputDebugStringA("Shader compilation error:\n");
        OutputDebugStringA((char*)error->GetBufferPointer());
        error->Release();
    }

    ThrowIfFailed(hr);
    
    return byteCode;
}

void D3D12UiShader::BuildUiPSO()
{
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
        {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = m_pRootSig;
    psoDesc.InputLayout = {inputLayout, _countof(inputLayout)};
    psoDesc.VS =  { m_pVsBlob->GetBufferPointer(), m_pVsBlob->GetBufferSize() };
    psoDesc.PS = { m_pPsBlob->GetBufferPointer(), m_pPsBlob->GetBufferSize() };
     
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
     
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    psoDesc.DepthStencilState.StencilEnable = FALSE;

    D3D12_RENDER_TARGET_BLEND_DESC rtBlend = psoDesc.BlendState.RenderTarget[0];
    rtBlend.BlendEnable = TRUE;
    rtBlend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    rtBlend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    rtBlend.BlendOp = D3D12_BLEND_OP_ADD;
    rtBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
    rtBlend.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    rtBlend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    rtBlend.LogicOp = D3D12_LOGIC_OP_NOOP;
    rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    psoDesc.BlendState.RenderTarget[0] = rtBlend;
    
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = m_pContext->m_backBufferFormat;
    psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

    psoDesc.SampleDesc.Count = m_pContext->Get4xMsaaQuality() > 1 ? 4 : 1;
    psoDesc.SampleDesc.Quality = m_pContext->Get4xMsaaQuality() > 1 ? m_pContext->Get4xMsaaQuality() - 1 : 0;
    
     if (m_pPso != nullptr)
     {
         m_pPso->Release();
         m_pPso = nullptr;
     }
     
     HRESULT hr = m_pContext->GetDevice()->CreateGraphicsPipelineState(
         &psoDesc, IID_PPV_ARGS(&m_pPso));

    if (FAILED(hr)) {
        char buf[256];
        sprintf_s(buf, "PSO creation failed: HRESULT = 0x%08X\n", (unsigned)hr);
        OutputDebugStringA(buf);
    }
    
     if (FAILED(hr))
         OutputDebugStringA("Failed to create graphics pipeline state\n");
     ThrowIfFailed(hr);

     if (m_pPso != nullptr)
         OutputDebugStringA("PSO created successfully!\n");
     else
         OutputDebugStringA("PSO is NULL!\n");
    
     if (m_pRootSig != nullptr)
         OutputDebugStringA("Root signature is valid!\n");
     else
         OutputDebugStringA("Root signature is NULL!\n");
}
