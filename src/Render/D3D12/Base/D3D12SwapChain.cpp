#include "D3D12SwapChain.h"

#include "D3D12Context.h"
#include "D3D12DepthStencil.h"
#include "D3D12RenderTarget.h"

D3D12SwapChain::~D3D12SwapChain()
{
    if (m_pSwapChain != nullptr)
    {
        m_pSwapChain->Release();
        m_pSwapChain = nullptr;
    }

    for (int i = 0; i < m_bufferCount; i++)
    {
        if (m_vRenderTargets[i] != nullptr)
            delete m_vRenderTargets[i];
    }
    m_vRenderTargets.clear();
    
    delete m_pDepthStencil;
} 

void D3D12SwapChain::Present()
{
    m_pSwapChain->Present(0, 0);
    m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    m_pContext->FlushCommandQueue();
}

void D3D12SwapChain::Resize(int _width, int _heigth)
{
    m_pContext->FlushCommandQueue();
    
    for (int i = 0; i < m_vRenderTargets.size(); i++)
    {
        delete m_vRenderTargets[i];
    }
    m_vRenderTargets.clear();
    
    m_pSwapChain->ResizeBuffers(
       m_bufferCount, 
       _width, _heigth, 
       DXGI_FORMAT_R8G8B8A8_UNORM, 
       DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    
    for (int i = 0; i < m_bufferCount; i++)
    {
        ID3D12Resource* r = nullptr;
        m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&r));
        D3D12RenderTarget* rt = new D3D12RenderTarget(_width, _heigth, m_pContext);

        rt->SetResource(r, true);
        
        m_vRenderTargets.push_back(rt);
    }

    m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
    
    m_pDepthStencil->Resize(_width, _heigth);
}

D3D12SwapChain::D3D12SwapChain(int _width, int _height, D3D12Context* _pContext, HWND _hwnd)
{
    m_pContext = _pContext;

    assert(m_pContext != nullptr && "Can't create SwapChain with null context");

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = _width;
    sd.BufferDesc.Height = _height;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;

    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    sd.BufferCount = m_bufferCount;
    sd.OutputWindow = _hwnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGISwapChain* tempSwapChain;
    ThrowIfFailed(m_pContext->GetDXGIFactory()->CreateSwapChain(
        m_pContext->GetCommandQueue(), &sd, &tempSwapChain));
	
    ThrowIfFailed(tempSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain)));
	
    tempSwapChain->Release();

    // for (int i = 0; i < m_bufferCount; i++)
    // {
    //     m_vRenderTargets.push_back(new D3D12RenderTarget(_width, _height, _pContext));
    // }
    D3D12DepthStencil* ds = new D3D12DepthStencil(_width, _height, _pContext);
    ds->Init();

    m_pDepthStencil = ds;

    Resize(_width, _height);
}
