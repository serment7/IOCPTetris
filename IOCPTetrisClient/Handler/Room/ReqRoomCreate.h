#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "Scene/InGame.h"

namespace Handler
{

	class ReqRoomCreate : public BaseHandler<Packet::REQ_ROOM_CREATE, Packet::RES_ROOM_CREATE>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_ROOM_CREATE&& res) override
		{
			Game::Get()->SetPreservedScene(new IOCPTetris::InGameScene(res.newRoomIndex));
		}

		ReqRoomCreate() = default;
		virtual ~ReqRoomCreate() = default;
	};
}