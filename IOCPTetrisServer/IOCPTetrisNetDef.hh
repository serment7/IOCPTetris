#pragma once

#include "../IOCPTetrisCommon/Packet.hh"

namespace SBNet
{
	const int MAX_SOCKBUF = 1024;
	const int MAX_SOCK_RECVBUF = 256;
	const int MAX_SOCK_SENDBUF = 4096;
	const int RE_USE_SESSION_WAIT_TIMESEC = 3;

	enum class ServerType : short
	{
		NONE,
		MAINGAMESERVER,
	};
}