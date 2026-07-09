#ifndef DESERIALIZATION_INL_INCLUDED
#define DESERIALIZATION_INL_INCLUDED

namespace Serialization 
{
    template<class CONTAINER>
    bool Deserializeration::readContainer(CONTAINER& container)
    {
        uint8 nbElements;
        if (!read(nbElements))
            return false;
        container.clear();
        container.reserve(nbElements);
        for (uint8 i = 0; i < nbElements; ++i)
        {
            typename CONTAINER::value_type element;
            if (!read(element))
                return false;
            container.push_back(element);
        }
        return true;
    }
}

#endif