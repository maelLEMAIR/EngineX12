// ============================================================================
//  Luminance.hlsl
//  Converts an RGB render target to grayscale luminance.
//  Bindings:
//    t0 - input  color RT  (SRV)
//    u0 - output result RT (UAV / RTV binding)
// ============================================================================

#define RS \
    "RootFlags(0)," \
    "CBV(b0)," \
    "UAV(u0)," \
    "DescriptorTable(UAV(u1))"

cbuffer CharCB : register(b0)
{
    float minX, minY, maxX, maxY;
    
    float penX, penY;
    float spread;
    float smoothing;
    
    uint lineCount;
    uint canvasWidth;
    uint canvasHeight;
    float _pad;
};

struct Line
{
    float x1, y1, x2, y2;
};

RWStructuredBuffer<Line> g_Lines : register(u0);
RWTexture2D<unorm float4> g_Output : register(u1);

[RootSignature(RS)]
[numthreads(8, 8, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint pixelW = (uint) (maxX - minX + 1);
    uint pixelH = (uint) (maxY - minY + 1);
    if (id.x >= pixelW || id.y >= pixelH)
        return;

    float x = minX + (float) id.x;
    float y = minY + (float) id.y;

    uint interCount = 0;
    float minDist = 1e30;

    for (uint i = 0; i < lineCount; i++)
    {
        float x1 = g_Lines[i].x1, y1 = g_Lines[i].y1;
        float x2 = g_Lines[i].x2, y2 = g_Lines[i].y2;

        float dx = x2 - x1, dy = y2 - y1;
        float len2 = dx * dx + dy * dy;
        if (len2 > 0)
        {
            float t = clamp(((x - x1) * dx + (y - y1) * dy) / len2, 0.0, 1.0);
            float nx = x1 + t * dx - x;
            float ny = y1 + t * dy - y;
            minDist = min(minDist, sqrt(nx * nx + ny * ny));
        }

        if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y))
        {
            float t = (y - y1) / (y2 - y1);
            if (x < x1 + t * (x2 - x1))
                interCount++;
        }
    }

    bool inside = (interCount % 2) == 1;
    if (!inside)
        return;

    int px = (int) (penX + x);
    int py = (int) (penY - y);

    if (px < 0 || py < 0 || px >= (int) canvasWidth || py >= (int) canvasHeight)
        return;

    g_Output[uint2(px, py)] = float4(1, 1, 1, 1);
}