#include "PlayerManager.h"
#include "Player.h"

#include <algorithm>

int PlayerManager::GetPlayerCount()
{
	return players.size();
}

Player* PlayerManager::GetEmptyPlayer()
{
	for (int i = 0; i < players.size(); ++i)
	{
		if (players[i] == nullptr)
		{
			players[i] = new Player();;
			players[i]->playerUID = i;
			return players[i];
		}
	}

	int newPlayerUID = players.size();

	players.push_back(new Player());
	players[newPlayerUID]->playerUID = newPlayerUID;

	return players[newPlayerUID];
}

void PlayerManager::RemovePlayer(int playerUID)
{
	auto target = std::find_if(players.begin(), players.end(), [playerUID](Player* x) {
		return x->playerUID == playerUID;
	});
	if (target != players.end())
	{
		delete* target;
		*target = nullptr;
	}
}

Player* PlayerManager::FindPlayer(int playerUID)
{
	auto target = std::find_if(players.begin(), players.end(), [playerUID](Player* x) {
		return x->playerUID == playerUID;
	});
	if (target != players.end())
	{
		return *target;
	}

	return nullptr;
}

Player* PlayerManager::FindPlayerBySessionIndex(int sessionIndex)
{
	auto target = std::find_if(players.begin(), players.end(), [sessionIndex](Player* x) {
		return x->sessionIndex == sessionIndex;
	});
	if (target != players.end())
	{
		return *target;
	}

	return nullptr;
}
