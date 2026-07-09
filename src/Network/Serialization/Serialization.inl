#ifndef SERIALIZATION_INL_INCLUDED
#define SERIALIZATION_INL_INCLUDED

namespace Serialization 
{
    template<class CONTAINER>
    bool Serializer::writeContainer(const CONTAINER& container)
    {
        if (!write(static_cast<uint8>(container.size())))
            return false;
        for (const auto& element : container)
            if (!write(element))
                return false;
        return true;
    }
}

#endif