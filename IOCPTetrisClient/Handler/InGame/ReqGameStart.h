#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "Game.h"
#include "Scene/InGame.h"
#include "../IOCPTetrisCommon/TetrisBoard.h"

namespace Handler
{
	class ReqGameStart : public BaseHandler<Packet::REQ_GAME_START, Packet::RES_GAME_START>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_GAME_START&& res) override
		{
			auto curScene = Game::Get()->GetCurrentScene<IOCPTetris::InGameScene>();
			if (curScene != nullptr)
			{
				for (int i = 0; i < Common::PACKET_FALLING_BLOCK_SIZE; ++i)
				{
					curScene->AddFallingBlock(res.newBlocks[i]);
				}

				curScene->GameStart();
			}
		}

		ReqGameStart() = default;
		virtual ~ReqGameStart() = default;
	};
}