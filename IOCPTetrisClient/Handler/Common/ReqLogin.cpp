#include "pch.h"
#include "ReqLogin.h"
#include "System/Server.h"

// BaseHandler��(��) ���� ��ӵ�
void Handler::ReqLogin::OnResponse(const Packet::RES_LOGIN&& res)
{
	Game::Get()->SetPlayerSessionIndex(res.newSessionID);
}
