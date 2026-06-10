uint Hash(uint s)
{
    s ^= 2747636419u;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    return s;
}

float Random(uint seed)
{
    return float(Hash(seed)) / 4294967295.0;
}

float Random1(uint seed, float min, float max)
{
    float r = Random(seed);
    
    float dist = max - min;
    r = r % dist + min;
    
    return r;
}

float2 Random2(uint seed, float2 min, float2 max)
{
    return float2(Random1(seed, min.x, max.x), Random1(seed, min.y, max.y));
}

float3 Random3(uint seed, float3 min, float3 max)
{
    return float3(Random2(seed, min.xy, max.xy), Random1(seed, min.z, max.z));
}

float4 Random4(uint seed, float4 min, float4 max)
{
    return float4(Random2(seed, min.xy, max.xy), Random2(seed, min.zw, max.zw));
}