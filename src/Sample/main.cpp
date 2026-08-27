#include "Core/define.h"
#include "main.h"

#include "Engine.h"
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#include "Core/Console.h"
#include "Tests.h"

static std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result.data(), size, nullptr, nullptr);
    return result;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    int argc;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

    Console::InitConsol();
    std::vector<std::string> argStrings(argc);
    std::vector<char*>       argv(argc);
    for (int i = 0; i < argc; i++)
    {
        argStrings[i] = WStringToString(std::wstring(argvW[i]));
        argv[i]       = argStrings[i].data();
    }
    LocalFree(argvW);

    //TestNetwork::Run(argc, argv.data());
    //TestEngineManager::Run();
    //TestPhysicWorld::Run();
	DemoGeos::Run();

    EngineManager::GetInstance().Run();
    Console::DeleteConsol();
    return 0;
}