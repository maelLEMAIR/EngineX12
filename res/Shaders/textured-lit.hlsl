#include "common.hlsli"
#include "Lights.hlsli"

#define RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility=SHADER_VISIBILITY_VERTEX), " \
    "CBV(b1, visibility=SHADER_VISIBILITY_ALL), " \
    "CBV(b2, visibility=SHADER_VISIBILITY_ALL), " \
    "CBV(b3, visibility=SHADER_VISIBILITY_ALL), " \
    "DescriptorTable(SRV(t0)), " \
    "DescriptorTable(SRV(t1)), " \
    "DescriptorTable(SRV(t2)), " \
    "DescriptorTable(SRV(t3)), " \
    "StaticSampler(s0, " \
        "filter = FILTER_ANISOTROPIC, " \
        "maxAnisotropy = 8, " \
        "addressU = TEXTURE_ADDRESS_WRAP, " \
        "addressV = TEXTURE_ADDRESS_WRAP)"

Texture2D albedoMap     : register(t0);
Texture2D roughnessMap  : register(t1);
Texture2D normalMap     : register(t2);
Texture2D ambientMap    : register(t3);
SamplerState sampler0 : register(s0);

cbuffer MaterialCB : register(b1)
{
    int useAlbedoMap;
    int useRoughnessMap;
    int useNormalMap;
    int useAmbientMap;

    float3 gFresnelR0;
    float gRoughness;
}

[RootSignature(RS)]
VertexOut VSMain(VertexIn input)
{
    VertexOut output;
    
    float4 posW = mul(float4(input.PosL, 1.0f), gWorld);
    output.PosW = posW.xyz;
    float3 n = mul(input.NormalL, (float3x3) gWorld);
    output.NormalW = n;
    float3 t = mul(input.TangentL, (float3x3) gWorld);
    output.TangentW = t;
    output.PosH = mul(posW, gViewProj);

    //float3 B = cross(n, t);
    //float3x3 TBN = float3x3(normalize(t), normalize(B), normalize(n));
    
    //float3 scl;
    //DecomposeScale(gWorld, scl);
    //float3x3 sclMat = float3x3(scl.x, 0, 0, 0, scl.y, 0, 0, 0, scl.z);
        
    //TBN = mul(TBN, sclMat);
    
    float3 u = mul(normalize(input.TangentL), (float3x3) gWorld);
    
    float3 v = mul(cross(normalize(input.TangentL), normalize(input.NormalL)), (float3x3) gWorld);
    
    
    
    output.TexCoord = input.TexC * float2(abs(length(u)), abs(length(v)));
    output.Color = input.Color;
    
    
    
    return output;
}

float4 PSMain(VertexOut pinput) : SV_TARGET
{
    float4 albedo;
    if (useAlbedoMap)
        albedo = albedoMap.Sample(sampler0, pinput.TexCoord);
    else
        albedo = pinput.Color;
    
    if (useNormalMap)
    {
        float3 T = normalize(pinput.TangentW);
        float3 N = normalize(pinput.NormalW);
        T = normalize(T - dot(T, N) * N);
        float3 B = cross(N, T);
        float3x3 TBN = float3x3(T, B, N);

        float3 sampledNormal = normalMap.Sample(sampler0, pinput.TexCoord).rgb;
        sampledNormal = sampledNormal * 2.0f - 1.0f;
        pinput.NormalW = normalize(mul(sampledNormal, TBN));
    }
    else
        pinput.NormalW = normalize(pinput.NormalW);

    float4 ambient = gAmbientLight * albedo;
    if (useAmbientMap)
        ambient = ambient * ambientMap.Sample(sampler0, pinput.TexCoord);

    float roughness;
    if (useRoughnessMap)
        roughness = roughnessMap.Sample(sampler0, pinput.TexCoord).r;
    else 
        roughness = gRoughness;

    float3 toEye = normalize(gEyePosW - pinput.PosW);

    const float shininess = 1.0f - roughness;
    Material mat = { albedo, gFresnelR0, shininess };
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    float4 directLight = ComputeLighting(gLights, mat, pinput.PosW, pinput.NormalW, toEye, shadowFactor);
   
    float4 litColor = ambient + directLight;
    litColor.a = albedo.a;

    return litColor;
}