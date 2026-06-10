#ifndef D3D12DEVICE_H_DEFINED
#define D3D12DEVICE_H_DEFINED

#include "../../Generic/Base/Device.h"
#include "D3D12Context.h"

#include "../../Generic/Base/Camera.h"
#include "../../Generic/Shader-Mat/Lights.hpp"
#include "../../Generic/Shader-Mat/ShaderFormat.h"
#include "Generic/FontRendering/Text.hpp"

template <typename T>
class UploadBuffer;

class D3D12Device : public Device
{
public:
    D3D12Device();
    bool Init() override;
    void InitBuffer();

    ~D3D12Device() override;
    
    void BeginDraw(RenderTarget* _pRenderTarget, DepthStencil* _pDepthStencil) override;
    void Blit(RenderTarget* _pRenderTarget = nullptr) override;
    void EndDraw() override;

    void RunComputeShader(ComputeShader* _pComputeShader, bool _isOnRT) override;
    
    void Draw(Geometry* _geo, XMFLOAT4X4& _mat) override;
    void DrawUi(Sprite* _sprite, XMFLOAT4X4& _mat) override;
    void DrawRenderText(Text* _text, XMFLOAT4X4& _mat) override;

    void SetViewport(int _width, int _height) override;

    void SetLights(Vector<LightDescriptor>& _vLights) override;

    Shader* CreateShader(WString const& _path, ShaderFormat _format, bool _isLit = false) override;
    UiShader* CreateUiShader(WString const& _path, ShaderFormat _format) override;
    Geometry* CreateGeometry(bool _isDynamic = false) override;
    Sprite* CreateSprite(bool _isDynamic) override;
    RenderTarget* CreateRenderTarget(int _width, int _height) override;
    Texture* CreateTexture(WString const& _path) override;
    ComputeShader* CreateComputeShader(WString const& _path) override;
    RenderFont* CreateRenderFont(WString const& _path, float _fontSize) override;
    Text* CreateText(RenderFont* _pFont) override;
    
private:
    D3D12Context m_pContext;

    D3D12_VIEWPORT m_screenViewport; 
    D3D12_RECT m_scissorRect;

    Vector<UploadBuffer<XMFLOAT4X4>*> m_vPerObjectBuffers;
    int m_objCbIndex = 0;
    
    PassData m_passData;
    UploadBuffer<PassData>* m_pPerPassBuffer = nullptr;

    LightData m_lightData;
    UploadBuffer<LightData>* m_pLightBuffer = nullptr;

    UiShader* m_blitShader = nullptr;

    D3D12_GPU_VIRTUAL_ADDRESS GetObjectCBAdress(XMFLOAT4X4 _mat);
    void ResetPerObjectBuffers();
    
    friend class D3D12RenderTarget;
    friend class D3D12DepthStencil;
    friend class D3D12Geometry;
    friend class Window;
    friend class D3D12Font;
};

#endif
