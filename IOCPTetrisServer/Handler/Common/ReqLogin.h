#pragma once

#include "../../../IOCPTetrisCommon/Packet.hh"
#include "../BaseHandler.h"

class Player;

namespace Handler
{

	class ReqLogin : public BaseHandler<Packet::REQ_LOGIN, Packet::RES_LOGIN>
	{
	public:
		virtual void Logic(Player* player) override;
		ReqLogin() = default;
		virtual ~ReqLogin() = default;
	};
}