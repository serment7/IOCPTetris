#pragma once

#include "../../IOCPTetrisCommon/IOCPTetrisDef.h"
#include "../../IOCPTetrisCommon/ResultID.h"

#include <vector>
#include <algorithm>
#include <vector>

class Player;

class Room
{
public:
	void AddPlayer(Player* enterUser);

	size_t GetCurrentPlayerCnt();
	int GetMaxPlayerCnt();
	int GetRoomIndex();

	ResultID CanEnterPlayer();

	void RemovePlayer(int sessionIndex);
	void GameStart(int);
	void GameStop(int);
	void AddKeyInput(int, IOCPTetris::InputType*, int);

	Player* FindUser(int sessionIndex);

	Room(int _roomIndex, int _maxPlayerCnt);
	void GenerateFallingBlocks();
	std::vector<IOCPTetris::ShapeType> GetFallingBlock(Player* player, int fallingBlockCnt);

	Room() = default;
	~Room() = default;
private:
	std::vector<Player*> players;
	int maxPlayerCnt = 0;
	int roomIndex = 0;
	bool isPlaying = false;
	std::vector<IOCPTetris::ShapeType> fallingBlocks;
};

