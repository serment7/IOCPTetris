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
		// BaseHandler��(��) ���� ��ӵ�
		virtual void OnResponse(const Packet::RES_ROOM_ENTER&& res) override
		{
			auto scene = Game::Get()->GetCurrentScene<IOCPTetris::InGameScene>();

			if (scene == nullptr)
			{
				return;
			}

			// ���� ID�� �ٸ��ٸ� �ٸ� ������ �ִ� ���̴�.
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