#pragma once

#include <memory>

#include "System/RoomManager.h"
#include "System/SessionManager.h"
#include "Game/PlayerManager.h"

class Game
{
public:
	static Game* Get()
	{
		static Game instance;
		return &instance;
	}

	Game()
	{
		playerManager = std::make_unique<PlayerManager>();
		roomManager = std::make_unique<RoomManager>();
		sessionManager = std::make_unique<SessionManager>();
	}
	~Game() = default;

public:
	std::unique_ptr<RoomManager>  roomManager;
	std::unique_ptr<PlayerManager> playerManager;
	std::unique_ptr<SessionManager> sessionManager;

};