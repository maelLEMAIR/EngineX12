#ifndef D3D12_DEPTH_STENCIL_H_DEFINED
#define D3D12_DEPTH_STENCIL_H_DEFINED

#include "../../Generic/Base/DepthStencil.h"
#include "D3D12/define.h"

class D3D12Context;

class D3D12DepthStencil : public DepthStencil
{
public:
    ~D3D12DepthStencil() override;

    ID3D12Resource* GetResource() const { return m_resource;}
    int GetHeapIndex() const { return m_heapIndex; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();

    void Init();
    void Resize(int _width, int _height) override;
    void Clear() override;

private:
    ID3D12Resource* m_resource = nullptr;
    int m_heapIndex = -1;

    D3D12Context* m_pContext = nullptr;
    
    D3D12DepthStencil(int _width, int _height, D3D12Context* _pContext);

    friend class D3D12Context;
    friend class D3D12SwapChain;
};

#endif
