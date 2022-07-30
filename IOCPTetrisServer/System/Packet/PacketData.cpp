#include "PacketData.h"

void IOCPTetris::PacketData::Set(PacketData& vlaue)
{
	SessionIndex = vlaue.SessionIndex;
	DataSize = vlaue.DataSize;

	pPacketData = new char[vlaue.DataSize];
	CopyMemory(pPacketData, vlaue.pPacketData, vlaue.DataSize);
}

void IOCPTetris::PacketData::Set(UINT32 sessionIndex_, UINT32 dataSize_, char* pData)
{
	SessionIndex = sessionIndex_;
	DataSize = dataSize_;

	pPacketData = new char[dataSize_];
	CopyMemory(pPacketData, pData, dataSize_);
}

void IOCPTetris::PacketData::Release()
{
	delete pPacketData;
}
