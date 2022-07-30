#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"
#include "Scene/LobbyScene.h"

namespace Handler
{
	class ReqRoomList : public BaseHandler<Packet::REQ_ROOM_LIST, Packet::RES_ROOM_LIST>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_ROOM_LIST&& res) override
		{
			auto curScene = Game::Get()->GetCurrentScene<LobbyScene>();
			if (curScene)
			{
				std::vector<Common::RoomInfo> rooms;
				for (size_t i = 0; i < Common::ROOM_CNT_PER_PAGE; i++)
				{
					rooms.push_back(res.roomInfo[i]);
				}

				curScene->SetupRoomPage(req.roomPage, rooms);
			}
		}

		ReqRoomList() = default;
		virtual ~ReqRoomList() = default;
	};
}