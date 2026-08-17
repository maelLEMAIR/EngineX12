#ifndef LIGHT_COMPONENT_HPP_DEFINED
#define LIGHT_COMPONENT_HPP_DEFINED

#include "../../Render/Generic/Render.h"

struct LightComponent
{
    Vect3f32 strength;
    float falloffStart;
    float falloffEnd;
    float spotPower;
    Vect4f32 color = {1.0f, 1.0f, 1.0f, 1.0f};
    LightType type;


    void SetPoint(  float _fallOfStart = 1.0f,
                    float _fallOfEnd = 10.0f,
                    float _strength = 1.0f)
    {
        falloffStart = _fallOfStart;
        falloffEnd = _fallOfEnd;
        strength = Vect3f32(_strength, _strength, _strength);
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
        strength = Vect3f32(_strength, _strength, _strength);
        type = LightType::Spot;
        toUpdate = true;
    }

    void SetColor(Vect4f32 const& _color)
    {
        color = _color;
        toUpdate = true;
    }
    void SetStrength(float _strength)
    {
        strength = Vect3f32(_strength, _strength, _strength);
        toUpdate = true;
    }
    void SetStrength(Vect3f32 const& _strength)
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
