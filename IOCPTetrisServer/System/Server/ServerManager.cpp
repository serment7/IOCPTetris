#include "ServerManager.h"
#include "IOCPTetrisServer.h"

void SBNet::ServerManager::Init()
{
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	assert(result == 0);

	servers.clear();

	isRun = true;

	logger.Info("Init");
}

void SBNet::ServerManager::StartServer(const ServerConfig& serverConfig)
{
	const auto& serverType = serverConfig.serverType;
	SBNet::IServer* newServer = nullptr;

	if (serverType == ServerType::MAINGAMESERVER)
	{
		newServer = new IOCPTetris::IOCPTetrisServer();
		newServer->Init(serverConfig);
		newServer->Start();
	}

	if (newServer != nullptr)
	{
		servers.push_back(newServer);
	}
}

void SBNet::ServerManager::Shutdown()
{
	isRun = false;

	try
	{
		for (const auto& server : servers)
		{
			server->Shutdown();
		}
		WSACleanup();
		servers.clear();

		logger.Info("Shutdown");
	}
	catch (std::exception exception)
	{
		logger.Error("Shutdown failed. %s", exception.what());
		throw exception;
	}
}

void SBNet::ServerManager::Command(std::string command)
{
	if (command == "quit")
	{
		Shutdown();
	}
}

bool SBNet::ServerManager::IsRun()
{
	return isRun;
}
