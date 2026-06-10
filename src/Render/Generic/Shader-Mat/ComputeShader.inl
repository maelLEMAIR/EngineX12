#ifndef COMPUTE_SHADER_INL_DEFINED
#define COMPUTE_SHADER_INL_DEFINED

#include "ComputeShader.h"

template <typename T>
void ComputeShader::SetConstant(String const& _bindingName, T const& _data)
{
    CopyIntoCB(_bindingName, &_data, sizeof(T));
}

template <typename T>
void ComputeShader::SetUnorderedAccess(String const& _bindingName, Vector<T> const& _data)
{
    CopyIntoUA(_bindingName, _data.data(), sizeof(T), _data.size());
}

template <typename T>
void ComputeShader::SetUnorderedAccess(String const& _bindingName, uint32 _count, T const& _defaultValue)
{
    Vector<T> data;
    data.resize(_count, _defaultValue);
    CopyIntoUA(_bindingName, data.data(), sizeof(T), _count);
}

template <typename T>
Vector<T> ComputeShader::GetUnorderedAccessData(String const& _bindingName, uint32 _elementCount)
{
    Vector<T> data;
    data.resize(_elementCount);
    CopyFromUA(_bindingName, data.data(), sizeof(T));
    return data;
}

#endif
