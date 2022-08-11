#pragma once

class Player;
namespace SBNet
{
	class ClientInfo;
}

namespace IOCPTetris
{
	class Session
	{
	public:
		Session(int _sessionIndex, Player* _playerInfo, SBNet::ClientInfo* _clientInfo);

		void SetSesssion(Player* _playerInfo, SBNet::ClientInfo* _clientInfo);

		bool IsUse();

		void CloseSession();

		Player* playerInfo = nullptr;
		SBNet::ClientInfo* clientInfo = nullptr;
		int sessionIndex = 0;
		bool isUse = false;
	};

}