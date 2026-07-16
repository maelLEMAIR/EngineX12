#ifndef CONSOL_H_DEFINED
#define CONSOL_H_DEFINED

#include "Core/define.h"

class Console
{
public:
    static void InitConsol() {
        AllocConsole();

        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        freopen_s(&f, "CONIN$", "r", stdin);

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        DWORD mode = 0;
        GetConsoleMode(hConsole, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, mode);
    };

    static void DeleteConsol() {
        FreeConsole();
    };
};

#endif