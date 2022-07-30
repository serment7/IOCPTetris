#pragma once

#include "../IOCPTetrisNetDef.hh"
#include "../../IOCPTetrisCommon/PacketBuffer.h"
#include <mutex>
#include <memory>
#include <queue>

class ClientInfo
{
public:
	ClientInfo();
	~ClientInfo() = default;

	void Init(const UINT32 index, HANDLE iocpHandle_);

	UINT32 GetClientIndex();

	UINT32 GetSessionIndex();

	bool IsConnect();

	SOCKET GetSock();

	UINT64 GetLatestClosedTimeSec();

	char* RecvBuffer();


	bool Connect(HANDLE iocpHandle_, SOCKET socket_);

	void Disconnect(bool bIsForce = false);

	void Clear();

	bool PostAccept(SOCKET listenSock_, const UINT64 curTimeSec_);

	bool AcceptCompletion();

	bool BindIOCompletionPort(HANDLE iocpHandle_);

	bool BindRecv();

	bool SendMsg(const UINT32 dataSize_, char* pMsg_, Packet::IOOperation operation = Packet::IOOperation::SEND);

	void SendCompleted(const UINT32 dataSize_);

	void OnRecv(char* buf, DWORD size)
	{
		std::lock_guard<std::mutex> guard(readBufLock);
		packetBuffer->Write(buf, size);
	}

	char* ReadPacket()
	{
		std::lock_guard<std::mutex> guard(readBufLock);
		return packetBuffer->Read();
	}

	INT32 clientIndex = 0;
	INT32 sessionIndex = 0;
	INT32 playerUID;
	SOCKET			sock = INVALID_SOCKET;
	char clientIP[32] = { 0,};

private:
	bool SendIO(Packet::OverlappedEx* sendOverlappedEx);

	bool SetSocketOption();

	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;

	INT64 mIsConnect = 0;
	UINT64 mLatestClosedTimeSec = 0;

	Packet::OverlappedEx	mAcceptContext;
	char mAcceptBuf[64] = { 0, };

	Packet::OverlappedEx	mRecvOverlappedEx;
	char			mRecvBuf[SBNet::MAX_SOCK_RECVBUF];

	std::mutex mSendLock, readBufLock;
	std::queue<Packet::OverlappedEx*> mSendDataqueue;
	std::unique_ptr<PacketBuffer> packetBuffer;

};