#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"
#include "../../Game/Room.h"

class Player;

namespace Handler
{
	class ReqRoomCanEnter : public BaseHandler<Packet::REQ_ROOM_CAN_ENTER, Packet::RES_ROOM_CAN_ENTER>
	{
	public:
		virtual void Logic(Player* player) override
		{
			auto* roomInfos = Game::Get()->roomManager->FindRoom(req.roomIndex);

			if (roomInfos == nullptr)
			{
				res.result.resultID = ResultID::ROOM_NOT_FOUND;
			}
			else
			{
				res.result.resultID = roomInfos->CanEnterPlayer();
			}
			
		}
		ReqRoomCanEnter() = default;
		virtual ~ReqRoomCanEnter() = default;
	};
}