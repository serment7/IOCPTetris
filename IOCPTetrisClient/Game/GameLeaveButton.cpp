#include "pch.h"
#include "GameLeaveButton.h"
#include "../System/Server.h"
#include "Game.h"
#include "Scene/LobbyScene.h"

void GameLeaveButton::OnPressed()
{
	{
		Packet::REQ_GAME_START req;
		Server::Get()->Send(Packet::PACKET_ID::REQ_ROOM_LEAVE, req);
	}
}
