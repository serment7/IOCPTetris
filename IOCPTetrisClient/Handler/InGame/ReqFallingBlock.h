#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "Game.h"
#include "Scene/InGame.h"

namespace Handler
{
	class ReqFallingBlock : public BaseHandler<Packet::REQ_FALLING_BLOCK, Packet::RES_FALLING_BLOCK>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_FALLING_BLOCK&& res) override
		{
			auto curScene = Game::Get()->GetCurrentScene<IOCPTetris::InGameScene>();

			if (curScene != nullptr)
			{
				for (int i = 0; i < Common::PACKET_FALLING_BLOCK_SIZE; ++i)
				{
					curScene->AddFallingBlock(res.newBlocks[i]);
				}
			}
		}

		ReqFallingBlock() = default;
		virtual ~ReqFallingBlock() = default;
	};
}