#pragma once

#include <unordered_map>
#include <WinSock2.h>
#include <memory>
#include <thread>
#include <deque>
#include <mutex>
#include <functional>
#include "../Handler/BaseHandler.h"
#include "../Util/Logger.h"

class Player;

namespace Packet
{
	class PacketInfo;
}

class PacketManager
{
public:
	PacketManager()
		:logger("PacketManager")
	{
	};

	~PacketManager();

	void Init();

	void PacketThread();

	void ProcessPacket(const UINT32 sessionIndex, const UINT32 packetID, const UINT16 packetSize, char* packet);

	void QueueRecvPacket(Packet::PacketInfo* packetData);
	void QueueSendPacket(Packet::PacketInfo* packetData);

	Packet::PacketInfo* DequeRecvPacket();
	Packet::PacketInfo* DequeSendPacket();

	std::function<void(INT32, char*, INT16)> SendPacketFunc;

	void Shutdown();

private:


	std::unordered_map<int, std::shared_ptr<IHandler>> mRecvFuntionDictionary;
	std::thread mProcessThread;
	std::deque<Packet::PacketInfo*> recvPacketQeueu;
	std::deque<Packet::PacketInfo*> sendPacketQeueu;
	bool isRun = false;
	Logger logger;
	
	std::mutex recvQueueGuard, sendQueueGuard;
};