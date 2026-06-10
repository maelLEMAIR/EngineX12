#ifndef RENDER_TARGET_H_DEFINED
#define RENDER_TARGET_H_DEFINED

#include "../../Common/Common.h"

class RenderTarget
{
public:
    virtual ~RenderTarget() = default;

    virtual void Init() = 0;
    virtual void Resize(int _width, int _height) = 0;

    virtual void SetToWrite() = 0;
    virtual void SetToRead() = 0;
    virtual void SetComputeOutput() = 0;
    virtual void SetInput() = 0;

    virtual void Clear(XMFLOAT3 _color) = 0;

    virtual void BindAsTexture(uint32 _rootIndex) = 0;
    
    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }

protected:
    int m_width = 0;
    int m_height = 0;

    XMFLOAT3 m_clearColor = XMFLOAT3(0.0f, 0.0f, 0.0f);
    
    friend class Device;
    friend class D3D12Device;
};

#endif
