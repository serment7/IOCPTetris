#include "pch.h"
#include "RoomButton.h"
#include "../System/Server.h"
#include "Game.h"
#include "Scene/InGame.h"

void RoomButton::OnPressed()
{
	if (roomIndex == 0)
	{
		Packet::REQ_ROOM_CREATE req;
		Packet::RES_ROOM_CREATE res;
		
		Server::Get()->SendSync(Packet::PACKET_ID::REQ_ROOM_CREATE, req, res);

		if (res.result.resultID == ResultID::SUCCESS)
		{
			BaseScene* newScene = new InGameScene(res.newRoomIndex);
			Game::Get()->SetPreservedScene(newScene);
		}
	}
	else
	{
		Packet::REQ_ROOM_CAN_ENTER req;
		Packet::RES_ROOM_CAN_ENTER res;

		req.roomIndex = roomIndex;

		Server::Get()->SendSync(Packet::PACKET_ID::REQ_ROOM_CAN_ENTER, req, res);

		if (res.result.resultID == ResultID::SUCCESS)
		{
			BaseScene* newScene = new InGameScene(req.roomIndex);
			Game::Get()->SetPreservedScene(newScene);
		}
		
	}
}
