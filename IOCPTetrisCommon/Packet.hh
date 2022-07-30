#pragma once

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <cstdint>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "Result.h"
#include "Common.h"
#include "IOCPTetrisDef.h"

namespace Packet
{

	enum class IOOperation
	{
		ACCEPT,
		RECV,
		SEND,
		NOTIFY,
	};

	struct OverlappedEx
	{
		WSAOVERLAPPED overlapped;
		WSABUF wsaBuf;
		IOOperation operation;
		int clientIndex;
	};


	class PacketInfo
	{
	public:
		uint16_t DataSize = 0;
		uint16_t PacketId = 0;
		uint32_t SessionIndex = 0;
		uint32_t SequenceIndex = 0;
		char* pDataPtr = nullptr;
	};

	enum class  PACKET_ID : uint16_t
	{
		NONE = 0,

		//SYSTEM

		REQ_LOGIN = 10001,
		RES_LOGIN = 10002,

		// ROOM
		REQ_ROOM_CREATE = 10101,
		RES_ROOM_CREATE = 10102,
		REQ_ROOM_ENTER = 10103,
		RES_ROOM_ENTER = 10104,
		REQ_ROOM_LIST = 10105,
		RES_ROOM_LIST = 10106,
		REQ_ROOM_LEAVE = 10107,
		RES_ROOM_LEAVE = 10108,
		REQ_ROOM_CAN_ENTER = 10109,
		RES_ROOM_CAN_ENTER = 10110,

		// GAME
		REQ_GAME_STOP = 10201,
		RES_GAME_STOP = 10202,
		REQ_GAME_START = 10203,
		RES_GAME_START = 10204,
		REQ_GAME_INPUT = 10205,
		RES_GAME_INPUT = 10206,
		REQ_FALLING_BLOCK = 10207,
		RES_FALLING_BLOCK = 10208,
	};

#pragma pack(push,1)
	struct PACKET_HEADER
	{
		uint16_t PacketLength;
		uint16_t PacketId;
		uint32_t sequenceIndex;
		uint32_t sessionIndex;
	};

	const uint32_t PACKET_HEADER_LENGTH = sizeof(PACKET_HEADER);

	//- 로그인 요청
	const int MAX_USER_ID_LEN = 32;
	const int MAX_USER_PW_LEN = 32;

	struct BASE_PACKET : public PACKET_HEADER
	{
		Result result;
	};

	struct REQ_LOGIN : public BASE_PACKET
	{

	};

	struct RES_LOGIN : public BASE_PACKET
	{
		char UserID[MAX_USER_ID_LEN + 1] = { 0, };
		int newSessionID = 0;
	};

	struct REQ_ROOM_CREATE : public BASE_PACKET
	{
		
	};

	struct RES_ROOM_CREATE : public BASE_PACKET
	{
		int newRoomIndex = 0;
	};

	struct REQ_ROOM_ENTER : public BASE_PACKET
	{
		int roomIndex = 0;
	};

	struct RES_ROOM_ENTER : public BASE_PACKET
	{
		int enterSessionIndex = 0;
	};

	struct REQ_ROOM_LIST : public BASE_PACKET
	{
		int roomPage = 0;
	};

	struct RES_ROOM_LIST : public BASE_PACKET
	{
		Common::RoomInfo roomInfo[Common::ROOM_CNT_PER_PAGE];
	};

	struct REQ_ROOM_LEAVE : public BASE_PACKET
	{
	};

	struct RES_ROOM_LEAVE : public BASE_PACKET
	{
	};

	struct REQ_ROOM_CAN_ENTER : public BASE_PACKET
	{
		int roomIndex;
	};

	struct RES_ROOM_CAN_ENTER : public BASE_PACKET
	{
	};

	struct REQ_GAME_START : public BASE_PACKET
	{
	};

	struct RES_GAME_START : public BASE_PACKET
	{
		IOCPTetris::ShapeType newBlocks[Common::PACKET_FALLING_BLOCK_SIZE] = { IOCPTetris::ShapeType::NONE, };
	};

	struct REQ_GAME_STOP : public BASE_PACKET
	{
	};

	struct RES_GAME_STOP : public BASE_PACKET
	{
	};

	struct REQ_GAME_INPUT : public BASE_PACKET
	{
		IOCPTetris::InputType inputs[Common::PACKET_KEY_INPUT_SIZE] = { IOCPTetris::InputType::NONE, };
		int inputCnt = 0;
		//float elapsedTime = 0.f;
	};

	struct RES_GAME_INPUT : public BASE_PACKET
	{
		IOCPTetris::InputType inputs[Common::PACKET_KEY_INPUT_SIZE] = { IOCPTetris::InputType::NONE, };
		//float elapsedTime = 0.f;
	};

	struct REQ_FALLING_BLOCK : public BASE_PACKET
	{
	};

	struct RES_FALLING_BLOCK : public BASE_PACKET
	{
		IOCPTetris::ShapeType newBlocks[Common::PACKET_FALLING_BLOCK_SIZE] = { IOCPTetris::ShapeType::NONE, };
	};

#pragma pack(pop)


}
