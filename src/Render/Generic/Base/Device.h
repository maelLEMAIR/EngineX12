#ifndef DEVICE_H_DEFINED
#define DEVICE_H_DEFINED

#include "../../Common/Common.h"

#include "DepthStencil.h"
#include "../Shader-Mat/Lights.hpp"
#include "../Shader-Mat/ShaderFormat.h"

class Text;
class ComputeShader;
class Sprite;
class UiMaterial;
class UiShader;
class RenderTarget;
class Geometry;
class Texture;
class Material;
class Shader;

class RenderFont;
class Brush;
struct RectanglePos;

class Camera;

class Device 
{
public:
    Device() = default;
    virtual ~Device() = default;

    virtual bool Init() { return true; }

    virtual void BeginDraw(RenderTarget* _pRenderTarget, DepthStencil* _pDepthStencil = nullptr) = 0;
    virtual void EndDraw() = 0;

    virtual void RunComputeShader(ComputeShader* _pComputeShader, bool _isOnRT = false) = 0;

    virtual void Blit(RenderTarget* _pRt = nullptr) = 0;
    virtual void Draw(Geometry* _geo, Mat4f32 const& _mat) = 0;
    virtual void DrawUi(Sprite* _sprite, Mat4f32 const& _mat) = 0;
    virtual void DrawRenderText(Text* _text, Mat4f32 const& _mat) = 0;

    void SetMaterial(Material* _material) { m_pCurrMaterial = _material; }
    void SetUiMaterial(UiMaterial* _material) { m_pCurrUiMaterial = _material; }
    void SetMainCamera(Camera* _pCamera) { m_pMainCamera = _pCamera; }
    void SetClearColor(Vect3f32 const& _color) { m_clearColor = _color; }

    RenderTarget* GetBlitRT() { return m_blitRT; } 
    
    virtual void SetLights(Vector<LightDescriptor>& _vLights) = 0;
    
    virtual RenderTarget* CreateRenderTarget(int _width, int _height)                                           = 0;
    virtual Geometry* CreateGeometry(bool _isDynamic = false)                                                   = 0;
    virtual Sprite*   CreateSprite(bool _isDynamic = false)                                                     = 0;
    virtual Texture* CreateTexture(WString const& _path)                                                        = 0;
    virtual Shader* CreateShader(WString const& _path, ShaderFormat _format, ShaderDescriptor _desc = {})       = 0; 
    virtual UiShader* CreateUiShader(WString const& _path, ShaderFormat _format, ShaderDescriptor _desc = {})   = 0;
    virtual ComputeShader* CreateComputeShader(WString const& _path)                                            = 0;
    virtual RenderFont* CreateRenderFont(WString const& _path, float _fontSize = 1.0f)                          = 0;
    virtual Text* CreateText(RenderFont* _pFont)                                                                = 0;

protected:
    RenderTarget* m_pRenderTarget = nullptr;
    RenderTarget* m_blitRT = nullptr;
    DepthStencil* m_pDepthStencil = nullptr;

    Camera* m_pMainCamera = nullptr;
    Material* m_pCurrMaterial = nullptr;
    UiMaterial* m_pCurrUiMaterial = nullptr;
    
    Vect3f32 m_clearColor = Vect3f32(0.1f);
    
    virtual void SetViewport(int _width, int _height) = 0;

    friend class Window;
};

#endif
