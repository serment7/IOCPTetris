#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"
#include "../../Game/Room.h"

#include<cassert>

namespace Handler
{
	class ReqGameInput : public BaseHandler<Packet::REQ_GAME_INPUT, Packet::RES_GAME_INPUT>
	{
	public:
		virtual void Logic(Player* player) override
		{
			auto roomInfo = Game::Get()->roomManager->FindRoom(player->roomIndex);

			if (roomInfo)
			{
				roomInfo->AddKeyInput(player->sessionIndex, req.inputs, req.inputCnt);
			}
		}
		ReqGameInput() = default;
		virtual ~ReqGameInput() = default;
	};
}