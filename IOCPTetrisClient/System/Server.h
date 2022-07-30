#pragma once

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include "../../IOCPTetrisCommon/PacketBuffer.h"
#include "../Handler/Handler.h"
#include "../Game.h"

#include <memory>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <atomic>

class Server
{
public:
	~Server();

	void Shutdown();

	static Server* Get();

	void Init()
	{
	}
	void ConnectServer(const char* ip, short port);

	template<typename Req>
	void Send(Packet::PACKET_ID packetID, Req req)
	{
		if (IsConnect() == false)
		{
			return;
		}

		int sessionIndex = Game::Get()->GetPlayerSessionIndex();

		req.sessionIndex = sessionIndex;
		req.PacketId = static_cast<uint16_t>(packetID);
		req.sequenceIndex = ++sequenceIndex;
		req.PacketLength = sizeof(Req);

		Packet::PacketInfo packetInfo;
		packetInfo.DataSize = sizeof(Req);
		packetInfo.PacketId = (uint16_t)packetID;
		packetInfo.pDataPtr = new char[sizeof(req)];
		packetInfo.SessionIndex = sessionIndex;
		packetInfo.SequenceIndex = req.sequenceIndex;
		
		std::copy(reinterpret_cast<char*>(&req), reinterpret_cast<char*>(&req) + sizeof(Req), packetInfo.pDataPtr);

		std::lock_guard<std::mutex> sendGuard(sendLock);
		sendPacketQueue.push(packetInfo);
	}

	template<typename Req, typename Res>
	void SendSync(Packet::PACKET_ID packetID, Req req, Res& res)
	{
		if (IsConnect() == false)
		{
			return;
		}

		int sessionIndex = Game::Get()->GetPlayerSessionIndex();

		req.sessionIndex = sessionIndex;
		req.PacketId = static_cast<uint16_t>(packetID);
		req.sequenceIndex = ++sequenceIndex;
		req.PacketLength = sizeof(Req);

		Packet::PacketInfo packetInfo;
		packetInfo.DataSize = sizeof(Req);
		packetInfo.PacketId = (uint16_t)packetID;
		packetInfo.pDataPtr = new char[sizeof(req)];
		packetInfo.SessionIndex = sessionIndex;
		packetInfo.SequenceIndex = req.sequenceIndex;

		std::copy(reinterpret_cast<char*>(&req), reinterpret_cast<char*>(&req) + sizeof(Req), packetInfo.pDataPtr);

		needSyncSequnce.store(req.sequenceIndex);
		needSyncPacket.store(true);

		{
			std::lock_guard<std::mutex> sendGuard(sendLock);
			sendPacketQueue.push(packetInfo);
		}

		while (needSyncPacket.load() == true)
		{
			HandlerThread();
			std::this_thread::sleep_for(std::chrono::milliseconds(32));
		}

		std::copy(syncPacket.pDataPtr, syncPacket.pDataPtr + syncPacket.DataSize, reinterpret_cast<char*>(&res));
	}

	void Close();

	bool IsConnect();

	void RecvThread();

	void HandlerThread();

	void SendThread();

private:

	Server();

	void RegistRes();

	struct PacketInfoLess
	{
		bool operator()(const Packet::PacketInfo& left, const Packet::PacketInfo& right)
		{
			return left.SequenceIndex < right.SequenceIndex;
		}
	};

	SOCKET sockServer = INVALID_SOCKET;
	std::unique_ptr<PacketBuffer> packetBuffer;
	std::thread recvThread, sendThread, handlerThread;
	std::queue<Packet::PacketInfo> sendPacketQueue;
	std::mutex sendLock, sockLock, packetBufferLock, handlerLock, syncLock;
	HANDLE handleIocp = INVALID_HANDLE_VALUE;
	// sequence, req
	std::priority_queue<Packet::PacketInfo, std::vector<Packet::PacketInfo>, PacketInfoLess> sendPackets;
	std::priority_queue<Packet::PacketInfo, std::vector<Packet::PacketInfo>, PacketInfoLess> recvPackets;
	std::queue<Packet::PacketInfo> notifyPackets;
	Packet::PacketInfo syncPacket;
	std::unordered_map <uint16_t, std::shared_ptr<IHandler>> handlers;
	
	std::atomic<bool> needSyncPacket;
	std::atomic<int> needSyncSequnce;
	bool isRun = false;
	bool isConnect = false;
	int sequenceIndex = 0;
};

