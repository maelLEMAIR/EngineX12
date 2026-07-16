#ifndef NETWORK_LAUNCH_ARGS_H_INCLUDED
#define NETWORK_LAUNCH_ARGS_H_INCLUDED

#include "NetworkRole.h"
#include <string>
#include <cstdint>

struct NetworkLaunchArgs
{
    NetworkRole role       = NetworkRole::None;
    std::string serverIp   = "127.0.0.1";  // utilisé côté client
    uint16_t    serverPort = 7777;
    uint16_t    localPort  = 7777;          // utilisé côté serveur
    uint8       player     = 0;

    static NetworkLaunchArgs Parse(int argc, char* argv[])
    {
        NetworkLaunchArgs args;

        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];

            if (arg == "--server")
            {
                args.role      = NetworkRole::Server;
                args.localPort = 7777;
            }
            else if (arg == "--client")
            {
                args.role      = NetworkRole::Client;
                args.localPort = 0;
            }
            else if (arg == "--ip" && i + 1 < argc)
            {
                args.serverIp = argv[++i];
            }
            else if (arg == "--port" && i + 1 < argc)
            {
                args.serverPort = (uint16_t)std::stoi(argv[++i]);
            }
            else if (arg == "--player" && i + 1 < argc)
            {
                args.player = (uint8)std::stoi(argv[++i]);
            }
        }

        return args;
    }
};

#endif