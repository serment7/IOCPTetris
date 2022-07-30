#pragma once
#include <string>
#include "IOCPTetrisNetDef.hh"

namespace SBNet
{
	class ServerConfig
	{
	public:
		std::string worldName;
		std::string serverName;
		unsigned short port = 0;
		ServerType serverType = ServerType::NONE;
		int maxSockBuf = 0;
		int workerQueueSize = 0;
		int listenQueueSize = 0;
		int clientQueueSize = 0;
	};
}

