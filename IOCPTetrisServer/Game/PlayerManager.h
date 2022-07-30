#pragma once

#include <vector>

class Player;

class PlayerManager
{
public:
	int GetPlayerCount();

	Player* GetEmptyPlayer();

	void RemovePlayer(int playerUID);

	Player* FindPlayer(int playerUID);

	Player* FindPlayerBySessionIndex(int sessionIndex);

private:
	std::vector<Player*> players;
};

