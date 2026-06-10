#ifndef COLUMN_CPP_INCLUDED
#define COLUMN_CPP_INCLUDED

#include "Column.h"

void Column::InitializeColumn(size_t _size)
{
    stride = _size;
    size = 0;
    capacity = 8;
    data = malloc(capacity * stride);
    
}

void* Column::GetElement(size_t _index)
{
    char* base = (char*)data;
    return base + _index * stride;
}

void Column::PushBack(void* _element)
{
    if ( size >= capacity )
        Grow();
    memcpy(GetElement(size), _element, stride);
    size++;
}

void Column::Grow()
{
    capacity *= 2;
    data = (char*)realloc(data, capacity * stride);
}

void Column::SwapRemove(size_t _index)
{
    if ( size == 0 || _index >= size )
        return;

    if ( _index != size - 1)
    {
        void* elem = GetElement(_index);
        void* back = GetElement(size - 1);
        memcpy(elem, back, stride);
    }
    size--;
}

#endif
