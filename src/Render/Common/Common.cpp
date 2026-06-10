#include "Common.h"

// const WString GetResPath()
// {
//     WCHAR exePath[MAX_PATH];
//     GetModuleFileNameW(nullptr, exePath, MAX_PATH);
//     WString path(exePath);
//     size_t lastSlash = path.find_last_of(L"\\/");
//     path = path.substr(0, lastSlash + 1);
//     path += L"../../../../res/";
//     return path;
// }