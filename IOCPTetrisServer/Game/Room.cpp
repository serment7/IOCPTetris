#include "Room.h"
#include "Player.h"
#include "../Game.h"
#include "../../IOCPTetrisCommon/Packet.hh"
#include <random>
#include <algorithm>

void Room::AddPlayer(Player* enterUser)
{
	if (players.size() >= maxPlayerCnt)
	{
		return;
	}

	Packet::RES_ROOM_ENTER res;
	res.enterSessionIndex = enterUser->sessionIndex;

	for (auto& r : players)
	{
		Game::Get()->sessionManager->SendNotify(r->sessionIndex, Packet::PACKET_ID::RES_ROOM_ENTER, res);

		Packet::RES_ROOM_ENTER newUserRes;
		newUserRes.enterSessionIndex = r->sessionIndex;
		Game::Get()->sessionManager->SendNotify(enterUser->sessionIndex, Packet::PACKET_ID::RES_ROOM_ENTER, newUserRes);
	}

	enterUser->roomIndex = roomIndex;
	players.push_back(enterUser);
}

size_t Room::GetCurrentPlayerCnt()
{
	return players.size();
}

int Room::GetMaxPlayerCnt()
{
	return maxPlayerCnt;
}

int Room::GetRoomIndex()
{
	return roomIndex;
}

ResultID Room::CanEnterPlayer()
{
	if (GetCurrentPlayerCnt() >= GetMaxPlayerCnt())
	{
		return ResultID::ROOM_MAX_PLAYER;
	}
	else if (isPlaying)
	{
		return ResultID::ROOM_IS_PLAYING;
	}

	return ResultID::SUCCESS;
}

void Room::RemovePlayer(int sessionIndex)
{
	auto target = std::find_if(players.begin(), players.end(), [sessionIndex](Player* x) {
		if (x != nullptr)
		{
			return x->sessionIndex == sessionIndex;
		}

		return false;
	});

	if (target != players.end())
	{
		(*target)->roomIndex = 0;
		players.erase(target);
	}
}

void Room::GameStart(int sessionIndex)
{
	isPlaying = true;

	fallingBlocks.clear();
	GenerateFallingBlocks();

	for (auto& r : players)
	{
		if (r->sessionIndex == sessionIndex)
		{
			continue;
		}

		r->fallingBlockIndex = 0;

		Packet::RES_GAME_START res;
		auto result = GetFallingBlock(r, Common::PACKET_FALLING_BLOCK_SIZE);
		for (size_t i = 0; i < Common::PACKET_FALLING_BLOCK_SIZE; i++)
		{
			res.newBlocks[i] = result[i];
		}

		Game::Get()->sessionManager->SendNotify(r->sessionIndex, Packet::PACKET_ID::RES_GAME_START, res);
	}
}

void Room::GameStop(int sessionIndex)
{
	isPlaying = false;

	for (auto& r : players)
	{
		if (r->sessionIndex == sessionIndex)
		{
			continue;
		}

		Game::Get()->sessionManager->SendNotify(r->sessionIndex, Packet::PACKET_ID::RES_GAME_STOP, Packet::RES_GAME_STOP());
	}
}

void Room::AddKeyInput(int sessionIndex, IOCPTetris::InputType* inputs, int cnt)
{
	if (cnt == 0)
	{
		return;
	}

	Packet::RES_GAME_INPUT res;
	std::copy(inputs, inputs + cnt, res.inputs);

	for (auto& i : players)
	{
		if (i->sessionIndex == sessionIndex)
		{
			continue;
		}

		Game::Get()->sessionManager->SendNotify(i->sessionIndex, Packet::PACKET_ID::RES_GAME_INPUT, res);
	}
}

Player* Room::FindUser(int sessionIndex)
{
	for (auto& player : players)
	{
		if (player->sessionIndex == sessionIndex)
		{
			return player;
		}
	}

	return nullptr;
}

Room::Room(int _roomIndex, int _maxPlayerCnt)
	:roomIndex(_roomIndex), maxPlayerCnt(_maxPlayerCnt)
{

}

void Room::GenerateFallingBlocks()
{
	std::vector<IOCPTetris::ShapeType> newFallingBlocks = {
		IOCPTetris::ShapeType::Z,
		IOCPTetris::ShapeType::L,
		IOCPTetris::ShapeType::O,
		IOCPTetris::ShapeType::S,
		IOCPTetris::ShapeType::I,
		IOCPTetris::ShapeType::J,
		IOCPTetris::ShapeType::T
	};

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::random_shuffle(newFallingBlocks.begin(), newFallingBlocks.end());

	for (int i = 0; i < 7; ++i)
	{
		fallingBlocks.push_back(newFallingBlocks[i]);
	}
}

std::vector<IOCPTetris::ShapeType> Room::GetFallingBlock(Player* player, int fallingBlockCnt)
{
	player->fallingBlockIndex += fallingBlockCnt;

	// 요청한 개수보다 준비된게 적으면 새로 만든다.
	while (player->fallingBlockIndex >= fallingBlocks.size())
	{
		GenerateFallingBlocks();
	}

	return std::vector<IOCPTetris::ShapeType>(fallingBlocks.begin() + player->fallingBlockIndex - fallingBlockCnt, fallingBlocks.begin() + player->fallingBlockIndex);
}
