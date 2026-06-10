// ============================================================================
//  Luminance.hlsl
//  Converts an RGB render target to grayscale luminance.
//  Bindings:
//    t0 - input  color RT  (SRV)
//    u0 - output result RT (UAV / RTV binding)
// ============================================================================

#define RS \
    "RootFlags(0)," \
    "DescriptorTable(UAV(u0))"

RWTexture2D<float4> g_Output : register(u0);

[RootSignature(RS)]
[numthreads(8, 8, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    float4 color = g_Output[id.xy];
    float lum = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    g_Output[id.xy] = float4(lum, lum, lum, color.a);
    
    //g_Output[id.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}