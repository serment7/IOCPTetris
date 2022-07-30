#pragma once

#include <vector>
#include "../Game/Session.h"
#include "../Game/Player.h"
#include "ClientInfo.h"
#include "../Util/Logger.h"

class SessionManager
{
public:

	void AddSession(Player* playerInfo, ClientInfo* clientInfo);

	void RemoveSession(int sessionIndex);

	Session* FindSession(int sessionIndex);
	Session* FindSession(Player* player);

	template<typename Res>
	void SendNotify(int sessionIndex, Packet::PACKET_ID packetID, Res req)
	{
		auto target = FindSession(sessionIndex);
		if (target == nullptr)
		{
			return;
		}

		auto clientInfo = target->clientInfo;

		uint16_t dataSize = sizeof(Res);

		req.sessionIndex = sessionIndex;
		req.PacketId = static_cast<uint16_t>(packetID);
		req.sequenceIndex = 0;
		req.PacketLength = dataSize;

		logger.Debug("[Send] sessionIndex=%d, PacketID=%d, size=%d", sessionIndex, (uint16_t)packetID, dataSize);

		char* notifyData = new char[sizeof(Res)];

		std::copy(reinterpret_cast<char*>(&req), reinterpret_cast<char*>(&req) + sizeof(Res), notifyData);

		clientInfo->SendMsg(dataSize, notifyData, Packet::IOOperation::NOTIFY);
	}

	SessionManager()
		:logger("SessionManager")
	{
	};
	~SessionManager() = default;

private:
	std::vector<Session*> sessions;
	Logger logger;
};