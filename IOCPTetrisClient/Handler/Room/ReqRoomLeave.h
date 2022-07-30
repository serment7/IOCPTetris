#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "Scene/LobbyScene.h"

namespace Handler
{

	class ReqRoomLeave : public BaseHandler<Packet::REQ_ROOM_LEAVE, Packet::RES_ROOM_LEAVE>
	{
	public:
		// BaseHandler��(��) ���� ��ӵ�
		virtual void OnResponse(const Packet::RES_ROOM_LEAVE&& res) override
		{
			Game::Get()->SetPreservedScene(new LobbyScene());
		}

		ReqRoomLeave() = default;
		virtual ~ReqRoomLeave() = default;
	};
}