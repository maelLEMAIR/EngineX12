
struct VertexIn
{
    float3 PosL : POSITION;
    float2 TexC : TEXCOORD;
    float3 NormalL : NORMAL;
    float4 Color : COLOR;
    float3 TangentL : TANGENT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float3 TangentW : TANGENT;
};

struct VertexUiIn
{
    float2 PosL : POSITION;
    float2 TexC : TEXCOORD;
    float4 Color : COLOR;
};

struct VertexUiOut
{
    float4 PosH : SV_POSITION;
    //float2 PosW : POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
}

cbuffer cbPerPassData : register(b2)
{
    float4x4 gViewProj;
    float4x4 gView;
    float4x4 gProj;
    float4x4 gInvViewProj;

    float3 gEyePosW;
    float gNearZ;

    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float pad0;

    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
};

void DecomposeScale(in float4x4 m, out float3 scale)
{
    float sx = length(float3(m[0][0], m[0][1], m[0][2]));
    float sy = length(float3(m[1][0], m[1][1], m[1][2]));
    float sz = length(float3(m[2][0], m[2][1], m[2][2]));

    scale.x = sx;
    scale.y = sy;
    scale.z = sz;
}