#include "common.hlsli"

#define RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility=SHADER_VISIBILITY_VERTEX), " \
    "CBV(b1, visibility=SHADER_VISIBILITY_ALL), " \
    "CBV(b2, visibility=SHADER_VISIBILITY_VERTEX)"

cbuffer cbMaterial : register(b1)
{
    float4 gColor;
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
    return pinput.Color;
}