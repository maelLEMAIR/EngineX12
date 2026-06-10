#ifndef DEFAULTBUFFERBASE_H_DEFINED
#define DEFAULTBUFFERBASE_H_DEFINED

#include "../../Common/Common.h"
#include "../define.h"
#include "../Utils/d3dUtil.h"

#include "../Base/D3D12Context.h"

class DefaultBufferBase
{
public:
    DefaultBufferBase(D3D12Context* _pContext, uint64 _elementSize, uint64 _elementCount, const void* _data) :
        m_pContext(_pContext)
    {
        m_pContext->ResetCmdList();
        m_pBuffer = d3dUtil::CreateDefaultBuffer(m_pContext->GetDevice(),
         m_pContext->GetCommandList(), _data, _elementSize *  _elementCount, m_pUploader);
        m_pContext->CloseCmdList();
        m_pContext->FlushCommandQueue();

        if (m_pUploader != nullptr)
        {
            m_pUploader->Release();
            m_pUploader = nullptr;
        }
    }

    ID3D12Resource* Resource()const
    {
        return m_pBuffer;
    }

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const
    {
        return m_pBuffer->GetGPUVirtualAddress();
    }

    ~DefaultBufferBase()
    {
        if (m_pBuffer != nullptr)
        {
            m_pBuffer->Release();
            m_pBuffer = nullptr;
        }
    }

    
protected:
    ID3D12Resource* m_pBuffer = nullptr;
    ID3D12Resource* m_pUploader = nullptr;

    D3D12Context* m_pContext;
};

#endif