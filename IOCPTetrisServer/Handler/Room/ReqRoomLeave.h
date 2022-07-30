#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"
#include "../../Game/Room.h"

namespace Handler
{
	class ReqRoomLeave : public BaseHandler<Packet::REQ_ROOM_LEAVE, Packet::RES_ROOM_LEAVE>
	{
	public:
		virtual void Logic(Player* player) override
		{
			auto* roomInfo = Game::Get()->roomManager->FindRoom(player->roomIndex);
			
			if (roomInfo == nullptr)
			{
				return;
			}

			roomInfo->RemovePlayer(player->sessionIndex);

			if (roomInfo->GetCurrentPlayerCnt() == 0)
			{
				auto targetRoomIndex = roomInfo->GetRoomIndex();
				Game::Get()->roomManager->RemoveRoom(targetRoomIndex);
			}
		}
		ReqRoomLeave() = default;
		virtual ~ReqRoomLeave() = default;
	};
}