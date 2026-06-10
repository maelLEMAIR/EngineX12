#ifndef COMPUTE_SHADER_H_DEFINED
#define COMPUTE_SHADER_H_DEFINED

#include "../../Common/Common.h"

enum ResBindingType
{
    CBV,
    SRV,
    UAV,
    RTV,
};

struct ResourceBinding
{
    ResBindingType type = UAV;
    uint32 shaderSlot = 0;
    uint32 rootIndex = 0;
    uint32 heapIndex = 0;
};

class RenderTarget;

class ComputeShader
{
public:
    virtual ~ComputeShader() = default;

    virtual void Bind() = 0;

    template <typename T>
    void SetConstant(String const& _bindingName, T const& _data);
    template <typename T>
    void SetUnorderedAccess(String const& _bindingName, Vector<T> const& _data);
    template <typename T>
    void SetUnorderedAccess(String const& _bindingName, uint32 _count, T const& _defaultValue);

    virtual void SetRTV(String const& _bindingName, RenderTarget* _pRenderTarget) = 0;
    virtual void SetSRV(String const& _bindingName, RenderTarget* _pTexture) = 0;
    
    template <typename T>
    Vector<T> GetUnorderedAccessData(String const& _bindingName, uint32 _elementCount);

    virtual void Dispatch() = 0;
    virtual void FlushUAVWrites() = 0;
    virtual void CalculateGroupSizes() = 0;
    
    virtual void AddBinding(String const& _bindingName, ResBindingType _type, uint32 _rootIndex, uint32 _shaderSlot)
        { m_resourceBindings[_bindingName] = { _type, _shaderSlot, _rootIndex, 0}; }

    void SetThreadGroupSize(uint32 _tGSizeX, uint32 _tGSizeY, uint32 _tGSizeZ)
        { m_threadGroupSizeX = _tGSizeX; m_threadGroupSizeY = _tGSizeY; m_threadGroupSizeZ = _tGSizeZ; }
    
protected:

    uint32 m_threadGroupSizeX = 1;
    uint32 m_threadGroupSizeY = 1;
    uint32 m_threadGroupSizeZ = 1;
    
    UnorderedMap<String, ResourceBinding> m_resourceBindings;
    UnorderedMap<String, RenderTarget*> m_inputRtvs;
    UnorderedMap<String, RenderTarget*> m_outputRtvs;

    virtual void CopyIntoCB(String const& _bindingName, const void* _data, uint32 _elementSize) = 0;
    virtual void CopyIntoUA(String const& _bindingName, const void* _data, uint32 _elementSize, uint32 _elementCount = 1) = 0;
    virtual void CopyFromUA(String const& _bindingName, void* _data, uint32 _elementSize) = 0;
};

#include "ComputeShader.inl"

#endif