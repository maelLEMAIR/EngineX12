#ifndef D3D12FONT_H_DEFINED
#define D3D12FONT_H_DEFINED

#include "../../Generic/FontRendering/Font.h"
#include "../Base/D3D12Device.h"
#include "../Base/D3D12RenderTarget.h"

class D3D12ComputeShader;
class D3D12Device;
class D3D12Context;

struct FloatLine
{
    float x1, y1, x2, y2;
};

class D3D12Font : public RenderFont
{
public:
    ~D3D12Font() override;

    RenderTarget* GetAtlas() override { return m_atlas; };
private:
    D3D12RenderTarget* m_atlas = nullptr;
    D3D12ComputeShader* m_shader = nullptr;

    D3D12Device* m_pDevice = nullptr;
    D3D12Context* m_pContext = nullptr;

    D3D12Font(D3D12Device* _pDevice, D3D12Context* _pRenderTarget);
    
    void Bake() override;

    friend class D3D12Device;
};

#endif
