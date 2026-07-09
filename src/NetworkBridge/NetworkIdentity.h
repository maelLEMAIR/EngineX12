#ifndef NETWORK_IDENTIFY_H_INCLUDED
#define NETWORK_IDENTIFY_H_INCLUDED

#include "define.h"

struct NetworkIdentity
{
    uint32 networkId  = 0;
    bool     isOwner    = false;
};

#endif