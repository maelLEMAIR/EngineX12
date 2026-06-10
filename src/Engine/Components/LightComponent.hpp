#ifndef LIGHT_COMPONENT_HPP_DEFINED
#define LIGHT_COMPONENT_HPP_DEFINED

#include "../../Render/Generic/Render.h"

struct LightComponent
{
    XMFLOAT3 strength;
    float falloffStart;
    float falloffEnd;
    float spotPower;
    XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    LightType type;

    
    void SetPoint(  float _fallOfStart = 1.0f,
                    float _fallOfEnd = 10.0f,
                    float _strength = 1.0f)
    {
        falloffStart = _fallOfStart;
        falloffEnd = _fallOfEnd;
        strength = XMFLOAT3(_strength, _strength, _strength);
        type = LightType::Point;
        toUpdate = true;
    }
    
    void SetSpot(   float _fallOfStart = 1.0f,
                    float _fallOfEnd = 10.0f,
                    float _strength = 1.0f,
                    float _spotPower = 1.0f)
    {
        falloffStart = _fallOfStart;
        falloffEnd = _fallOfEnd;
        spotPower = _spotPower;
        strength = XMFLOAT3(_strength, _strength, _strength);
        type = LightType::Spot;
        toUpdate = true;
    }

    void SetColor(XMFLOAT4 _color)
    {
        color = _color;
        toUpdate = true;
    }
    void SetStrength(float _strength)
    {
        strength = XMFLOAT3(_strength, _strength, _strength);
        toUpdate = true;
    }
    void SetStrength(XMFLOAT3 _strength)
    {
        strength = _strength;
        toUpdate = true;
    }

private:
    int index = -1;
    bool toUpdate = false;

    friend struct LightSystem;
};

#endif
