#ifndef CAMERA_COMPONENT_HPP_DEFINED
#define CAMERA_COMPONENT_HPP_DEFINED

#include "../../Render/Generic/Render.h"

struct CameraComponent
{
    uint32 camId = 0;
    bool isMainCamera = false;
};

#endif
