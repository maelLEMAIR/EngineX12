#include "D3D12Text.h"
#include "../Base/D3D12Device.h"
#include "../FontRendering/D3D12Font.h"
#include "Generic/Shader-Mat/Shader.h"

D3D12Text::D3D12Text(D3D12Device* _pDevice, D3D12Context* _pContext)
{
    m_pDevice  = _pDevice;
    m_pContext = _pContext;
    m_sprite   = _pDevice->CreateSprite(true);
    m_colorCB  = new UploadBuffer<TextColorCB>(_pContext, 1, true);
    
    ShaderFormat sf;
    m_shader   = _pDevice->CreateUiShader(RES("Shaders/TextShader.hlsl"), sf);
}

void D3D12Text::BuildVertices(Vector<UiVertex>& _vertices, Vector<uint32>& _indices)
{
    _vertices.clear();
    _indices.clear();

    if (!m_font || m_string.empty()) return;

    float atlasSize = 2048.0f;
    float penX = m_x;
    float penY = m_y;

    uint32 baseIndex = 0;

    for (char c : m_string)
    {
        if (c == '\n')
        {
            penX  = m_x;
            penY += m_font->GetLineHeight();
            continue;
        }

        Character* glyph = m_font->GetCharacter((uint16)c);
        if (!glyph || glyph->atlasWidth == 0 || glyph->atlasHeight == 0)
        {
            penX += glyph ? glyph->advanceWidth : 0;
            continue;
        }
        
        float u0 = glyph->atlasX / atlasSize;
        float v0 = glyph->atlasY / atlasSize;
        float u1 = (glyph->atlasX + glyph->atlasWidth + 1)  / atlasSize;
        float v1 = (glyph->atlasY + glyph->atlasHeight + 1) / atlasSize;
        
        float x0 = penX + glyph->leftSideBearing;
        float y0 = penY;
        float x1 = x0 + glyph->atlasWidth;  
        float y1 = y0 + glyph->atlasHeight;

        _vertices.push_back({ Vect2f32(x0, y0), Vect2f32(u0, v0), m_color });
        _vertices.push_back({ Vect2f32(x1, y0), Vect2f32(u1, v0), m_color });
        _vertices.push_back({ Vect2f32(x0, y1), Vect2f32(u0, v1), m_color });
        _vertices.push_back({ Vect2f32(x1, y1), Vect2f32(u1, v1), m_color });

        _indices.push_back(baseIndex + 0);
        _indices.push_back(baseIndex + 1);
        _indices.push_back(baseIndex + 2);
        _indices.push_back(baseIndex + 2);
        _indices.push_back(baseIndex + 1);
        _indices.push_back(baseIndex + 3);

        baseIndex += 4;
        penX += glyph->advanceWidth;
    }
}

void D3D12Text::Build()
{
    if (!m_dirty) return;
    m_dirty = false;

    Vector<UiVertex> vertices;
    Vector<uint32>   indices;
    BuildVertices(vertices, indices);

    if (vertices.empty()) return;

    m_sprite->SetVertexData(vertices.data(), vertices.size());
    m_sprite->SetIndexData(indices.data(),   indices.size());

    TextColorCB cb;
    cb.color = m_color;
    m_colorCB->Reset();
    m_colorCB->Append(cb);
}

void D3D12Text::Bind()
{
    D3D12Font* font = dynamic_cast<D3D12Font*>(m_font);
    assert(font != nullptr && "Font must be a D3D12Font");

    D3D12RenderTarget* atlas = dynamic_cast<D3D12RenderTarget*>(font->GetAtlas());
    assert(atlas != nullptr);

    m_shader->Bind();
    
    m_pContext->GetCommandList()->SetGraphicsRootConstantBufferView(
        1, m_colorCB->GetGPUAddress(0));

    atlas->SetInput();
    atlas->BindAsTexture(3);
}

D3D12Text::~D3D12Text()
{
    delete m_colorCB;
}
