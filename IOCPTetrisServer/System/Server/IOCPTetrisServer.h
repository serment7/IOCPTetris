#pragma once
#include "IOCPServer.h"
#include "../Packet/PacketData.h"
#include "../PacketManager.h"

#include <mutex>
#include <deque>

namespace IOCPTetris
{
	class IOCPTetrisServer :
		public SBNet::IOCPServer
	{
	public:
		// IOCPServer을(를) 통해 상속됨
		virtual void OnConnect(SBNet::ClientInfo* clientInfo) override;
		virtual void OnClose(const UINT32 clientIndex) override;
		virtual void OnReceive(const UINT32 clientIndex, const UINT32 recvSize, char* recvData) override;

		IOCPTetrisServer();

	private:
		virtual void ProcessThread() override;

		PacketData DequePacketData();

		std::thread processThread;
		std::mutex lock;
		std::deque<PacketData> packetQueue;
		std::unique_ptr<PacketManager> packetManager;
	};
}
