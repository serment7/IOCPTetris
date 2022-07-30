#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"

namespace Handler
{
	class ReqLogin : public BaseHandler<Packet::REQ_LOGIN, Packet::RES_LOGIN>
	{
	public:
		// BaseHandler을(를) 통해 상속됨
		virtual void OnResponse(const Packet::RES_LOGIN&& res) override;

		ReqLogin() = default;
		virtual ~ReqLogin() = default;
	};
}