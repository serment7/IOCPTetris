#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "Game.h"
#include "Scene/InGame.h"
#include "../../../IOCPTetrisCommon/TetrisBoard.h"

namespace Handler
{
	class ReqGameInput : public BaseHandler<Packet::REQ_GAME_INPUT, Packet::RES_GAME_INPUT>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_GAME_INPUT&& res) override
		{
			auto* curScene = Game::Get()->GetCurrentScene<IOCPTetris::InGameScene>();

			if (curScene == nullptr)
			{
				return;
			}

			auto enemyBoard = curScene->GetEnemyBoard();

			for (int i = 0; i < Common::PACKET_KEY_INPUT_SIZE; ++i)
			{
				if (res.inputs[i] == IOCPTetris::InputType::NONE)
				{
					break;
				}

				enemyBoard->QueueInput(res.inputs[i]);
			}
			
			enemyBoard->Update(0.f);
		}

		ReqGameInput() = default;
		virtual ~ReqGameInput() = default;
	};
}