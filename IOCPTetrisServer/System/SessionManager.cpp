#include "SessionManager.h"
//#include "../Game/Session.h"
//#include "../Game/Player.h"
//#include "ClientInfo.h"

void SessionManager::AddSession(Player* playerInfo, SBNet::ClientInfo* clientInfo)
{
	for (auto& i : sessions)
	{
		if (i->IsUse() == false)
		{
			i->SetSesssion(playerInfo, clientInfo);
			return;
		}
	}

	sessions.push_back(new Session(sessions.size() + 1, playerInfo, clientInfo));
}

void SessionManager::RemoveSession(int sessionIndex)
{
	sessions.at(--sessionIndex)->CloseSession();
}

Session* SessionManager::FindSession(int sessionIndex)
{
	return sessions.at(--sessionIndex);
}

Session* SessionManager::FindSession(Player* player)
{
	return FindSession(player->sessionIndex);
}
