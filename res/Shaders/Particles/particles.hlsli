#include "random.hlsli"

struct Particle
{
    float3 position;
    float life;
    
    float4 color;
    
    float3 velocity;
    float accel;
    
    float2 scale;
    float2 _pad01;
};

struct EmitterSettings
{
    float4 minColor, maxColor;
    
    float3 minOffset;
    float minLife;
    
    float3 maxOffset;
    float maxLife;
    
    float3 minVelocity;
    float minAccel;
    
    float3 maxVelocity;
    float maxAccel;
    
    float2 minScale, maxScale;
    
    float3 position;
    float _pad0;
};

void MakeParticle(out Particle p, uint seed, EmitterSettings em)
{
    p.life      = Random1(seed, em.minLife, em.maxLife);
    p.velocity  = Random3(seed, em.minVelocity, em.maxVelocity);
    p.accel     = Random1(seed, em.minAccel, em.maxAccel);
    p.scale     = Random2(seed, em.minScale, em.maxScale);
    p.color = Random4(seed, em.minColor, em.maxColor);
    
    float3 offset = Random3(seed, em.minOffset, em.maxOffset);
    p.position = em.position + offset;
}