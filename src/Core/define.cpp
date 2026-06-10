#include "define.h"

inline float Clamp(float v, float lo, float hi)   { return max(lo, min(v, hi)); }

const WString GetResPath()
{
    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    WString path(exePath);
    size_t lastSlash = path.find_last_of(L"\\/");
    path = path.substr(0, lastSlash + 1);
    path += L"../../../../res/";
    return path;
}

XMFLOAT3 ToColor(XMINT3 _color)
{
    XMFLOAT3 color;
    color.x = Clamp((float)_color.x, 0, 255)/255.0f;
    color.y = Clamp((float)_color.y, 0, 255)/255.0f;
    color.z = Clamp((float)_color.z, 0, 255)/255.0f;
    return color;
}

XMFLOAT3 ToColor(int _r, int _g, int _b)
{
    XMFLOAT3 color;
    color.x = Clamp((float)_r, 0, 255)/255.0f;
    color.y = Clamp((float)_g, 0, 255)/255.0f;
    color.z = Clamp((float)_b, 0, 255)/255.0f;
    return color;
}
