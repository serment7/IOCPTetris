#pragma once

enum class ResultID : short
{
	NONE = 0,
	SUCCESS = 1,
	FAILED = 2,

	//ROOM
	ROOM_MAX_PLAYER = 101,
	ROOM_NOT_FOUND = 102,
	ROOM_IS_PLAYING = 103,

	END,
};