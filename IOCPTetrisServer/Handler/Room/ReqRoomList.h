#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"

class Player;

namespace Handler
{
	class ReqRoomList : public BaseHandler<Packet::REQ_ROOM_LIST, Packet::RES_ROOM_LIST>
	{
	public:
		virtual void Logic(Player* player) override
		{
			const auto& roomInfos = Game::Get()->roomManager->GetRoomList(req.roomPage);

			for (int i = 0; i < roomInfos.size(); ++i)
			{
				res.roomInfo[i] = roomInfos[i];
			}

			res.result.resultID = ResultID::SUCCESS;
		}
		ReqRoomList() = default;
		virtual ~ReqRoomList() = default;
	};
}