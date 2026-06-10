#include "D3D12Material.h"

#include "../Buffers/UploadBuffer.hpp"
#include "../Base/D3D12Context.h"

#include "../../Generic/Shader-Mat/Texture.h"

D3D12Material::~D3D12Material()
{
    delete m_pUploadBuffer;
}

void D3D12Material::Bind()
{
    uint64 bufferSize = m_pUploadBuffer->GetElementByteSize();
    Vector<char> data(bufferSize, 0);

    CopyToBuffer(data.data());
    m_pUploadBuffer->CopyRawData(0, data.data());

    m_pShader->Bind();

    uint32 startIndex = m_pShader->IsLit() ? 4 : 3;
    
    for (auto [name, texture] : m_textures)
    {
        if (texture == nullptr) continue;

        uint32 index = startIndex +  m_pShader->GetFormat().GetTextures().at(name);
        
        texture->Bind(index);
    }
    
    m_pContext->GetCommandList()->SetGraphicsRootConstantBufferView(1, m_pUploadBuffer->GetGPUAddress(0));
}

D3D12Material::D3D12Material(D3D12Context* _pContext, Shader* _pShader) : Material(_pShader)
{
    m_pShader = _pShader;
    m_pContext = _pContext;
    m_pUploadBuffer = new UploadBufferBase(_pContext, _pShader->GetFormat().GetTotalSize(), 1);
}
