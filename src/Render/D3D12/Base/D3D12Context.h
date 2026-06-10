#ifndef GRAPHICS_CONTEXT_H_DEFINED
#define GRAPHICS_CONTEXT_H_DEFINED

#include "D3D12.h"
#include <d2d1_1.h>
#include <d3d11on12.h>

#pragma comment(lib, "dwrite.lib")

#include "../../Common/Common.h"

class D3D12Context 
{
public:
    bool Init();
    void FlushCommandQueue();
    void ResetCmdList();
    void CloseCmdList();
    
    IDXGIFactory*                GetDXGIFactory()        { return m_pFactory; }
    ID3D12Device*                GetDevice()             { return m_pDevice; }
    ID3D12Fence*                 GetFence()              { return m_pFence; }
    ID3D12CommandQueue*          GetCommandQueue()       { return m_pCommandQueue; }
    ID3D12CommandAllocator*      GetCommandAllocator()   { return m_pCommandAllocator; }
    ID3D12GraphicsCommandList*   GetCommandList()        { return m_pCommandList; }
    
    ID3D12DescriptorHeap*        GetRtvHeap()            { return m_pRtvHeap; }
    ID3D12DescriptorHeap*        GetDsvHeap()            { return m_pDsvHeap; }
    ID3D12DescriptorHeap*        GetCbvSrvUavHeap()      { return m_pCbvSrvUavHeap; }
    
    UINT                         GetCurrentFenceValue()  { return m_currentFence; }
    UINT                         Get4xMsaaQuality()      { return m_4xMsaaQuality; }

    int GetFirstAvailableRtvIndex();
    int GetFirstAvailableDsvIndex();
    int GetFirstAvailableCbvSrvUavIndex();
    void FreeRtvIndex(int _index);
    void FreeDsvIndex(int _index);
    void FreeCbvSrvUavIndex(int _index);
    
    UINT32 m_rtvDescriptorSize;
    UINT32 m_dsvDescriptorSize;
    UINT32 m_cbvSrvDescriptorSize;
    
    D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

private:

    D3D12Context();
    ~D3D12Context();
    
    IDXGIFactory* m_pFactory = nullptr;
    ID3D12Device* m_pDevice = nullptr;

    ID3D12Fence* m_pFence = nullptr;
    UINT m_currentFence = 0;

    ID3D12CommandQueue* m_pCommandQueue = nullptr;
    ID3D12CommandAllocator* m_pCommandAllocator = nullptr;
    ID3D12GraphicsCommandList* m_pCommandList = nullptr;
    
    /// DESCRIPTOR HEAPS

    int m_rtvCount = 16;
    int m_dsvCount = 16;
    int m_cbvSrvUavCount = 128;
    
    ID3D12DescriptorHeap* m_pRtvHeap        = nullptr;
    ID3D12DescriptorHeap* m_pDsvHeap        = nullptr;
    ID3D12DescriptorHeap* m_pCbvSrvUavHeap  = nullptr;

    Vector<int> m_availableRtvIndices;
    Vector<int> m_availableDsvIndices;
    Vector<int> m_availableCbvSrvUavIndices;
    
    UINT m_4xMsaaQuality = 1;

    friend class D3D12Device;
};

#endif
