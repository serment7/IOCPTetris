#include "pch.h"
#include "Server.h"

Server::~Server()
{
	Shutdown();

	isRun = false;

	if (recvThread.joinable())
	{
		recvThread.join();
	}

	if (sendThread.joinable())
	{
		sendThread.join();
	}

	if (handlerThread.joinable())
	{
		handlerThread.join();
	}
}

void Server::Shutdown()
{
	if (isConnect == true)
	{
		// 소켓 동기함수에서 블락되고 있을 경우 소켓을 먼저 해제해주어야 블락이 풀릴 것이다.
		Close();
	}
}

Server* Server::Get()
{
	static Server instance;
	return &instance;
}

void Server::ConnectServer(const char* ip, short port)
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip);

	int nRet = 0;

	{
		std::lock_guard<std::mutex> guard(sockLock);
		sockServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
		assert(sockServer != INVALID_SOCKET);

		CreateIoCompletionPort((HANDLE)sockServer, handleIocp, (ULONG_PTR)0, 0);

		nRet = WSAConnect(sockServer, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR), NULL, NULL, NULL, NULL);

		assert(nRet != SOCKET_ERROR);
	}

	assert(nRet == 0);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	isConnect = true;
}

void Server::Close()
{
	std::lock_guard<std::mutex> guard(sockLock);
	if (sockServer != INVALID_SOCKET)
	{
		shutdown(sockServer, SD_BOTH);
		closesocket(sockServer);
		sockServer = INVALID_SOCKET;
	}
	isConnect = false;
}

bool Server::IsConnect()
{
	return isConnect;
}

void Server::RecvThread()
{
	char buf[1024] = { 0, };
	WSABUF wsaBuf;
	wsaBuf.buf = buf;
	wsaBuf.len = 1024;
	Packet::OverlappedEx overlapped;
	ZeroMemory(&overlapped, sizeof(Packet::OverlappedEx));
	overlapped.wsaBuf.buf = buf;
	overlapped.wsaBuf.len = 1024;

	DWORD dwSendBytes = 0;
	DWORD dwReceivedBytes = 0;
	DWORD dwFlag = 0;

	while (isRun)
	{
		if (IsConnect() == false)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(32));
			continue;
		}

		int nRet = WSARecv(sockServer, &wsaBuf, 1, &dwReceivedBytes, &dwFlag, (LPWSAOVERLAPPED)&overlapped, NULL);
		Packet::OverlappedEx* pOverlappedEx = nullptr;

		if (nRet == SOCKET_ERROR)
		{
			int lastError = WSAGetLastError();

			// 연결이 해제되었다.
			if (lastError == WSA_IO_PENDING)
			{
				LPOVERLAPPED lpOverlapped = nullptr;
				void* completionkey = nullptr;
				auto bSuccess = GetQueuedCompletionStatus(handleIocp, &dwReceivedBytes, (PULONG_PTR)&completionkey, &lpOverlapped, INFINITE);

				assert(lpOverlapped != nullptr);

				if (FALSE == bSuccess && 0 == dwReceivedBytes)
				{
					isRun = false;
					continue;
				}

				pOverlappedEx = (Packet::OverlappedEx*)lpOverlapped;
			}
			else
			{
				assert(WSAGetLastError() != 0 && "recv error");
				Close();
				continue;
			}
		}
		else
		{
			pOverlappedEx = &overlapped;
		}

		char* readBuf = nullptr;

		{
			std::lock_guard<std::mutex> packetBufferGuard(packetBufferLock);
			packetBuffer->Write(pOverlappedEx->wsaBuf.buf, dwReceivedBytes);
		}

		while (true)
		{
			{
				std::lock_guard<std::mutex> packetBufferGuard(packetBufferLock);
				readBuf = packetBuffer->Read();
			}

			if (readBuf == nullptr)
			{
				break;
			}
			
			Packet::PACKET_HEADER* packetHeader = reinterpret_cast<Packet::PACKET_HEADER*>(readBuf);

			Packet::PacketInfo packetInfo;
			packetInfo.DataSize = packetHeader->PacketLength;
			packetInfo.PacketId = packetHeader->PacketId;
			packetInfo.pDataPtr = readBuf;
			packetInfo.SequenceIndex = packetHeader->sequenceIndex;

			{
				std::lock_guard<std::mutex> handlerGuard(handlerLock);

				// 서버가 먼저 요청한 건이다.
				if (packetInfo.SequenceIndex == 0)
				{
					notifyPackets.push(packetInfo);
				}
				else
				{
					recvPackets.push(packetInfo);
					if (recvPackets.size() > sendPackets.size())
					{
						int i = 0;
					}
				}
			}
		}
	}
}

void Server::HandlerThread()
{
	if (recvPackets.empty() != true)
	{
		// 요청한 시퀀스 넘버 응답이 오면 핸들러를 불러서 처리한다.
		auto req = sendPackets.top();
		auto res = recvPackets.top();

		if (req.SequenceIndex == res.SequenceIndex)
		{
			{
				std::lock_guard<std::mutex> guard(handlerLock);

				sendPackets.pop();
				recvPackets.pop();
			}

			assert(handlers.end() != handlers.find(res.PacketId));

			auto handler = handlers.at(res.PacketId);

			handler->SetRequest(req.pDataPtr, req.DataSize);
			handler->Response(res.pDataPtr, res.DataSize);

			if (needSyncPacket.load() && needSyncSequnce.load() == res.SequenceIndex)
			{
				syncPacket = res;
				needSyncPacket.store(false);
			}
		}
	}

	if (notifyPackets.empty() != true)
	{
		auto notifyRes = notifyPackets.front();

		{
			std::lock_guard<std::mutex> guard(handlerLock);
			notifyPackets.pop();
		}

		assert(handlers.end() != handlers.find(notifyRes.PacketId));

		auto handler = handlers.at(notifyRes.PacketId);

		//handler->SetRequest(notifyRes.pDataPtr, notifyRes.DataSize);
		handler->Response(notifyRes.pDataPtr, notifyRes.DataSize);
	}
}

//void Server::HandlerThread()
//{
//	while (isRun)
//	{
//		if (recvPackets.empty() != true)
//		{
//			// 요청한 시퀀스 넘버 응답이 오면 핸들러를 불러서 처리한다.
//			auto req = sendPackets.top();
//			auto res = recvPackets.top();
//
//			if (req.SequenceIndex != res.SequenceIndex)
//			{
//				std::this_thread::sleep_for(std::chrono::milliseconds(32));
//				continue;
//			}
//
//			{
//				std::lock_guard<std::mutex> guard(handlerLock);
//
//				sendPackets.pop();
//				recvPackets.pop();
//			}
//
//			assert(handlers.end() != handlers.find(res.PacketId));
//
//			auto handler = handlers.at(res.PacketId);
//
//			handler->SetRequest(req.pDataPtr, req.DataSize);
//			handler->Response(res.pDataPtr, res.DataSize);
//
//			if (needSyncPacket.load() && needSyncSequnce.load() == res.SequenceIndex)
//			{
//				syncPacket = res;
//				needSyncPacket.store(false);
//			}
//			continue;
//		}
//
//		if (notifyPackets.empty() != true)
//		{
//			auto notifyRes = notifyPackets.front();
//
//			{
//				std::lock_guard<std::mutex> guard(handlerLock);
//				notifyPackets.pop();
//			}
//
//			assert(handlers.end() != handlers.find(notifyRes.PacketId));
//
//			auto handler = handlers.at(notifyRes.PacketId);
//
//			handler->SetRequest(notifyRes.pDataPtr, notifyRes.DataSize);
//			handler->Response(notifyRes.pDataPtr, notifyRes.DataSize);
//			continue;
//		}
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(32));
//	}
//}

void Server::SendThread()
{
	while (isRun)
	{
		if (IsConnect() == false || sendPacketQueue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(32));
			continue;
		}

		auto sendPacket = sendPacketQueue.front();

		{
			std::lock_guard<std::mutex> sendGuard(sendLock);
			sendPacketQueue.pop();
		}

		{
			std::lock_guard<std::mutex> guard(handlerLock);
			sendPackets.push(sendPacket);
		}

		int nRet = send(sockServer, sendPacket.pDataPtr, sendPacket.DataSize, 0);

		// 연결이 해제되었다.
		if (nRet == SOCKET_ERROR)
		{
			Close();
			continue;
		}
	}
}

Server::Server()
{
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	assert(result == 0);

	packetBuffer = std::make_unique<PacketBuffer>();
	packetBuffer->Init(20000, sizeof(Packet::PACKET_HEADER));

	handleIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);

	RegistRes();

	isRun = true;

	recvThread = std::thread([&]() { RecvThread(); });
	sendThread = std::thread([&]() { SendThread(); });
	//handlerThread = std::thread([&]() { HandlerThread(); });
}

void Server::RegistRes()
{
	// Common
	handlers[(uint16_t)Packet::PACKET_ID::RES_LOGIN] = std::make_shared<Handler::ReqLogin>();

	// InGame
	handlers[(uint16_t)Packet::PACKET_ID::RES_FALLING_BLOCK] = std::make_shared<Handler::ReqFallingBlock>();
	handlers[(uint16_t)Packet::PACKET_ID::RES_GAME_INPUT] = std::make_shared<Handler::ReqGameInput>();
	handlers[(uint16_t)Packet::PACKET_ID::RES_GAME_START] = std::make_shared<Handler::ReqGameStart>();
	handlers[(uint16_t)Packet::PACKET_ID::RES_GAME_STOP] = std::make_shared<Handler::ReqGameStop>();

	// Room
	handlers[(uint16_t)Packet::PACKET_ID::RES_ROOM_CAN_ENTER] = std::make_shared<Handler::ReqRoomCanEnter>();
	handlers[(uint16_t)Packet::PACKET_ID::RES_ROOM_CREATE] = std::make_shared<Handler::ReqRoomCreate>();
	handlers[(uint16_t)Packet::PACKET_ID::RES_ROOM_ENTER] = std::make_shared<Handler::ReqRoomEnter>();
	handlers[(uint16_t)Packet::PACKET_ID::RES_ROOM_LEAVE] = std::make_shared<Handler::ReqRoomLeave>();
	handlers[(uint16_t)Packet::PACKET_ID::RES_ROOM_LIST] = std::make_shared<Handler::ReqRoomList>();
}
