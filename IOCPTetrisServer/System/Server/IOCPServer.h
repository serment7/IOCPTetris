#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "IServer.hh"
#include "../../Util/Logger.h"

#include <vector>
#include <thread>
#include <cassert>

class ClientInfo;

namespace SBNet
{
	class IOCPServer : public IServer
	{
	public:
		IOCPServer(const char* loggerName = "IOCPServer");
		virtual ~IOCPServer();

		virtual void Init(const ServerConfig& _config);

		virtual void Start();

		virtual void Shutdown();

		ClientInfo* FindEmptyClientInfo();

		ClientInfo* FindClientInfo(int clientIndex);

		void AcceptThread();

		void WorkerThread();

		void Send(INT32 clientIndex, char* pData, INT16 size);

		void CloseSocket(ClientInfo* clientInfo_, bool isForce_ = false);

		void Disconnect(int clientIndex);

		bool IsRun();

		virtual void OnConnect(ClientInfo* clientInfo) = 0;
		virtual void OnClose(const UINT32 clientIndex) = 0;
		virtual void OnReceive(const UINT32 clientIndex, const UINT32 recvSize, char* recvData) = 0;

	protected:
		Logger logger;

	private:
		virtual void ProcessThread() = 0;
		
		void SetupWorkerThread(const int workerQueueSize);

		void SetupClientInfos(int clientQueueSize);

		SOCKET sockListener = INVALID_SOCKET;
		SBNet::ServerConfig config;
		std::vector<ClientInfo*> clientInfos;
		std::thread acceptThread;
		std::thread processThread;
		int clientNumber = 0;
		std::vector<std::thread> workerThreads;
		bool isRun = false;
		HANDLE		handleIOCP = NULL;
	};

}