#pragma once

#include <string>

class Player
{
public:
	int sessionIndex = 0;
	int playerUID = 0;
	std::string userName;
	int roomIndex = 0;
	int fallingBlockIndex = 0;
};
