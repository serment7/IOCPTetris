#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "../../Game/Player.h"
#include "../../Game.h"
#include <cassert>

namespace Handler
{
	class ReqFallingBlock : public BaseHandler<Packet::REQ_FALLING_BLOCK, Packet::RES_FALLING_BLOCK>
	{
	public:
		virtual void Logic(Player* player) override
		{
			auto* room = Game::Get()->roomManager->FindRoom(player->roomIndex);

			if (room == nullptr)
			{
				return;
			}

			const auto& fallingBlocks = room->GetFallingBlock(player, Common::PACKET_FALLING_BLOCK_SIZE);
			for (int i = 0; i < Common::PACKET_FALLING_BLOCK_SIZE; ++i)
			{
				res.newBlocks[i] = fallingBlocks[i];
			}
		}
		ReqFallingBlock() = default;
		virtual ~ReqFallingBlock() = default;
	};
}