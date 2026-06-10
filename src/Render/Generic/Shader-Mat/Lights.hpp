#ifndef LIGHTS_HPP_DEFINED
#define LIGHTS_HPP_DEFINED

#include "../../Common/Common.h"

#define MAX_LIGHTS 16

enum class LightType
{
    Directional, Point, Spot
};

struct Light
{
    XMFLOAT3 Strength;
    float FalloffStart;
    XMFLOAT3 Direction;
    float FalloffEnd;
    XMFLOAT3 Position;
    float SpotPower;

    XMFLOAT4 Color;
};

struct LightDescriptor
{
    Light light;
    LightType type;
};

struct LightData
{
    Light lights[MAX_LIGHTS];

    float ambient = 0.2f;
    int dirLightCount = 0;
    int pointLightCount = 0;
    int spotLightCount = 0;
};

class LightHelper
{
public:
    static LightData GetLightData(Vector<LightDescriptor>& _lights)
    {
        LightData lightData;

        Vector<Light> lights;

        int dirStart = 0;
        int pointStart = 1;
        int spotStart = 2;

        std::vector<Light> dirLights, pointLights, spotLights;

        for (int i = 0; i < _lights.size() && i < 16; i++)
        {
            if (_lights[i].type == LightType::Directional)
                dirLights.push_back(_lights[i].light);
            else if (_lights[i].type == LightType::Point)
                pointLights.push_back(_lights[i].light);
            else if (_lights[i].type == LightType::Spot)
                spotLights.push_back(_lights[i].light);
        }

        lightData.dirLightCount   = (int)dirLights.size();
        lightData.pointLightCount = (int)pointLights.size();
        lightData.spotLightCount  = (int)spotLights.size();

        lights.clear();
        lights.insert(lights.end(), dirLights.begin(),   dirLights.end());
        lights.insert(lights.end(), pointLights.begin(), pointLights.end());
        lights.insert(lights.end(), spotLights.begin(),  spotLights.end());

        memset(lightData.lights, 0, sizeof(Light) * 16);
        memcpy(lightData.lights, lights.data(), sizeof(Light) * lights.size());

        return lightData;
    }
    
    static LightDescriptor CreateLight(LightType _type)
    {
        LightDescriptor result;
        result.type = _type;
        result.light = {
            XMFLOAT3{ 1.0f, 1.0f, 1.0f },
            5.0f,
            XMFLOAT3{ 1.0f, 0.0f, 0.0f },
            10.0f,
            XMFLOAT3{ 0.0f, 0.0f, 0.0f },
            2.0f,
            XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
        };
        return result;
    }
};

#endif