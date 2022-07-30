#pragma once



namespace Common
{
	const int ROOM_CNT_PER_PAGE = 8;
	const int PACKET_KEY_INPUT_SIZE = 30;
	const int PACKET_FALLING_BLOCK_SIZE = 14;
	const int ROOM_MAX_PLAYER = 2;


#pragma pack(push,1)

	struct RoomInfo
	{
		int maxPlayerCnt = 0;
		int roomIndex = 0;
		int currentPlayerCnt = 0;
		bool isPlaying = false;
	};
#pragma pack(pop)
}

