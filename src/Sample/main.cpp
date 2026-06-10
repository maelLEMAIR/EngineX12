#include <iostream>
#include <windows.h>
#include "main.h"

#include "Core/Console.h"
#include "Tests.h"

#ifdef _DEBUG
int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    /////////////////////////////////////////////////////////////////////////////
    /*Console::InitConsol();*/
    
    TestScript::Run();

    /*Console::DeleteConsol();*/
    return 0;
}
#else
int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    TestCollision::Run();
	
    return 0;
}

#endif // !_DEBUG