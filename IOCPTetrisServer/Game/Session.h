#pragma once

class Player;
class ClientInfo;

class Session
{
public:
	Session(int _sessionIndex, Player* _playerInfo, ClientInfo* _clientInfo);

	void SetSesssion(Player* _playerInfo, ClientInfo* _clientInfo);

	bool IsUse();

	void CloseSession();

	Player* playerInfo = nullptr;
	ClientInfo* clientInfo = nullptr;
	int sessionIndex = 0;
	bool isUse = false;
};
