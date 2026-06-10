#include "D3D12Shader.h"

#include "../Base/D3D12Context.h"
#include "D3D12Material.h"

D3D12Shader::D3D12Shader(D3D12Context* _pContext, ShaderFormat _format, bool _isLit)
{
    m_pContext = _pContext;
    m_format = _format;
    m_isLit = _isLit;
}

void D3D12Shader::Bind()
{
    assert(m_pContext != nullptr && "Can't bind Shader with Null Context");
    assert(m_pPso != nullptr && "Can't bind Shader with Null PSO");
    assert(m_pRootSig != nullptr && "Can't bind Shader with Null Root signature");

    ID3D12DescriptorHeap* heaps[] = { m_pContext->GetCbvSrvUavHeap() };
    m_pContext->GetCommandList()->SetDescriptorHeaps(1, heaps);
    
    m_pContext->GetCommandList()->SetPipelineState(m_pPso);
    m_pContext->GetCommandList()->SetGraphicsRootSignature(m_pRootSig);
}

Material* D3D12Shader::CreateMaterial()
{
    D3D12Material* mat = new D3D12Material(m_pContext, this);
    return mat;
}

D3D12Shader::~D3D12Shader()
{
    if (m_pVsBlob != nullptr)
        m_pVsBlob->Release();
    delete m_pVsBlob;
    if (m_pPsBlob != nullptr)
        m_pPsBlob->Release();
    delete m_pPsBlob;
    if (m_pPso != nullptr)
        m_pPso->Release();
    delete m_pPso;
    if (m_pRootSig != nullptr)
        m_pRootSig->Release();
    delete m_pRootSig;
}

void D3D12Shader::Compile(WString const& _shaderPath)
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

    BuildPSO();
}

ID3DBlob* D3D12Shader::CompileShader(WString _shaderPath, const String& _entryPoint, const String& _target)
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

void D3D12Shader::BuildPSO()
{
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
        {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = m_pRootSig;
    psoDesc.InputLayout = {inputLayout, _countof(inputLayout)};
    psoDesc.VS =  { m_pVsBlob->GetBufferPointer(), m_pVsBlob->GetBufferSize() };
    psoDesc.PS = { m_pPsBlob->GetBufferPointer(), m_pPsBlob->GetBufferSize() };
     
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
     
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
     
    psoDesc.BlendState.RenderTarget[0].BlendEnable = false;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
     
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.SampleDesc.Count = m_pContext->Get4xMsaaQuality() > 1 ? 4 : 1;
    psoDesc.SampleDesc.Quality = m_pContext->Get4xMsaaQuality() - 1;
    psoDesc.RTVFormats[0] = m_pContext->m_backBufferFormat;
    psoDesc.DSVFormat = m_pContext->m_depthStencilFormat;

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
