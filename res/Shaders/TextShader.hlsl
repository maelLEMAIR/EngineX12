#include "common.hlsli"

#define RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0)," \
    "CBV(b1)," \
    "CBV(b2), " \
    "DescriptorTable(SRV(t0))," \
    "StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR, addressU=TEXTURE_ADDRESS_CLAMP, addressV=TEXTURE_ADDRESS_CLAMP)"

cbuffer ColorCB : register(b1)
{
    float4 gColor;
};

Texture2D g_Atlas : register(t0);
SamplerState g_Sampler : register(s0);

[RootSignature(RS)]
VertexUiOut VSMain(VertexUiIn i)
{
    VertexUiOut o;
    
    float4 worldPos = mul(float4(i.PosL, 0, 1), gWorld);
    
    o.PosH = float4((worldPos.x / (gRenderTargetSize.x * 0.5f)),
                         -(worldPos.y / (gRenderTargetSize.y * 0.5f)),
                          0.0f, 1.0f);
    
    o.TexCoord = i.TexC;
    o.Color = i.Color;
    return o;
}

float4 PSMain(VertexUiOut i) : SV_Target
{
    float4 sample = g_Atlas.Sample(g_Sampler, i.TexCoord);
    return float4(i.Color.rgb, i.Color.a * sample.a);
}