#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"

namespace Handler
{
	class ReqGameStop : public BaseHandler<Packet::REQ_GAME_STOP, Packet::RES_GAME_STOP>
	{
	public:
		virtual void Logic(Player* player) override
		{
			Game::Get()->roomManager->FindRoom(player->roomIndex)->GameStop(player->sessionIndex);
		}
		ReqGameStop() = default;
		virtual ~ReqGameStop() = default;
	};
}