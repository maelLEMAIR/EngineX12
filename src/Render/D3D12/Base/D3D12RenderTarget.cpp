#include "D3D12RenderTarget.h"

#include "D3D12Context.h"

D3D12RenderTarget::D3D12RenderTarget(int _width, int _height, D3D12Context* _pContext, XMFLOAT3 _clearColor)
{
    m_width = _width;
    m_height = _height;

    m_pContext = _pContext;
    m_clearColor = _clearColor;
}


D3D12RenderTarget::~D3D12RenderTarget()
{
    if ( m_resource != nullptr )
        m_resource->Release();
    if (m_pContext != nullptr)
    {
        if (m_rtvHeapIndex != -1)
            m_pContext->FreeRtvIndex(m_rtvHeapIndex);
        if (m_srvHeapIndex != -1)                     
            m_pContext->FreeCbvSrvUavIndex(m_srvHeapIndex);
        if (m_uavHeapIndex != -1)                     
            m_pContext->FreeCbvSrvUavIndex(m_uavHeapIndex);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderTarget::GetRtvHandle()
{
    assert(m_pContext != nullptr && m_rtvHeapIndex != -1 && "Can't access handle to uninitiated RenderTarget");

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_pContext->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    rtvHeapHandle.Offset(m_rtvHeapIndex, m_pContext->m_rtvDescriptorSize);

    return rtvHeapHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12RenderTarget::GetSrvHandle()
{
    assert(m_pContext != nullptr && m_srvHeapIndex != -1 && "Can't access handle to uninitiated RenderTarget");

    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHeapHandle(m_pContext->GetCbvSrvUavHeap()->GetGPUDescriptorHandleForHeapStart());
    srvHeapHandle.Offset(m_srvHeapIndex, m_pContext->m_cbvSrvDescriptorSize);

    return srvHeapHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12RenderTarget::GetUavHandle()
{
    assert(m_pContext != nullptr && m_uavHeapIndex != -1 && "Can't access handle to uninitiated RenderTarget");
    CD3DX12_GPU_DESCRIPTOR_HANDLE uavHeapHandle(m_pContext->GetCbvSrvUavHeap()->GetGPUDescriptorHandleForHeapStart());
    uavHeapHandle.Offset(m_uavHeapIndex, m_pContext->m_cbvSrvDescriptorSize);

    return uavHeapHandle;
}

void D3D12RenderTarget::SetResource(ID3D12Resource* _resource, bool _isBackBuffer)
{
    if (m_resource != nullptr)
    {
        m_resource->Release();
        m_pContext->FreeRtvIndex(m_rtvHeapIndex);
        m_pContext->FreeCbvSrvUavIndex(m_srvHeapIndex);
        m_pContext->FreeCbvSrvUavIndex(m_uavHeapIndex);
    }

    m_srvHeapIndex = m_pContext->GetFirstAvailableCbvSrvUavIndex();
    m_uavHeapIndex = m_pContext->GetFirstAvailableCbvSrvUavIndex();
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_pContext->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    m_rtvHeapIndex = m_pContext->GetFirstAvailableRtvIndex();
    rtvHeapHandle.Offset(m_rtvHeapIndex, m_pContext->m_rtvDescriptorSize);

    m_pContext->GetDevice()->CreateRenderTargetView(_resource, nullptr, rtvHeapHandle);
    m_resource = _resource;

    if (!_isBackBuffer)
        CreateComputeDescriptors();
}

void D3D12RenderTarget::Init()
{
    if (m_resource != nullptr)
    {
        m_resource->Release();
        m_pContext->FreeRtvIndex(m_rtvHeapIndex);
        m_pContext->FreeCbvSrvUavIndex(m_srvHeapIndex);
        m_pContext->FreeCbvSrvUavIndex(m_uavHeapIndex);
    }

    m_srvHeapIndex = m_pContext->GetFirstAvailableCbvSrvUavIndex();
    m_uavHeapIndex = m_pContext->GetFirstAvailableCbvSrvUavIndex();
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_pContext->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    m_rtvHeapIndex = m_pContext->GetFirstAvailableRtvIndex();
    rtvHeapHandle.Offset(m_rtvHeapIndex, m_pContext->m_rtvDescriptorSize);

    D3D12_RESOURCE_DESC renderTargetDesc;
    renderTargetDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    renderTargetDesc.Alignment = 0;
    renderTargetDesc.Width = m_width;
    renderTargetDesc.Height = m_height;
    renderTargetDesc.DepthOrArraySize = 1;
    renderTargetDesc.MipLevels = 1;
    renderTargetDesc.Format = m_pContext->m_backBufferFormat;
    renderTargetDesc.SampleDesc.Count = m_pContext->Get4xMsaaQuality() > 1 ? 4 : 1;
    renderTargetDesc.SampleDesc.Quality = m_pContext->Get4xMsaaQuality() > 1 ? m_pContext->Get4xMsaaQuality() - 1 : 0;
    renderTargetDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    renderTargetDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    
    D3D12_CLEAR_VALUE optClear;
    optClear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    optClear.Color[0] = m_clearColor.x;
    optClear.Color[1] = m_clearColor.y;
    optClear.Color[2] = m_clearColor.z;
    optClear.Color[3] = 1.0f;
    
    CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ID3D12Device* device = m_pContext->GetDevice();
    
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &renderTargetDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &optClear,
        IID_PPV_ARGS(&m_resource)));

    m_currState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = m_pContext->m_backBufferFormat;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;

    device->CreateRenderTargetView(m_resource, &rtvDesc, rtvHeapHandle);

    CreateComputeDescriptors();
}

void D3D12RenderTarget::Resize(int _width, int _height)
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
    
    D3D12_RESOURCE_DESC renderTargetDesc;
    renderTargetDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    renderTargetDesc.Alignment = 0;
    renderTargetDesc.Width = m_width;
    renderTargetDesc.Height = m_height;
    renderTargetDesc.DepthOrArraySize = 1;
    renderTargetDesc.MipLevels = 1;
    renderTargetDesc.Format = m_pContext->m_backBufferFormat;
    renderTargetDesc.SampleDesc.Count = m_pContext->Get4xMsaaQuality() > 1 ? 4 : 1;
    renderTargetDesc.SampleDesc.Quality = m_pContext->Get4xMsaaQuality() > 1 ? m_pContext->Get4xMsaaQuality() - 1 : 0;
    renderTargetDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    renderTargetDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    
    D3D12_CLEAR_VALUE optClear;
    optClear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    optClear.Color[0] = m_clearColor.x;
    optClear.Color[1] = m_clearColor.y;
    optClear.Color[2] = m_clearColor.z;
    optClear.Color[3] = 1.0f;
    
    CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ID3D12Device* device = m_pContext->GetDevice();
    
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &renderTargetDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &optClear,
        IID_PPV_ARGS(&m_resource)));

    m_currState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_pContext->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    rtvHeapHandle.Offset(m_rtvHeapIndex, m_pContext->m_rtvDescriptorSize);
    
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = m_pContext->m_backBufferFormat;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;
    
    device->CreateRenderTargetView(m_resource, &rtvDesc, rtvHeapHandle);

    CreateComputeDescriptors();
}

void D3D12RenderTarget::CreateComputeDescriptors()
{
    ID3D12Device*         device = m_pContext->GetDevice();
    ID3D12DescriptorHeap* heap   = m_pContext->GetCbvSrvUavHeap();

    D3D12_CPU_DESCRIPTOR_HANDLE heapStart = heap->GetCPUDescriptorHandleForHeapStart();
    const DXGI_FORMAT format = m_resource->GetDesc().Format;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc     = {};
    srvDesc.Shader4ComponentMapping             = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format                              = format;
    srvDesc.ViewDimension                       = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels                 = 1;
    srvDesc.Texture2D.MostDetailedMip           = 0;
    srvDesc.Texture2D.ResourceMinLODClamp       = 0.0f;

    CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_pContext->GetCbvSrvUavHeap()->GetCPUDescriptorHandleForHeapStart());
    srvHandle.Offset(m_srvHeapIndex, m_pContext->m_cbvSrvDescriptorSize);
    device->CreateShaderResourceView(m_resource, &srvDesc, srvHandle);

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc    = {};
    uavDesc.Format                              = format;
    uavDesc.ViewDimension                       = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice                  = 0;
    uavDesc.Texture2D.PlaneSlice                = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE uavHandle(m_pContext->GetCbvSrvUavHeap()->GetCPUDescriptorHandleForHeapStart());
    uavHandle.Offset(m_uavHeapIndex, m_pContext->m_cbvSrvDescriptorSize);
    device->CreateUnorderedAccessView(m_resource, nullptr, &uavDesc, uavHandle);
}

void D3D12RenderTarget::Clear(XMFLOAT3 _color)
{
    if (m_pContext == nullptr)
        return;
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_pContext->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    rtvHeapHandle.Offset(m_rtvHeapIndex, m_pContext->m_rtvDescriptorSize);
    
    m_pContext->GetCommandList()->ClearRenderTargetView(
        rtvHeapHandle, XMVECTORF32{ _color.x, _color.y, _color.z, 1.0f },
        0, nullptr
        );
}

void D3D12RenderTarget::BindAsTexture(uint32 _rootIndex)
{
    assert(m_pContext != nullptr && m_srvHeapIndex != -1 && "Can't bind uninitiated RenderTarget as SRV");
    assert(m_currState == (D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
        && "RenderTarget must be in shader resource state before binding — call SetInput() first");

    m_pContext->GetCommandList()->SetGraphicsRootDescriptorTable(
        _rootIndex, GetSrvHandle());
}

void D3D12RenderTarget::SetToWrite()
{
    if (m_pContext == nullptr)
        return;

    if (m_currState == D3D12_RESOURCE_STATE_RENDER_TARGET) return;
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
    m_resource,
    m_currState,
    D3D12_RESOURCE_STATE_RENDER_TARGET);

    m_currState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    
    m_pContext->GetCommandList()->ResourceBarrier(1, &barrier);
}

void D3D12RenderTarget::SetToRead()
{
    if (m_pContext == nullptr)
        return;

    if (m_currState == D3D12_RESOURCE_STATE_PRESENT) return;
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
    m_resource,
    m_currState,
    D3D12_RESOURCE_STATE_PRESENT);

    m_currState = D3D12_RESOURCE_STATE_PRESENT;

    m_pContext->GetCommandList()->ResourceBarrier(1, &barrier);
}

void D3D12RenderTarget::SetComputeOutput()
{
    if (m_pContext == nullptr)
        return;
    
    if (m_currState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS) return;
    
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_resource,
        m_currState,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    m_currState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    
    m_pContext->GetCommandList()->ResourceBarrier(1, &barrier);
}

void D3D12RenderTarget::SetInput()
{
    if (m_pContext == nullptr)
        return;
    
    if (m_currState == (D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)) return;
    
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_resource,
        m_currState,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
        | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_currState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    
    m_pContext->GetCommandList()->ResourceBarrier(1, &barrier);
}
