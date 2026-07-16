#ifndef ENGINE_MANAGER_H_DEFINED
#define ENGINE_MANAGER_H_DEFINED

// Engine
#include "define.h"

// Core
#include "Core/Chrono.h"

// Network
#include "Network/NetworkManager.h"
#include "NetworkBridge/Packet/PacketHandler.h"

class Device;
class Window;
class Scene;
class Camera;
class RessourceManager;
class SceneManager;
class Client;
class Server;

class EngineManager
{
public:
    EngineManager();
    ~EngineManager();

    static EngineManager& GetInstance();
    
    void Initialize(UINT _width, UINT _height, WString _title, bool _fullscreen, int argc = 0, char* argv[] = nullptr);
    void Run();
    void TryRegisterNewClients(const NetworkPacket& packet, NetworkManager& net);
    void Exit();
	
    static float GetDeltaTime() { return GetInstance().m_deltaTime; }
    float GetTotalTime() const { return m_totalTime; }
    static Window* GetWindow() { return GetInstance().m_pWindow; }
    static Device* GetDevice() { return GetInstance().m_pDevice; }

    static bool		IsServer()	{ return GetInstance().m_pServer != nullptr; }
    static Client*	GetClient() { return GetInstance().m_pClient; }
    static Server*	GetServer() { return GetInstance().m_pServer; }

private:
    static EngineManager* s_pInstance;

    Chrono m_chrono;
    Chrono m_networkChrono;
    
    float m_deltaTime = 0.0f;
    float m_totalTime = 0.0f;
    
    Camera* m_camera = nullptr;
    
    Window* m_pWindow = nullptr;
    Device* m_pDevice = nullptr;
    
    SceneManager*       m_pSceneManager;
    RessourceManager*   m_pRessourceManager;
    NetworkManager*     m_pNetworkManager;
    PacketHandler*      m_pPacketHandler;

    Client* m_pClient = nullptr;
    Server* m_pServer = nullptr;
    
    float m_DeltaTime = 0.0f;
};

#endif

