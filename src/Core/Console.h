#ifndef CONSOL_H_DEFINED
#define CONSOL_H_DEFINED

#include "define.h"

class Console
{
public:
    static void InitConsol() {
        AllocConsole();

        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        freopen_s(&f, "CONIN$", "r", stdin);

        // Open a dedicated handle with full access — bypasses the freopen access restriction
        HANDLE hOut = CreateFileA(
            "CONOUT$",
            GENERIC_READ | GENERIC_WRITE,  // needs both for GetConsoleMode
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        DWORD dwMode = 0;
        
        if (!GetConsoleMode(hOut, &dwMode)) {
            // hOut is probably INVALID_HANDLE_VALUE — freopen broke the link
            printf("GetConsoleMode failed: %lu\n", GetLastError());
        }
        if (!SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            printf("SetConsoleMode failed: %lu\n", GetLastError());
        }
        
        CloseHandle(hOut);
        
    };

    static void DeleteConsol() {
        FreeConsole();
    };
};

#endif