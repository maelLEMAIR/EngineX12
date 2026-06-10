#include "D3D12Texture.h"

#include "../Base/D3D12Context.h"
#include "../Utils/DDSTextureLoader.h"

void D3D12Texture::Load(const WString& _path)
{
    m_pContext->FlushCommandQueue();
    m_pContext->ResetCmdList();
    
    HRESULT hr = CreateDDSTextureFromFile12(m_pContext->GetDevice(),
        m_pContext->GetCommandList(), _path.c_str(),
        &m_pResource, &m_pUploader);

    ThrowIfFailed(hr);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
    );
    m_pContext->GetCommandList()->ResourceBarrier(1, &barrier);
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
        m_pContext->GetCbvSrvUavHeap()->GetCPUDescriptorHandleForHeapStart());

    m_heapIndex = m_pContext->GetFirstAvailableCbvSrvUavIndex();
    hDescriptor.Offset(m_heapIndex, m_pContext->m_cbvSrvDescriptorSize);
    
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = m_pResource->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = m_pResource->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    m_pContext->GetDevice()->CreateShaderResourceView(m_pResource, &srvDesc, hDescriptor);

    m_pContext->CloseCmdList();
    m_pContext->FlushCommandQueue();

    
}

void D3D12Texture::Bind(uint32 _index)
{
    // char buf[256];
    // sprintf_s(buf, "CommandList: 0x%p\n", m_pContext->GetCommandList());
    // OutputDebugStringA(buf);
    // sprintf_s(buf, "Root index:  %u\n", _index);
    // OutputDebugStringA(buf);

    assert(m_pContext->GetCommandList() != nullptr);
    
    m_pContext->GetCommandList()->SetGraphicsRootDescriptorTable(
        _index, GetHandle());
}

D3D12Texture::~D3D12Texture()
{
    if (m_pResource != nullptr)
    {
        m_pResource->Release();
        delete m_pResource;
    }
    if (m_pUploader != nullptr)
    {
        m_pUploader->Release();
        delete m_pUploader;
    }

    m_pContext->FreeCbvSrvUavIndex(m_heapIndex);
}

D3D12Texture::D3D12Texture(D3D12Context* _pContext)
{
    m_pContext = _pContext;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE D3D12Texture::GetHandle()
{
    assert((m_pContext != nullptr || m_heapIndex != -1) && "Can't access handle to uninitiated Texture");
    
    assert(m_pContext != nullptr);
    assert(m_heapIndex != -1);
    assert(m_pContext->GetCbvSrvUavHeap() != nullptr);
    assert(m_pContext->m_cbvSrvDescriptorSize != 0);
    
    CD3DX12_GPU_DESCRIPTOR_HANDLE heapHandle(m_pContext->GetCbvSrvUavHeap()->GetGPUDescriptorHandleForHeapStart());
    heapHandle.Offset(m_heapIndex, m_pContext->m_cbvSrvDescriptorSize);

    return heapHandle;
}
