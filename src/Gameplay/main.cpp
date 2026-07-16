#include "Core/define.h"
#include "main.h"

#include "Engine.h"
#include <shellapi.h>

#include "Core/Console.h"
#include "Gameplay/MainScene.h"
#include "Gameplay/Scenes/MenuScene.h"


#pragma comment(lib, "shell32.lib")

static std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result.data(), size, nullptr, nullptr);
    return result;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
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

    EngineManager engineManager;
    engineManager.Initialize(1920, 1080, L"TestEngineManager", true);

    SceneManager::CreateSceneType<MainScene>("MainScene");
	SceneManager::CreateSceneType<MenuScene>("MenuScene");
    SceneManager::SetCurrentScene("MenuScene");

    engineManager.Run();

    Console::DeleteConsol();
    return 0;
}