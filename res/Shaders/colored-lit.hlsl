#include "common.hlsli"
#include "Lights.hlsli"

#define RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility=SHADER_VISIBILITY_VERTEX), " \
    "CBV(b1, visibility=SHADER_VISIBILITY_ALL), " \
    "CBV(b2, visibility=SHADER_VISIBILITY_ALL)," \
    "CBV(b3, visibility=SHADER_VISIBILITY_ALL)"


cbuffer cbMaterial : register(b1)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
}

[RootSignature(RS)]
VertexOut VSMain(VertexIn input)
{
    VertexOut output;
    
    float4 posW = mul(float4(input.PosL, 1.0f), gWorld);
    output.PosW = posW.xyz;
    output.NormalW = mul(input.NormalL, (float3x3) gWorld);
    output.PosH = mul(posW, gViewProj);
    output.TexCoord = input.TexC;
    output.Color = input.Color;
    
    return output;
}

float4 PSMain(VertexOut pinput) : SV_TARGET
{
    float3 normalW = normalize(pinput.NormalW);
    
    float3 toEye = normalize(gEyePosW - pinput.PosW);
    float4 ambient = gAmbientLight * gDiffuseAlbedo;
    
    const float shininess = 1.0f - gRoughness;
    Material mat = { gDiffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    float4 directLight = ComputeLighting(gLights, mat, pinput.PosW, normalW, toEye, shadowFactor);
   
    float4 litColor = ambient + directLight;
    litColor.a = gDiffuseAlbedo.a;
    
    return litColor;

}
