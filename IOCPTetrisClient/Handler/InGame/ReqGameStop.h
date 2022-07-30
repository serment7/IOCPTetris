#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"

namespace Handler
{
	class ReqGameStop : public BaseHandler<Packet::REQ_GAME_STOP, Packet::RES_GAME_STOP>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_GAME_STOP&& res) override
		{

		}

		ReqGameStop() = default;
		virtual ~ReqGameStop() = default;
	};
}