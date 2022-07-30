#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../../../IOCPTetrisCommon/TetrisBoard.h"
#include "../BaseHandler.h"
#include "Scene/InGame.h"

namespace Handler
{

	class ReqRoomEnter : public BaseHandler<Packet::REQ_ROOM_ENTER, Packet::RES_ROOM_ENTER>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_ROOM_ENTER&& res) override
		{
			auto scene = Game::Get()->GetCurrentScene<IOCPTetris::InGameScene>();

			if (scene == nullptr)
			{
				return;
			}

			// 세션 ID가 다르다면 다른 유저가 있는 것이다.
			if (Game::Get()->GetPlayerSessionIndex() == res.enterSessionIndex)
			{	
				scene->GetMyBoard()->Init(res.enterSessionIndex);
			}
			else
			{
				scene->GetEnemyBoard()->Init(res.enterSessionIndex);
			}
		}

		ReqRoomEnter() = default;
		virtual ~ReqRoomEnter() = default;
	};
}