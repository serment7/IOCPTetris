#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"

namespace Handler
{

	class ReqRoomCanEnter : public BaseHandler<Packet::REQ_ROOM_CAN_ENTER, Packet::RES_ROOM_CAN_ENTER>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_ROOM_CAN_ENTER&& res) override
		{
			
		}

		ReqRoomCanEnter() = default;
		virtual ~ReqRoomCanEnter() = default;
	};
}