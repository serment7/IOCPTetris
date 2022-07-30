#pragma once

#include <vector>
#include <memory>

#include "../Game/Room.h"
#include "../../IOCPTetrisCommon/Common.h"

class RoomManager
{
public:
	int CreateRoom()
	{
		rooms.push_back(std::make_unique<Room>(roomIndex, Common::ROOM_MAX_PLAYER));
		return roomIndex++;
	}

	void RemoveRoom(int roomIndex)
	{
		for (size_t i = 0; i < rooms.size(); i++)
		{
			if (rooms[i]->GetRoomIndex() == roomIndex)
			{
				rooms.erase(rooms.begin() + i);
				return;
			}
		}
	}

	Room* FindRoom(int roomIndex)
	{
		for (auto& i : rooms)
		{
			if (i->GetRoomIndex() == roomIndex)
			{
				return i.get();
			}
		}
		
		return nullptr;
	}

	std::vector<Common::RoomInfo> GetRoomList(int roomPage)
	{
		std::vector<Common::RoomInfo> result;

		int endIndex = roomPage * Common::ROOM_CNT_PER_PAGE;

		if (endIndex > rooms.size())
		{
			endIndex = rooms.size();
		}

		int curIndex = endIndex - 8;
		if (curIndex < 0)
		{
			curIndex = 0;
		}

		for (; curIndex < endIndex; ++curIndex)
		{
			Common::RoomInfo roomInfo;
			roomInfo.currentPlayerCnt = rooms[curIndex]->GetCurrentPlayerCnt();
			roomInfo.maxPlayerCnt = rooms[curIndex]->GetMaxPlayerCnt();
			roomInfo.roomIndex = rooms[curIndex]->GetRoomIndex();
			result.push_back(roomInfo);
		}

		if (result.size() < Common::ROOM_CNT_PER_PAGE)
		{
			int emptyRoomCnt = Common::ROOM_CNT_PER_PAGE - result.size();
			for (int i = 0; i < emptyRoomCnt; ++i)
			{
				result.emplace_back();
			}
		}

		return result;
	}

	RoomManager() = default;
	~RoomManager() = default;

private:
	std::vector<std::unique_ptr<Room>> rooms;
	int roomIndex = 1;
};
