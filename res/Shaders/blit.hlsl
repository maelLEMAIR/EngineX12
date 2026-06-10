#include "common.hlsli"

#define RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "DescriptorTable(SRV(t0)), " \
    "StaticSampler(s0, " \
        "filter = FILTER_ANISOTROPIC, " \
        "maxAnisotropy = 8, " \
        "addressU = TEXTURE_ADDRESS_WRAP, " \
        "addressV = TEXTURE_ADDRESS_WRAP)"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

[RootSignature(RS)]
VertexUiOut VSMain(uint vertexID : SV_VertexID)
{
    VertexUiOut output;
    
    output.TexCoord = float2((vertexID << 1) & 2, vertexID & 2);
    output.PosH = float4(output.TexCoord * 2.0f - 1.0f, 0.0f, 1.0f);
    output.PosH.y = -output.PosH.y; // flip Y for D3D
    return output;
}

float4 PSMain(VertexUiOut pinput) : SV_TARGET
{
    return gTexture.Sample(gSampler, pinput.TexCoord);
}