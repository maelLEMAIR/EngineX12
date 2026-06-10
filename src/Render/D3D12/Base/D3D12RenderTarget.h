#ifndef D3D12_RENDER_TARGET_H_DEFINED
#define D3D12_RENDER_TARGET_H_DEFINED

#include "../../Generic/Base/RenderTarget.h"
#include "../define.h"

class D3D12Context;

class D3D12RenderTarget : public RenderTarget
{
public:
    ~D3D12RenderTarget() override;

    ID3D12Resource* GetResource() const { return m_resource;}
    
    int GetRtvHeapIndex() const { return m_rtvHeapIndex; }
    int GetSRVHeapIndex() const { return m_srvHeapIndex; }
    int GetUAVHeapIndex() const { return m_uavHeapIndex; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle();
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle();
    D3D12_GPU_DESCRIPTOR_HANDLE GetUavHandle();
    
    void SetResource(ID3D12Resource* resource, bool _isBackBuffer = false);
    void Init() override;

    void Resize(int _width, int _height) override;

    void Clear(XMFLOAT3 _color) override;
    void BindAsTexture(uint32 _rootIndex);

    void SetToWrite() override;
    void SetToRead() override;
    void SetComputeOutput() override;
    void SetInput() override;

private:
    ID3D12Resource* m_resource = nullptr;
    int m_rtvHeapIndex = -1;
    int m_srvHeapIndex = -1;
    int m_uavHeapIndex = -1;

    D3D12Context* m_pContext = nullptr;
    D3D12RenderTarget(int width, int height, D3D12Context* _pContext, XMFLOAT3 _clearColor = XMFLOAT3(0.0f, 0.0f, 0.0f));

    D3D12_RESOURCE_STATES m_currState = D3D12_RESOURCE_STATE_PRESENT;
    
    void CreateComputeDescriptors();

    friend class D3D12Device;
    friend class D3D12SwapChain;
};

#endif
