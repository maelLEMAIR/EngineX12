#ifndef TEXTURE_H_DEFINED
#define TEXTURE_H_DEFINED

#include "../../Common/Common.h"

class Texture
{
public:
    virtual ~Texture() = default;
    
    virtual void Load(const WString& _path) = 0;
    virtual void Bind(uint32 _index) = 0;

protected:
    uint8 width = 0, height = 0;
};

#endif
