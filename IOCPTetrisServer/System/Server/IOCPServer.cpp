#include "IOCPServer.h"
#include "../ClientInfo.h"

SBNet::IOCPServer::IOCPServer(const char* loggerName)
	: logger(loggerName)
{
}

SBNet::IOCPServer::~IOCPServer()
{

}

void SBNet::IOCPServer::Init(const ServerConfig& _config)
{
	config = _config;
	sockListener = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	assert(sockListener != INVALID_SOCKET);
}

void SBNet::IOCPServer::Start()
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(config.port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int result = bind(sockListener, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR));
	assert(result == 0);

	result = listen(sockListener, config.listenQueueSize);
	assert(result == 0);

	handleIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, config.workerQueueSize);
	assert(handleIOCP != NULL);

	auto listenBind = CreateIoCompletionPort((HANDLE)sockListener, handleIOCP, (UINT32)0, 0);
	assert(listenBind != NULL);

	isRun = true;

	SetupClientInfos(config.clientQueueSize);

	acceptThread = std::thread([this]() { AcceptThread(); });

	SetupWorkerThread(config.workerQueueSize);

	processThread = std::thread([this]() { ProcessThread(); });


}

void SBNet::IOCPServer::Shutdown()
{
	isRun = false;

	CloseHandle(handleIOCP);

	for (auto& worker : workerThreads)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}

	if (acceptThread.joinable())
	{
		acceptThread.join();
	}

	if (processThread.joinable())
	{
		processThread.join();
	}

	closesocket(sockListener);

	for (auto& clientInfo : clientInfos)
	{
		clientInfo->Disconnect(true);
		delete clientInfo;
		clientInfo = nullptr;
	}

	WSACleanup();
}

SBNet::ClientInfo* SBNet::IOCPServer::FindEmptyClientInfo()
{
	for (int i = 0; i < clientInfos.size(); ++i)
	{
		if (clientInfos[i]->IsConnect() == false)
		{
			return clientInfos[i];
		}
	}
	return nullptr;
}

SBNet::ClientInfo* SBNet::IOCPServer::FindClientInfo(int clientIndex)
{
	for (int i = 0; i < clientInfos.size(); ++i)
	{
		if (clientInfos[i]->clientIndex == clientIndex)
		{
			return clientInfos[i];
		}
	}
	return nullptr;
}

void SBNet::IOCPServer::AcceptThread()
{
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);

	while (isRun)
	{
		auto curTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

		for (auto client : clientInfos)
		{
			if (client->IsConnect())
			{
				continue;
			}

			if ((UINT64)curTimeSec < client->GetLatestClosedTimeSec())
			{
				continue;
			}

			auto diff = curTimeSec - client->GetLatestClosedTimeSec();
			if (diff <= RE_USE_SESSION_WAIT_TIMESEC)
			{
				continue;
			}

			client->PostAccept(sockListener, curTimeSec);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(32));
	}
}

void SBNet::IOCPServer::WorkerThread()
{
	ClientInfo* clientInfo = NULL;
	BOOL bSuccess = TRUE;
	DWORD dwIoSize = 0;
	LPOVERLAPPED lpOverlapped = NULL;

	while (isRun)
	{
		bSuccess = GetQueuedCompletionStatus(handleIOCP,
			&dwIoSize,
			(PULONG_PTR)&clientInfo,
			&lpOverlapped,
			INFINITE);


		if (FALSE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			isRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		auto pOverlappedEx = (Packet::OverlappedEx*)lpOverlapped;
		
		if (!bSuccess || (0 == dwIoSize && Packet::IOOperation::ACCEPT != pOverlappedEx->operation))
		{
			if (clientInfo == nullptr)
			{
				logger.Error("GetQueuedCompletionStatus Failed. WSAGetLastError=%d", WSAGetLastError());
			}
			else
			{
				clientInfo->Disconnect();
			}

			continue;
		}

		if (Packet::IOOperation::ACCEPT == pOverlappedEx->operation)
		{
			logger.Info("Accepted");
			auto clientInfo = FindClientInfo(pOverlappedEx->clientIndex);
			if (clientInfo->AcceptCompletion())
			{
				//클라이언트 갯수 증가
				++clientNumber;
			}
			else
			{
				CloseSocket(clientInfo, true);
			}
		}
		else if (Packet::IOOperation::RECV == pOverlappedEx->operation)
		{
			clientInfo->OnRecv(pOverlappedEx->wsaBuf.buf, dwIoSize);

			if (clientInfo->sessionIndex == 0)
			{
				OnConnect(clientInfo);
			}

			while (true)
			{
				auto readBuf = clientInfo->ReadPacket();
				if (readBuf == nullptr)
				{
					break;
				}

				Packet::PACKET_HEADER* packetHeader = reinterpret_cast<Packet::PACKET_HEADER*>(readBuf);
				OnReceive(clientInfo->sessionIndex, packetHeader->PacketLength, readBuf);
			}

			clientInfo->BindRecv();
		}
		else if (Packet::IOOperation::SEND == pOverlappedEx->operation)
		{
			logger.Info("Send to session : %d", clientInfo->GetSessionIndex());
			clientInfo->SendCompleted(dwIoSize);
		}
		else if (Packet::IOOperation::NOTIFY == pOverlappedEx->operation)
		{
			logger.Info("Notified to session : %d", clientInfo->GetSessionIndex());
			clientInfo->SendCompleted(dwIoSize);
		}
		//예외 상황
		else
		{
			logger.Error("Unknown Operaion. sessionNumber=%d", clientInfo->clientIndex);
		}
	}
}

void SBNet::IOCPServer::Send(INT32 clientIndex, char* pData, INT16 size)
{
	auto* clientInfo = FindClientInfo(clientIndex);
	clientInfo->SendMsg(size, pData);
}

void SBNet::IOCPServer::CloseSocket(ClientInfo* clientInfo_, bool isForce_)
{
	if (clientInfo_->IsConnect() == false)
	{
		return;
	}

	auto clientIndex = clientInfo_->GetClientIndex();

	clientInfo_->Disconnect(isForce_);

	OnClose(clientIndex);
}

void SBNet::IOCPServer::Disconnect(int clientIndex)
{
	auto clientInfo = FindClientInfo(clientIndex);
	clientInfo->Disconnect();
}

bool SBNet::IOCPServer::IsRun()
{
	return isRun;
}

void SBNet::IOCPServer::SetupWorkerThread(const int workerQueueSize)
{
	workerThreads.clear();
	for (int i = 0; i < workerQueueSize; ++i)
	{
		workerThreads.emplace_back([this]() { WorkerThread();  });
	}
}

void SBNet::IOCPServer::SetupClientInfos(int clientQueueSize)
{
	clientInfos.clear();
	for (int i = 1; i <= clientQueueSize; ++i)
	{
		auto* newClientInfo = new ClientInfo();
		newClientInfo->Init(i, handleIOCP);
		clientInfos.push_back(newClientInfo);
	}
}
