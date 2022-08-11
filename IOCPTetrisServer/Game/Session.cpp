#include "Session.h"
#include "Player.h"
#include "../System/ClientInfo.h"

IOCPTetris::Session::Session(int _sessionIndex, Player* _playerInfo, SBNet::ClientInfo* _clientInfo)
	:sessionIndex(_sessionIndex)
{
	SetSesssion(_playerInfo, _clientInfo);
}

void IOCPTetris::Session::SetSesssion(Player* _playerInfo, SBNet::ClientInfo* _clientInfo)
{
	playerInfo = _playerInfo;
	clientInfo = _clientInfo;

	playerInfo->sessionIndex = sessionIndex;
	clientInfo->sessionIndex = sessionIndex;
	isUse = true;
}

bool IOCPTetris::Session::IsUse()
{
	return isUse;
}

void IOCPTetris::Session::CloseSession()
{
	playerInfo->sessionIndex = 0;
	clientInfo->sessionIndex = 0;

	playerInfo = nullptr;
	clientInfo = nullptr;
	isUse = false;
}
