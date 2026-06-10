#ifndef CONSOL_H_DEFINED
#define CONSOL_H_DEFINED

#include <Windows.h>
#include <iostream>

class Console
{
public:
    static void InitConsol() {
        AllocConsole();

        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        freopen_s(&f, "CONIN$", "r", stdin);
    };

    static void DeleteConsol() {
        FreeConsole();
    };
};

#endif