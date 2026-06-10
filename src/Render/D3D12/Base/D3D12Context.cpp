#ifndef GRAPHICS_CONTEXT_CPP_DEFINED
#define GRAPHICS_CONTEXT_CPP_DEFINED

#include "D3D12Context.h"

#pragma comment(lib, "d3d11.lib")

#include "../Utils/d3dUtil.h"

bool D3D12Context::Init()
{
#if defined(DEBUG) || defined(_DEBUG) 
    // Enable the D3D12 debug layer.
    {
        // ID3D12Debug* debugController;
        // ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        // debugController->EnableDebugLayer();
    }
#endif
    
    ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&m_pFactory)));
    ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice)));
    ThrowIfFailed(m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));

    m_rtvDescriptorSize      = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsvDescriptorSize      = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_cbvSrvDescriptorSize   = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = m_backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    m_pDevice->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels));

    m_4xMsaaQuality = msQualityLevels.NumQualityLevels;

    // Command Objects Creation
    
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    ThrowIfFailed(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue)));
    ThrowIfFailed(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator)));
    ThrowIfFailed(m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList)));

    m_pCommandList->Close();

    // DescriptorHeaps

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = m_rtvCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRtvHeap));
    for (int i = 0; i < m_rtvCount; i++)
    {
        m_availableRtvIndices.push_back(i);
    }

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = m_dsvCount;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pDsvHeap));
    for (int i = 0; i < m_dsvCount; i++)
    {
        m_availableDsvIndices.push_back(i);
    }

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = m_cbvSrvUavCount;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    m_pDevice->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&m_pCbvSrvUavHeap));
    for (int i = 0; i < m_cbvSrvUavCount; i++)
    {
        m_availableCbvSrvUavIndices.push_back(i);
    }

    return true;
}

void D3D12Context::FlushCommandQueue()
{
    m_currentFence++;
    m_pCommandQueue->Signal(m_pFence, m_currentFence);

    if (m_pFence->GetCompletedValue() < m_currentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    
        m_pFence->SetEventOnCompletion(m_currentFence, eventHandle);
    
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void D3D12Context::ResetCmdList()
{
    m_pCommandAllocator->Reset();
    m_pCommandList->Reset(m_pCommandAllocator, nullptr);
}

void D3D12Context::CloseCmdList()
{
    ThrowIfFailed(m_pCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { m_pCommandList };
    m_pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

int D3D12Context::GetFirstAvailableRtvIndex()
{
    int i = m_availableRtvIndices.front();
    m_availableRtvIndices.erase(m_availableRtvIndices.begin());
    return i;
}

int D3D12Context::GetFirstAvailableDsvIndex()
{
    int i = m_availableDsvIndices.front();
    m_availableDsvIndices.erase(m_availableDsvIndices.begin());
    return i;
}

int D3D12Context::GetFirstAvailableCbvSrvUavIndex()
{
    int i = m_availableCbvSrvUavIndices.front();
    m_availableCbvSrvUavIndices.erase(m_availableCbvSrvUavIndices.begin());
    return i;
}

void D3D12Context::FreeRtvIndex(int _index)
{
    m_availableRtvIndices.push_back(_index);
}

void D3D12Context::FreeDsvIndex(int _index)
{
    m_availableDsvIndices.push_back(_index);
}

void D3D12Context::FreeCbvSrvUavIndex(int _index)
{
    m_availableCbvSrvUavIndices.push_back(_index);
}

D3D12Context::D3D12Context()
{
    Init();
}

D3D12Context::~D3D12Context()
{
    m_pFactory->Release();
    m_pDevice->Release();
    m_pFence->Release();
    m_pCommandQueue->Release();
    m_pCommandAllocator->Release();
    m_pCommandList->Release();
    m_pRtvHeap->Release();      
    m_pDsvHeap->Release();      
    m_pCbvSrvUavHeap->Release();
}



#endif
