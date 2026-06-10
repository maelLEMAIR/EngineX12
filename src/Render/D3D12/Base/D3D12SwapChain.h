#ifndef D3D12SWAPCHAIN_H_DEFINED
#define D3D12SWAPCHAIN_H_DEFINED

#include "../define.h"
#include "../../Generic/Base/SwapChain.h"

class D3D12Context;

class D3D12SwapChain : public SwapChain
{
public:
    D3D12SwapChain(int _width, int _height, D3D12Context* _pContext, HWND _hwnd);
    ~D3D12SwapChain() override;

    void Present() override;
    void Resize(int _width, int _heigth) override;

    IDXGISwapChain3* GetSwapChain() { return m_pSwapChain; }
    
private:
    D3D12Context* m_pContext = nullptr;
    IDXGISwapChain3* m_pSwapChain = nullptr;
    
};

#endif
