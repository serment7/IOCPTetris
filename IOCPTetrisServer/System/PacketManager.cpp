#include "PacketManager.h"

#include "../../IOCPTetrisCommon/Packet.hh"
#include "../Handler/Handler.h"
#include "../Game.h"

PacketManager::~PacketManager()
{
	Shutdown();
}

void PacketManager::Init()
{
	//Common
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_LOGIN] = std::make_shared<Handler::ReqLogin>();

	// InGame
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_FALLING_BLOCK] = std::make_shared<Handler::ReqFallingBlock>();
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_GAME_INPUT] = std::make_shared<Handler::ReqGameInput>();
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_GAME_START] = std::make_shared<Handler::ReqGameStart>();
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_GAME_STOP] = std::make_shared<Handler::ReqGameStop>();

	// Room
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_ROOM_CAN_ENTER] = std::make_shared<Handler::ReqRoomCanEnter>();
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_ROOM_CREATE] = std::make_shared<Handler::ReqRoomCreate>();
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_ROOM_ENTER] = std::make_shared<Handler::ReqRoomEnter>();
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_ROOM_LEAVE] = std::make_shared<Handler::ReqRoomLeave>();
	mRecvFuntionDictionary[(int)Packet::PACKET_ID::REQ_ROOM_LIST] = std::make_shared<Handler::ReqRoomList>();

	isRun = true;

	mProcessThread = std::thread([this]() { PacketThread(); });
}

void PacketManager::PacketThread()
{
	while (isRun)
	{
		auto packetData = DequeRecvPacket();
		if (packetData != nullptr)
		{
			ProcessPacket(packetData->SessionIndex, packetData->PacketId, packetData->DataSize, packetData->pDataPtr);
			/*delete packetData;*/
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

void PacketManager::ProcessPacket(const UINT32 sessionIndex, const UINT32 packetID, const UINT16 packetSize, char* packet)
{
	auto handler = mRecvFuntionDictionary.at(packetID);
	handler->ReadReq(packet);

	Player* player = Game::Get()->playerManager->FindPlayerBySessionIndex(sessionIndex);

	handler->Logic(player);

	char* res = nullptr;
	INT16 size = 0;
	handler->WriteResponse(res, size);

	logger.Debug("[Send] sessionIndex=%d, PacketID=%d, size=%d", sessionIndex, packetID, size);

	SendPacketFunc(sessionIndex, res, size);
}

void PacketManager::QueueRecvPacket(Packet::PacketInfo* packetData)
{
	logger.Debug("[Recv] sessionIndex=%d, PacketID=%d, size=%d", packetData->SessionIndex, packetData->PacketId, packetData->DataSize);

	std::lock_guard<std::mutex> guard(recvQueueGuard);
	recvPacketQeueu.push_back(packetData);
}

Packet::PacketInfo* PacketManager::DequeRecvPacket()
{
	Packet::PacketInfo* result = nullptr;
	std::lock_guard<std::mutex> guard(recvQueueGuard);

	if (recvPacketQeueu.empty())
	{
		return result;
	}

	result = recvPacketQeueu.front();
	recvPacketQeueu.pop_front();
	return result;
}

void PacketManager::QueueSendPacket(Packet::PacketInfo* packetData)
{
	std::lock_guard<std::mutex> guard(sendQueueGuard);
	sendPacketQeueu.push_back(packetData);
}

Packet::PacketInfo* PacketManager::DequeSendPacket()
{
	Packet::PacketInfo* result = nullptr;
	std::lock_guard<std::mutex> guard(sendQueueGuard);

	if (sendPacketQeueu.empty())
	{
		return result;
	}

	result = sendPacketQeueu.front();
	sendPacketQeueu.pop_front();
	return result;
}

void PacketManager::Shutdown()
{
	isRun = false;

	if (mProcessThread.joinable())
	{
		mProcessThread.join();
	}
}
