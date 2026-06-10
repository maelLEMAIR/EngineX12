#include "D3D12DepthStencil.h"

#include "D3D12Context.h"

D3D12DepthStencil::D3D12DepthStencil(int _width, int _height, D3D12Context* _pContext)
{
    m_width = _width;
    m_height = _height;
    m_pContext = _pContext;
}

D3D12DepthStencil::~D3D12DepthStencil()
{
    if ( m_resource != nullptr )
        m_resource->Release();
    if (m_pContext != nullptr && m_heapIndex != -1)
    {
        m_pContext->FreeDsvIndex(m_heapIndex);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DepthStencil::GetHandle()
{
    assert(m_pContext != nullptr && m_heapIndex != -1 && "Can't access handle to uninitiated RenderTarget");

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(m_pContext->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart());
    dsvHeapHandle.Offset(m_heapIndex, m_pContext->m_dsvDescriptorSize);

    return dsvHeapHandle;
}

void D3D12DepthStencil::Init()
{
    if (m_resource != nullptr)
    {
        m_resource->Release();
        m_pContext->FreeDsvIndex(m_heapIndex);
    }
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(m_pContext->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart());
    m_heapIndex = m_pContext->GetFirstAvailableDsvIndex();
    dsvHeapHandle.Offset(m_heapIndex, m_pContext->m_dsvDescriptorSize);

    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_width;
    depthStencilDesc.Height = m_height;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

    depthStencilDesc.SampleDesc.Count = m_pContext->Get4xMsaaQuality() > 1 ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m_pContext->Get4xMsaaQuality() >1 ? m_pContext->Get4xMsaaQuality() - 1 : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ID3D12Device* device = m_pContext->GetDevice();
    
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optClear,
        IID_PPV_ARGS(&m_resource)));
    
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = m_pContext->m_depthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;

    device->CreateDepthStencilView(m_resource, &dsvDesc, dsvHeapHandle);
}

void D3D12DepthStencil::Resize(int _width, int _height)
{
    if (m_width == _width && m_height == _height)
        return;
    
    // Update dimensions
    m_width = _width;
    m_height = _height;
    
    if (m_pContext == nullptr || m_resource == nullptr)
        return;
    
    m_resource->Release();
    m_resource = nullptr;

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(m_pContext->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart());
    dsvHeapHandle.Offset(m_heapIndex, m_pContext->m_dsvDescriptorSize);

    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_width;
    depthStencilDesc.Height = m_height;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

    depthStencilDesc.SampleDesc.Count = m_pContext->Get4xMsaaQuality() > 1 ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m_pContext->Get4xMsaaQuality() >1 ? m_pContext->Get4xMsaaQuality() - 1 : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ID3D12Device* device = m_pContext->GetDevice();
    
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optClear,
        IID_PPV_ARGS(&m_resource)));
    
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = m_pContext->m_depthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;

    device->CreateDepthStencilView(m_resource, &dsvDesc, dsvHeapHandle);
}

void D3D12DepthStencil::Clear()
{
    if (m_pContext == nullptr)
        return;
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(m_pContext->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart());
    dsvHeapHandle.Offset(m_heapIndex, m_pContext->m_dsvDescriptorSize);
    
    m_pContext->GetCommandList()->ClearDepthStencilView(
        dsvHeapHandle,
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.0f, 0, 0, nullptr
        );
}

