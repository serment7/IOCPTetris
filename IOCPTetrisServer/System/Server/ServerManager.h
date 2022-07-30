#pragma once

#include "../../ServerConfig.hh"
#include "../../Util/Logger.h"
#include <cassert>
#include <string>
#include <vector>


namespace SBNet
{
	class IServer;
}

namespace SBNet
{
	class ServerManager
	{
	public:
		void Init();

		void StartServer(const ServerConfig& serverConfig);

		void Shutdown();

		void Command(std::string command);

		bool IsRun();

	public:
		ServerManager() = default;
	private:
		bool isRun = false;
		std::vector < SBNet::IServer* > servers;
		const Logger& logger = Logger("ServerFactory");
	};
}




