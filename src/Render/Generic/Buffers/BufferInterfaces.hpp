#ifndef BUFFER_INTERFACES_HPP_DEFINED
#define BUFFER_INTERFACES_HPP_DEFINED

#include "../../Common/Common.h"

class IDefaultBuffer
{
    virtual ~IDefaultBuffer() = default;
};

class IUploadBuffer
{
    virtual ~IUploadBuffer() = default;

    virtual void Resize() = 0;
    virtual void CopyRawData(uint64 elementIndex, const void* data) = 0;
    virtual uint64 AppendRaw(const void* _data) = 0;
    virtual uint64 AppendRangeRaw(const void* _data, const uint64 _count) = 0;
    
    virtual void Reset() { m_firstAvailableIndex = 0; }
    
    uint64 GetElementByteSize() const { return m_elementByteSize; }
    uint64 GetCapacity() const { return m_elementCount; }
    uint64 GetFirstAvailable() const { return m_firstAvailableIndex; }
    
protected:
    uint64 m_elementByteSize = 0;
    uint64 m_elementCount = 0;
    uint64 m_firstAvailableIndex = 0;
    bool m_isConstantBuffer = false;
};

class IUAVBuffer 
{
    virtual ~IUAVBuffer() = default;

    virtual void SetOutput() = 0;
    virtual void SetInput() = 0;
    virtual void SetCopy() = 0;
    virtual void SetFree() = 0;

protected:
    uint64                m_elementByteSize = 0;
    uint64                m_elementCount    = 0;
};

#endif
