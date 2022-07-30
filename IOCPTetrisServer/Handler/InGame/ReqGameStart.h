#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"

namespace Handler
{
	class ReqGameStart : public BaseHandler<Packet::REQ_GAME_START, Packet::RES_GAME_START>
	{
	public:
		virtual void Logic(Player* player) override
		{
			auto* room = Game::Get()->roomManager->FindRoom(player->roomIndex);
			room->GameStart(player->sessionIndex);

			player->fallingBlockIndex = 0;

			auto result = room->GetFallingBlock(player, Common::PACKET_FALLING_BLOCK_SIZE);
			for (size_t i = 0; i < Common::PACKET_FALLING_BLOCK_SIZE; i++)
			{
				res.newBlocks[i] = result[i];
			}
		}
		ReqGameStart() = default;
		virtual ~ReqGameStart() = default;
	};
}