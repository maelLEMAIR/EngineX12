#include "D3D12Font.h"

#include <iostream>

#include "../Shader-Mat/D3D12ComputeShader.h"

D3D12Font::~D3D12Font()
{
    if (m_atlas != nullptr)
        delete m_atlas;
    if (m_shader != nullptr)
        delete m_shader;
}

D3D12Font::D3D12Font(D3D12Device* _pDevice, D3D12Context* _pContext)
{
    m_pDevice = _pDevice;
    m_pContext = _pContext;
}

void D3D12Font::Bake()
{
    if (m_atlas == nullptr)
    {
        m_atlas = dynamic_cast<D3D12RenderTarget*>(m_pDevice->CreateRenderTarget(2048, 2048));
        m_atlas->Init();
    }

    if (m_shader == nullptr)
    {
        m_shader = dynamic_cast<D3D12ComputeShader*>(
            m_pDevice->CreateComputeShader(RES("/Shaders/ComputeShaders/FontBake.hlsl")));
        m_shader->AddBinding("CharData", ResBindingType::CBV, 0, 0);
        m_shader->AddBinding("LineBuffer", ResBindingType::UAV, 1, 0);
        m_shader->AddBinding("Output", ResBindingType::RTV, 2, 0);

        m_shader->SetRTV("Output", m_atlas);
    }

    float lineHeight = m_lineHeight;
    float ascender   = m_ascender;

    float xCoord    = 0.0f;
    float yCoord    = ascender + 20;
    float rowHeight = lineHeight;

    m_pContext->FlushCommandQueue();

    for (size_t i = 0; i < m_availableCharacterCodes.size(); i++)
    {
        Character* glyph = &m_characters[m_availableCharacterCodes[i]];
        
        if (glyph->minX < 0)
            xCoord -= glyph->minX;
        if (glyph->minY + ascender < 0)
            yCoord += glyph->minY + ascender;
        
        float cellXMin = min(xCoord, xCoord + glyph->minX);
        float cellXMax = max(xCoord + glyph->maxX + 1, xCoord + glyph->advanceWidth);
        float cellW = cellXMax - cellXMin;
        
        float cellYMin = min(yCoord - ascender, yCoord - glyph->maxY);
        float cellYMax = max(cellYMin + lineHeight, yCoord - glyph->minY);
        float cellH = cellYMax - cellYMin;

        bool recalculate = false;
        if (xCoord + cellW >= 2048)
        {
            xCoord = 0.0f;
            yCoord += rowHeight;
            rowHeight = lineHeight;
            recalculate = true;
        }

        if (recalculate)
        {
            cellXMin = min(xCoord, xCoord + glyph->minX);
            if (cellXMin < 0)
            {
                xCoord -= cellXMin;
                cellXMin = min(xCoord, xCoord + glyph->minX);
            }
            cellXMax = max(xCoord + glyph->maxX + 1, xCoord + glyph->advanceWidth);
            cellW = cellXMax - cellXMin;
            
            cellYMin = min(yCoord - ascender, yCoord - glyph->maxY);
            cellYMax = max(cellYMin + lineHeight, yCoord - glyph->minY);
            cellH = cellYMax - cellYMin;
        }

        glyph->atlasX = (uint16)cellXMin;
        glyph->atlasY = (uint16)cellYMin;
        glyph->atlasWidth = (uint16)cellW;
        glyph->atlasHeight = (uint16)cellH;
        
        Vector<FloatLine> lines;
        for (auto& line : glyph->lines)
            lines.push_back({ (float)line.x1, (float)line.y1, (float)line.x2, (float)line.y2 });

        CharData data;
        data.minX = glyph->minX;
        data.minY = glyph->minY;
        data.maxX = glyph->maxX;
        data.maxY = glyph->maxY;
        data.penX = xCoord;
        data.penY = yCoord;
        data.spread = 2.0f;
        data.smoothing = 0.1f;
        data.lineCount = (uint32)lines.size();
        data.canvasWidth  = 2048;
        data.canvasHeight = 2048;

        uint32 pixelW = (uint32)(glyph->maxX - glyph->minX + 1);
        uint32 pixelH = (uint32)(glyph->maxY - glyph->minY + 1);

        if (pixelW == 0 || pixelH == 0 || lines.empty())
        {
            rowHeight = max(rowHeight, cellH);
            xCoord += cellW;
            continue;
        }

        m_pContext->ResetCmdList();
        
        m_shader->SetUnorderedAccess("LineBuffer", lines);
        m_shader->SetConstant("CharData", data);
        
        m_shader->Bind();
        m_shader->SetThreadGroupSize(
            (uint32)ceil(pixelW / 8.0f),
            (uint32)ceil(pixelH / 8.0f),
            1
        );
        m_shader->Dispatch();
        m_shader->FlushUAVWrites();
        
        m_pContext->CloseCmdList();
        m_pContext->FlushCommandQueue();
        
        rowHeight = max(rowHeight, cellH);
        xCoord += cellW;
    }
}
