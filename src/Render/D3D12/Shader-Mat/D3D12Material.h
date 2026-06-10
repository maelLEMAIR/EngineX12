#ifndef D3D12MATERIAL_H_DEFINED
#define D3D12MATERIAL_H_DEFINED

#include "../../Generic/Shader-Mat/Material.h"
#include "../../Generic/Shader-Mat/Shader.h"

class UploadBufferBase;
class D3D12Context;

class D3D12Material : public Material
{
public:
    ~D3D12Material() override;

    void Bind() override;
    
protected:
    UploadBufferBase* m_pUploadBuffer = nullptr;

    D3D12Context* m_pContext = nullptr;
    D3D12Material(D3D12Context* _pContext, Shader* _pShader);
    
    friend class D3D12Shader;
    friend class D3D12Device;
};

#endif
