#ifndef SWAPCHAIN_H_DEFINED
#define SWAPCHAIN_H_DEFINED

#include "../../Common/Common.h"

class RenderTarget;
class DepthStencil;

class SwapChain
{
public:
    virtual ~SwapChain() = default;
    
    virtual void Present() = 0;
    virtual void Resize(int _width, int _heigth) = 0;

    RenderTarget* GetBackBuffer() { return m_vRenderTargets[m_backBufferIndex]; }
    DepthStencil* GetDepthStencil() { return m_pDepthStencil; }
    
protected:
    Vector<RenderTarget*> m_vRenderTargets;
    DepthStencil* m_pDepthStencil = nullptr;
    
    UINT m_backBufferIndex = 0;
    const int m_bufferCount = 2; 
};

#endif
