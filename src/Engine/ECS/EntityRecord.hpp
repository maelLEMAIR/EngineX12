#ifndef ENTITY_RECORD_HPP_INCLUDED
#define ENTITY_RECORD_HPP_INCLUDED

#include "../define.h"

#include "Archetype.hpp"

struct EntityRecord
{
    Archetype* archetype = nullptr;
    size_t     rowId     = 0;
};

#endif
