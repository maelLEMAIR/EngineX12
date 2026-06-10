#ifndef D3D12UIMATERIAL_H_DEFINED
#define D3D12UIMATERIAL_H_DEFINED

#include "../../Generic/Shader-Mat/Material.h"
#include "D3D12UiShader.h"

class UploadBufferBase;

class D3D12UiMaterial : public UiMaterial
{
public:
    ~D3D12UiMaterial() override;

    void Bind() override;
    
private:
    UploadBufferBase* m_pUploadBuffer = nullptr;

    D3D12Context* m_pContext = nullptr;
    
    D3D12UiMaterial(D3D12Context* _pContext, D3D12UiShader* _pShader);

    friend class D3D12UiShader;
    friend class D3D12Device;
};

#endif
