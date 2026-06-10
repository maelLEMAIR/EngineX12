#include "common.hlsli"

#define RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility=SHADER_VISIBILITY_VERTEX), " \
    "CBV(b1, visibility=SHADER_VISIBILITY_ALL), " \
    "CBV(b2, visibility=SHADER_VISIBILITY_VERTEX), " \
    "DescriptorTable(SRV(t0)), " \
    "StaticSampler(s0, " \
        "filter = FILTER_ANISOTROPIC, " \
        "maxAnisotropy = 8, " \
        "addressU = TEXTURE_ADDRESS_WRAP, " \
        "addressV = TEXTURE_ADDRESS_WRAP)"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer cbMaterial : register(b1)
{
    float4 gColor;
    float4 gTextureRect;
    int useTexture;
}

[RootSignature(RS)]
VertexUiOut VSMain(VertexUiIn input)
{
    VertexUiOut output;
    
    float4 worldPos = mul(float4(input.PosL, 0.0f, 1.0f), gWorld);

    output.PosH = float4((worldPos.x / (gRenderTargetSize.x * 0.5f)),
                         (worldPos.y / (gRenderTargetSize.y * 0.5f)),
                          0.0f, 1.0f);
    
    output.TexCoord = input.TexC;
    output.Color = input.Color * gColor;
    
    return output;
}

float4 PSMain(VertexUiOut pinput) : SV_TARGET
{
    float2 uvCoord = float2(gTextureRect.x + pinput.TexCoord.x * gTextureRect.z,
                         gTextureRect.y + pinput.TexCoord.y * gTextureRect.w);
    
    float4 color = pinput.Color;
    float4 texSample = gTexture.Sample(gSampler, uvCoord);
    color *= lerp(float4(1, 1, 1, 1), texSample, float(useTexture));
    
    return color;
}