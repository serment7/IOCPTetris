#include "Session.h"
#include "Player.h"
#include "../System/ClientInfo.h"

Session::Session(int _sessionIndex, Player* _playerInfo, ClientInfo* _clientInfo)
	:sessionIndex(_sessionIndex)
{
	SetSesssion(_playerInfo, _clientInfo);
}

void Session::SetSesssion(Player* _playerInfo, ClientInfo* _clientInfo)
{
	playerInfo = _playerInfo;
	clientInfo = _clientInfo;

	playerInfo->sessionIndex = sessionIndex;
	clientInfo->sessionIndex = sessionIndex;
	isUse = true;
}

bool Session::IsUse()
{
	return isUse;
}

void Session::CloseSession()
{
	playerInfo->sessionIndex = 0;
	clientInfo->sessionIndex = 0;

	playerInfo = nullptr;
	clientInfo = nullptr;
	isUse = false;
}
