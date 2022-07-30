#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"

namespace Handler
{

	class ReqRoomEnter : public BaseHandler<Packet::REQ_ROOM_ENTER, Packet::RES_ROOM_ENTER>
	{
	public:
		virtual void Logic(Player* player) override
		{
			auto* roomInfo = Game::Get()->roomManager->FindRoom(req.roomIndex);
			if (roomInfo == nullptr)
			{
				res.result.resultID = ResultID::ROOM_NOT_FOUND;
				return;
			}

			roomInfo->AddPlayer(player);
			res.enterSessionIndex = player->sessionIndex;
		}
		ReqRoomEnter() = default;
		virtual ~ReqRoomEnter() = default;
	};
}