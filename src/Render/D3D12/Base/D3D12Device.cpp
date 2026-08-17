#include "D3D12Device.h"

#include "D3D12RenderTarget.h"
#include "D3D12DepthStencil.h"

#include "../RenderItems/D3D12Geometry.h"
#include "../RenderItems/D3D12Sprite.h"

#include "../Shader-Mat/D3D12Material.h"
#include "../Shader-Mat/D3D12Shader.h"
#include "../Shader-Mat/D3D12UiShader.h"
#include "../Shader-Mat/D3D12Texture.h"
#include "../Shader-Mat/D3D12ComputeShader.h"

#include "../Buffers/UploadBuffer.hpp"

#include "../../Generic/Factories/ShaderFactory.hpp"

#include "../../Generic/Base/RenderTarget.h"
#include "D3D12/FontRendering/D3D12Font.h"
#include "D3D12/FontRendering/D3D12Text.h"
#include "Generic/FontRendering/Text.hpp"

#pragma comment(lib, "d2d1.lib")

D3D12Device::D3D12Device() : m_pContext()
{
    
}

bool D3D12Device::Init()
{
    m_blitShader = ShaderFactory::CreateBlitShader(this);
    
    return true;
}

void D3D12Device::InitBuffer()
{
    m_pPerPassBuffer = new UploadBuffer<PassData>(&m_pContext, 1);
    m_pLightBuffer = new UploadBuffer<LightData>(&m_pContext, 1);
    m_pLightBuffer->CopyData(0, m_lightData);
}

D3D12Device::~D3D12Device()
{
    // TODO Destroy upload buffers
}

void D3D12Device::BeginDraw(RenderTarget* _pRenderTarget, DepthStencil* _pDepthStencil)
{
    if (_pRenderTarget == nullptr) return;
    
    D3D12RenderTarget* rt = dynamic_cast<D3D12RenderTarget*>(_pRenderTarget);
    if (rt == nullptr) return;

    D3D12RenderTarget* blitRT = dynamic_cast<D3D12RenderTarget*>(m_blitRT);
    if (blitRT == nullptr) return;
    
    m_pRenderTarget = _pRenderTarget;
    m_pDepthStencil = _pDepthStencil;
    
    m_pContext.ResetCmdList();

    blitRT->SetToWrite();
    
    blitRT->Clear(m_clearColor);
    if (m_pDepthStencil != nullptr)
    {
        m_pDepthStencil->Clear();
    }

    m_pContext.GetCommandList()->RSSetViewports(1, &m_screenViewport);
    m_pContext.GetCommandList()->RSSetScissorRects(1, &m_scissorRect);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = blitRT->GetRtvHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE pDsvHandle;
    
    if (m_pDepthStencil != nullptr)
    {
        D3D12DepthStencil* dsv = dynamic_cast<D3D12DepthStencil*>(m_pDepthStencil);
        if (dsv != nullptr)
        {
            pDsvHandle = dsv->GetHandle();
            m_pContext.GetCommandList()->OMSetRenderTargets(1,
        &rtvHandle, FALSE, &pDsvHandle);
        }
    }
    else
    {
        m_pContext.GetCommandList()->OMSetRenderTargets(1,
            &rtvHandle, FALSE, nullptr);   
    }

    if (m_pMainCamera != nullptr)
    {
        m_pMainCamera->SetAspectRatio(m_screenViewport.Width / m_screenViewport.Height);
        m_pMainCamera->FillData(&m_passData);
        m_passData.renderTargetSize = Vect2f32((float)m_pRenderTarget->m_width, (float)m_pRenderTarget->m_height);
    }
    
    m_pPerPassBuffer->CopyData(0, m_passData);

    ResetPerObjectBuffers();
}

void D3D12Device::Draw(Geometry* _geo, Mat4f32 const& _mat)
{
    assert(m_pCurrMaterial != nullptr && "No material selected");

    if (m_pMainCamera == nullptr) return;

    if (!_geo->FrustumCheck(m_pMainCamera->GetFrustum(), _mat))
    {
        // std::cout << "Object culled" << std::endl;
        return;
    }
    
    m_pCurrMaterial->Bind();
    m_pContext.GetCommandList()->SetGraphicsRootConstantBufferView(0, GetObjectCBAdress(_mat));
    m_pContext.GetCommandList()->SetGraphicsRootConstantBufferView(2, m_pPerPassBuffer->GetGPUAddress(0));

    if (m_pCurrMaterial->IsLit())
        m_pContext.GetCommandList()->SetGraphicsRootConstantBufferView(3, m_pLightBuffer->GetGPUAddress(0));

    D3D12Geometry* geo = dynamic_cast<D3D12Geometry*>(_geo);

    assert(geo != nullptr && "Unusable geometry type");
    
    m_pContext.GetCommandList()->IASetPrimitiveTopology(geo->GetD3DTopology());
    
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = geo->VertexBufferView();
    D3D12_INDEX_BUFFER_VIEW indexBufferView = geo->IndexBufferView();

    m_pContext.GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    m_pContext.GetCommandList()->IASetIndexBuffer(&indexBufferView);

    if (geo->IsIndexed())
        m_pContext.GetCommandList()->DrawIndexedInstanced((UINT)geo->GetIndexCount(), 1, 0, 0, 0);
    else
        m_pContext.GetCommandList()->DrawInstanced((UINT)geo->GetVertexCount(), 1, 0, 0);
}

void D3D12Device::Blit(RenderTarget* _renderTarget)
{
    if (m_pRenderTarget == nullptr) return;

    D3D12RenderTarget* rt = dynamic_cast<D3D12RenderTarget*>(m_pRenderTarget);
    if (rt == nullptr) return;

    D3D12RenderTarget* blitRT = nullptr;
    if (_renderTarget != nullptr)
        blitRT = dynamic_cast<D3D12RenderTarget*>(_renderTarget);
    else
        blitRT = dynamic_cast<D3D12RenderTarget*>(m_blitRT);
    if (blitRT == nullptr) return;

    ID3D12GraphicsCommandList* cmd = m_pContext.GetCommandList();
    
    blitRT->SetInput();
    rt->SetToWrite();
    
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rt->GetRtvHandle();

    m_pContext.GetCommandList()->OMSetRenderTargets(1,
           &rtvHandle, FALSE, nullptr);

    m_blitShader->Bind();

    cmd->SetGraphicsRootDescriptorTable(0, blitRT->GetSrvHandle());
    cmd->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->DrawInstanced(3, 1, 0, 0);
}

void D3D12Device::EndDraw()
{
    if (m_pRenderTarget == nullptr) return;

    Blit();
    
    m_pRenderTarget->SetToRead();

    m_pContext.CloseCmdList();
    
    m_pRenderTarget = nullptr;
    m_pDepthStencil = nullptr;
}

void D3D12Device::RunComputeShader(ComputeShader* _pComputeShader, bool _isOnRT)
{
    _pComputeShader->Bind();
    if (_isOnRT)
        _pComputeShader->CalculateGroupSizes();
    _pComputeShader->Dispatch();
    _pComputeShader->FlushUAVWrites();
}

void D3D12Device::DrawUi(Sprite* _sprite, Mat4f32 const& _mat)
{
    assert(m_pCurrUiMaterial != nullptr && "No material selected");

    if (m_pMainCamera == nullptr) return;

    m_pCurrUiMaterial->Bind();
    m_pContext.GetCommandList()->SetGraphicsRootConstantBufferView(0, GetObjectCBAdress(_mat));
    m_pContext.GetCommandList()->SetGraphicsRootConstantBufferView(2, m_pPerPassBuffer->GetGPUAddress(0));

    D3D12Sprite* sprite = dynamic_cast<D3D12Sprite*>(_sprite);

    assert(sprite != nullptr && "Unusable geometry type");

    // TODO use per geometry topology
    m_pContext.GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = sprite->VertexBufferView();
    D3D12_INDEX_BUFFER_VIEW indexBufferView = sprite->IndexBufferView();

    m_pContext.GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    m_pContext.GetCommandList()->IASetIndexBuffer(&indexBufferView);

    if (sprite->IsIndexed())
        m_pContext.GetCommandList()->DrawIndexedInstanced((UINT)sprite->GetIndexCount(), 1, 0, 0, 0);
    else
        m_pContext.GetCommandList()->DrawInstanced((UINT)sprite->GetVertexCount(), 1, 0, 0);
}

void D3D12Device::DrawRenderText(Text* _text, Mat4f32 const& _mat)
{
    if (_text->IsDirty()) _text->Build();

    Sprite* sprite = _text->GetSprite();
    if (!sprite || sprite->GetVertexCount() == 0) return;

    D3D12Text* text = dynamic_cast<D3D12Text*>(_text);
    assert(text != nullptr && "Unusable text type");

    text->Bind();

    m_pContext.GetCommandList()->SetGraphicsRootConstantBufferView(0, GetObjectCBAdress(_mat));
    m_pContext.GetCommandList()->SetGraphicsRootConstantBufferView(2, m_pPerPassBuffer->GetGPUAddress(0));
    

    D3D12Sprite* d3dSprite = dynamic_cast<D3D12Sprite*>(sprite);
    assert(d3dSprite != nullptr);

    m_pContext.GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D12_VERTEX_BUFFER_VIEW vbv = d3dSprite->VertexBufferView();
    D3D12_INDEX_BUFFER_VIEW  ibv = d3dSprite->IndexBufferView();

    m_pContext.GetCommandList()->IASetVertexBuffers(0, 1, &vbv);
    m_pContext.GetCommandList()->IASetIndexBuffer(&ibv);

    m_pContext.GetCommandList()->DrawIndexedInstanced(
        (UINT)sprite->GetIndexCount(), 1, 0, 0, 0);
}

void D3D12Device::SetViewport(int _width, int _height)
{
    m_screenViewport.TopLeftX = 0;
    m_screenViewport.TopLeftY = 0;
    m_screenViewport.Width    = static_cast<float>(_width);
    m_screenViewport.Height   = static_cast<float>(_height);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;

    m_scissorRect = { 0, 0, _width, _height };

    m_passData.renderTargetSize = {(float)_width, (float)_height};

    if (m_blitRT == nullptr)
    {
        m_blitRT = CreateRenderTarget(_width, _height);
        m_blitRT->Init();
    }
    else
        m_blitRT->Resize(_width, _height);
}

void D3D12Device::SetLights(Vector<LightDescriptor>& _vLights)
{
    m_lightData = LightHelper::GetLightData(_vLights);

    m_pLightBuffer->CopyData(0, m_lightData);
}

Shader* D3D12Device::CreateShader(WString const& _path, ShaderFormat _format, ShaderDescriptor _desc)
{
    D3D12Shader* shader = new D3D12Shader(&m_pContext, _format, _desc);
    shader->Compile(_path);
    return shader;
}

UiShader* D3D12Device::CreateUiShader(WString const& _path, ShaderFormat _format, ShaderDescriptor _desc)
{
    D3D12UiShader* shader = new D3D12UiShader(&m_pContext, _format, _desc);
    shader->Compile(_path);
    return shader;
}

Geometry* D3D12Device::CreateGeometry(bool _isDynamic)
{
    return new D3D12Geometry(&m_pContext, _isDynamic);
}

Sprite* D3D12Device::CreateSprite(bool _isDynamic)
{
    return new D3D12Sprite(&m_pContext, _isDynamic);
}

RenderTarget* D3D12Device::CreateRenderTarget(int _width, int _height)
{
    return new D3D12RenderTarget(_width, _height, &m_pContext, m_clearColor);
}

Texture* D3D12Device::CreateTexture(WString const& _path)
{
    D3D12Texture* texture = new D3D12Texture(&m_pContext);
    texture->Load(_path);

    return texture;
}

ComputeShader* D3D12Device::CreateComputeShader(WString const& _path)
{
    D3D12ComputeShader* shader = new D3D12ComputeShader(&m_pContext);
    shader->Compile(_path);
    return shader;
}

RenderFont* D3D12Device::CreateRenderFont(WString const& _path, float _fontSize)
{
    D3D12Font* font = new D3D12Font(this, &m_pContext);
    font->Load(_path, _fontSize);
    return font;
}

Text* D3D12Device::CreateText(RenderFont* _pFont)
{
    D3D12Text* text = new D3D12Text(this, &m_pContext);
    text->SetFont(_pFont);

    return text;
}

D3D12_GPU_VIRTUAL_ADDRESS D3D12Device::GetObjectCBAdress(Mat4f32 const& _mat)
{
    if (m_vPerObjectBuffers.size() <= m_objCbIndex)
    {
        m_vPerObjectBuffers.push_back(new UploadBuffer<Mat4f32>(&m_pContext, 1000));
    }

    UploadBuffer<Mat4f32>* pCurrBuffer = m_vPerObjectBuffers[m_objCbIndex];

    UINT bufferIndex = (UINT)pCurrBuffer->GetFirstAvailable();

    pCurrBuffer->CopyData(bufferIndex, _mat.Transposed());

    if (bufferIndex == 999)
    {
        m_objCbIndex++;
        if (m_objCbIndex >= m_vPerObjectBuffers.size())
            m_vPerObjectBuffers.push_back(new UploadBuffer<Mat4f32>(&m_pContext, 1000));
    }

    D3D12_GPU_VIRTUAL_ADDRESS addr = pCurrBuffer->GetGPUAddress(bufferIndex);
    return addr;
}

void D3D12Device::ResetPerObjectBuffers()
{
    m_objCbIndex = 0;
    for (UploadBuffer<Mat4f32>* buffer : m_vPerObjectBuffers)
    {
        buffer->Reset();
    }
}


