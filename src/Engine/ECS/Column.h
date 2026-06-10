#ifndef COLUMN_HPP_INCLUDED
#define COLUMN_HPP_INCLUDED

#include "define.h"

struct Column
{
    void InitializeColumn(size_t _size);
    
    void* GetElement(size_t _index);
    void PushBack(void* _element);
    void Grow();
    void SwapRemove(size_t _index);
    
    
    void* data; // vector of elements
    size_t stride; // size of a component
    size_t size; // number of element 
    size_t capacity; // memory capacity
};

#endif