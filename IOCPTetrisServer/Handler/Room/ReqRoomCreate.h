#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"

namespace Handler
{
	class ReqRoomCreate : public BaseHandler<Packet::REQ_ROOM_CREATE, Packet::RES_ROOM_CREATE>
	{
	public:
		virtual void Logic(Player* player) override
		{
			res.newRoomIndex = Game::Get()->roomManager->CreateRoom();
		}
		ReqRoomCreate() = default;
		virtual ~ReqRoomCreate() = default;
	};
}