#ifndef COMPUTE_SHADER_FACTORY_HPP_DEFINED
#define COMPUTE_SHADER_FACTORY_HPP_DEFINED

#include "../Base/Device.h"
#include "../Shader-Mat/ComputeShader.h"

class ComputeShaderFactory
{
public:
    static ComputeShader* CreateLuminance(Device* _pDevice)
    {
        ComputeShader* cs = _pDevice->CreateComputeShader(RES("/Shaders/ComputeShaders/Luminance.hlsl"));
        // cs->AddBinding("Input", ResBindingType::SRV, 0);
        cs->AddBinding("Output", ResBindingType::RTV, 0, 0);

        return cs;
    }
};

#endif