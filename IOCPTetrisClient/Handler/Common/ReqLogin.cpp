#include "pch.h"
#include "ReqLogin.h"
#include "System/Server.h"

// BaseHandler을(를) 통해 상속됨
void Handler::ReqLogin::OnResponse(const Packet::RES_LOGIN&& res)
{
	Game::Get()->SetPlayerSessionIndex(res.newSessionID);
}
