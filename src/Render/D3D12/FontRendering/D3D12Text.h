#ifndef D3D12TEXT_H_DEFINED
#define D3D12TEXT_H_DEFINED

#include "../../Generic/FontRendering/Text.hpp"
#include "../Base/D3D12Context.h"
#include "../Buffers/UploadBuffer.hpp"

class UiShader;
class D3D12Device;
class D3D12RenderTarget;

struct TextColorCB
{
    Vect4f32 color;
};

class D3D12Text : public Text
{
public:
    void Build() override;
    void Bind() override;

    ~D3D12Text() override;

private:
    D3D12Text(D3D12Device* _pDevice, D3D12Context* _pContext);

    void BuildVertices(Vector<UiVertex>& _vertices, Vector<uint32>& _indices);

    D3D12Device*                m_pDevice   = nullptr;
    D3D12Context*               m_pContext  = nullptr;
    UploadBuffer<TextColorCB>*  m_colorCB   = nullptr;
    UiShader*                     m_shader    = nullptr;

    friend class D3D12Device;
};

#endif
