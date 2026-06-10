#ifndef D3D12TEXTURE_H_DEFINED
#define D3D12TEXTURE_H_DEFINED

#include "../../Common/Common.h"
#include "../../Generic/Shader-Mat/Texture.h"
#include "../define.h"

class D3D12Context;

class D3D12Texture : public Texture
{
public:
    void Load(const WString& _path) override;
    void Bind(uint32 _index) override;

    int GetHeapIndex() { return m_heapIndex; }
    
    ~D3D12Texture() override;
private:
    D3D12Texture(D3D12Context* _pContext);

    INT m_heapIndex = -1;
    
    ID3D12Resource* m_pResource = nullptr;
    ID3D12Resource* m_pUploader = nullptr;

    D3D12Context* m_pContext = nullptr;

    CD3DX12_GPU_DESCRIPTOR_HANDLE GetHandle();

    friend class D3D12Device;
};

#endif
