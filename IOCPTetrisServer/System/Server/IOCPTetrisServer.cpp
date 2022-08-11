#include "IOCPTetrisServer.h"
#include "../../Game.h"
#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../../Game/Player.h"

using namespace Packet;

void IOCPTetris::IOCPTetrisServer::OnConnect(SBNet::ClientInfo* clientInfo)
{
	logger.Info("OnConnect");
	auto* newPlayer = Game::Get()->playerManager->GetEmptyPlayer();
	clientInfo->playerUID = newPlayer->playerUID;
	Game::Get()->sessionManager->AddSession(newPlayer, clientInfo);
}

void IOCPTetris::IOCPTetrisServer::OnClose(const UINT32 clientIndex)
{
	auto sessionIndex = FindClientInfo(clientIndex)->sessionIndex;
	auto playerUID = Game::Get()->sessionManager->FindSession(sessionIndex)->playerInfo->playerUID;

	Game::Get()->sessionManager->RemoveSession(sessionIndex);
	Game::Get()->playerManager->RemovePlayer(playerUID);
}

void IOCPTetris::IOCPTetrisServer::OnReceive(const UINT32 sessionIndex, const UINT32 recvSize, char* recvData)
{
	PACKET_HEADER* pHeader = reinterpret_cast<PACKET_HEADER*>(recvData);
	PacketInfo* packetInfo = new PacketInfo();
	packetInfo->DataSize = recvSize;
	packetInfo->pDataPtr = recvData;
	packetInfo->SessionIndex = sessionIndex;
	packetInfo->PacketId = pHeader->PacketId;
	packetManager->QueueRecvPacket(packetInfo);
}

IOCPTetris::IOCPTetrisServer::IOCPTetrisServer()
	: SBNet::IOCPServer("IOCPTetrisServer")
{
	packetManager = std::make_unique<PacketManager>();
	packetManager->Init();
	packetManager->SendPacketFunc = [&](INT32 sessionIndex, char* pData, INT16 size) {
		SBNet::ClientInfo* clientInfo = Game::Get()->sessionManager->FindSession(sessionIndex)->clientInfo;
		assert(clientInfo != nullptr);
		clientInfo->SendMsg(size, pData);
	};
}

void IOCPTetris::IOCPTetrisServer::ProcessThread()
{
	while (IsRun())
	{
		auto packetData = DequePacketData();
		if (packetData.DataSize != 0)
		{
			auto clientInfo = FindClientInfo(packetData.SessionIndex);
			if (clientInfo != nullptr)
			{
				clientInfo->SendMsg(packetData.DataSize, packetData.pPacketData);
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}
}

IOCPTetris::PacketData IOCPTetris::IOCPTetrisServer::DequePacketData()
{
	IOCPTetris::PacketData packetData;

	std::lock_guard<std::mutex> guard(lock);
	if (packetQueue.empty())
	{
		return PacketData();
	}

	packetData.Set(packetQueue.front());

	packetQueue.front().Release();
	packetQueue.pop_front();

	return packetData;
}
