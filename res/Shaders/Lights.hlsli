#define MAX_LIGHTS 16

struct Light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction; // directional/spot light only
    float FalloffEnd; // point/spot light only
    float3 Position; // point light only
    float SpotPower; // spot light only
    
    float4 Color;
};

cbuffer cbLights : register(b3)
{
    Light gLights[MAX_LIGHTS];
    
    float gAmbientLight;
    int gActiveDirLights;
    int gActivePointLights;
    int gActiveSpotLights;
};

struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    
    float Shininess;
};

float CalcAttenuation(float _d, float _fallOfStart, float _fallOfEnd)
{
    return saturate((_fallOfEnd - _d) / (_fallOfEnd - _fallOfStart));
} 

float3 SchlickFresnel(float3 _R0, float3 _normal, float3 _lightVec)
{
    float cosIncidentAngle = saturate(dot(_normal, _lightVec));
    
    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = _R0 + (1.0f - _R0) * (f0 * f0 * f0 * f0);
    
    return reflectPercent;
}

float3 BlinnPhong(  float3 _lightStrength, float3 _lightVec, 
                    float3 _normal, float3 _toEye, Material _mat)
{
    const float m = _mat.Shininess * 256.0f;
    float3 halfVec = normalize(_toEye + _lightVec);
    
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, _normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(_mat.FresnelR0, halfVec, _lightVec);
    
    float3 specAlbedo = fresnelFactor*roughnessFactor;
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);
    
    return (_mat.DiffuseAlbedo.rgb + specAlbedo) * _lightStrength;
}

float3 ComputeDirectionalLight(Light _L, Material _mat, float3 _normal, float3 _toEye)
{
    float3 lightVec = -_L.Direction;
    
    float ndot1 = max(dot(lightVec, _normal), 0.0f);
    float3 lightStrength = _L.Strength * ndot1;
    
    return BlinnPhong(lightStrength, lightVec, _normal, _toEye, _mat) * _L.Color.rgb;
}

float3 ComputePointLight(Light _L, Material _mat, float3 _pos, float3 _normal, float3 _toEye)
{
    float3 lightVec = _L.Position - _pos;
    
    float d = length(lightVec);
    if (d > _L.FalloffEnd)
        return 0.0f;
    
    lightVec /= d;
    
    float ndotl = max(dot(lightVec, _normal), 0.0f);
    float3 lightStrength = _L.Strength * ndotl;
    
    float att = CalcAttenuation(d, _L.FalloffStart, _L.FalloffEnd);
    lightStrength *= att;
    
    return BlinnPhong(lightStrength, lightVec, _normal, _toEye, _mat) * _L.Color.rgb;
}

float3 ComputeSpotLight(Light _L, Material _mat, float3 _pos, float3 _normal, float3 _toEye)
{
    float3 lightVec = _L.Position - _pos;
    
    float d = length(lightVec);
    if (d > _L.FalloffEnd)
        return 0.0f;
    
    lightVec /= d;
    
    float ndot1 = max(dot(lightVec, _normal), 0.0f);
    float3 lightStrength = _L.Strength * ndot1;
    
    float att = CalcAttenuation(d, _L.FalloffStart, _L.FalloffEnd);
    lightStrength *= att;
    
    float spotFactor = pow(max(dot(-lightVec, _L.Direction), 0.0f), _L.SpotPower);
    lightStrength *= spotFactor;
    
    return BlinnPhong(lightStrength, lightVec, _normal, _toEye, _mat) * _L.Color.rgb;
}

float4 ComputeLighting(Light _gLights[MAX_LIGHTS], Material _mat, float3 _pos, float3 _normal, float3 _toEye, float3 _shadowFactor)
{
    float3 result = 0.0f;
    
    int i = 0;
    
    int limit = gActiveDirLights;
    
    for (i = 0; i < limit; i++)
    {
        result += ComputeDirectionalLight(_gLights[i], _mat, _normal, _toEye);
    }
    
    limit += gActivePointLights;
    
    for (i = gActiveDirLights; i < limit; i++)
    {
        result += ComputePointLight(_gLights[i], _mat, _pos, _normal, _toEye);
    }
    
    limit += gActiveSpotLights;
    
    for (i = gActiveDirLights + gActivePointLights; i < limit; i++)
    {
        result += ComputeSpotLight(_gLights[i], _mat, _pos, _normal, _toEye);
    }
    
    return float4(result, 0.0f);
}