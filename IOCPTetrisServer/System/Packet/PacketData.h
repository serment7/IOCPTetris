#pragma once

#include <WinSock2.h>

namespace IOCPTetris
{
	class PacketData
	{
	public:
		UINT32 SessionIndex = 0;
		UINT32 DataSize = 0;
		char* pPacketData = nullptr;

		void Set(PacketData& vlaue);

		void Set(UINT32 sessionIndex_, UINT32 dataSize_, char* pData);

		void Release();
	};
}