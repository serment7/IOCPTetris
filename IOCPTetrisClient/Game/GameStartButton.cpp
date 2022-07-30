#include "pch.h"
#include "GameStartButton.h"
#include "../System/Server.h"
#include "Game.h"
#include "Scene/InGame.h"

void GameStartButton::OnPressed()
{
	{
		Packet::REQ_GAME_START req;
		Server::Get()->Send(Packet::PACKET_ID::REQ_GAME_START, req);
	}
}

