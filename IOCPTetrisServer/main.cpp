#include "Util/Logger.h"
#include "System/Server/ServerManager.h"
#include "ServerConfig.hh"

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	SBNet::Logger::LoadConfig();

	SBNet::Logger main("main");
	main.Info("server start");

	SBNet::ServerConfig config;
	config.worldName = "IOCPTetris";
	config.serverName = "MAINGAMESERVER";
	config.port = 15001;
	config.serverType = SBNet::ServerType::MAINGAMESERVER;
	config.maxSockBuf = 1024;
	config.workerQueueSize = 16;
	config.listenQueueSize = 5;
	config.clientQueueSize = 100;

	// start server
	SBNet::ServerManager serverManager;

	serverManager.Init();
	serverManager.StartServer(config);

	while (serverManager.IsRun())
	{
		std::string command;
		std::getline(std::cin, command);

		serverManager.Command(command);
	}

	// server end
	serverManager.Shutdown();
	SBNet::Logger::Shutdown();
	main.Info("server end");
	return 0;
}