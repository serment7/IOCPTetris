#include "ClientInfo.h"
#include "../../IOCPTetrisCommon/Packet.hh"

SBNet::ClientInfo::ClientInfo()
{
	ZeroMemory(&mRecvOverlappedEx, sizeof(Packet::OverlappedEx));

	packetBuffer = std::make_unique<SBNet::PacketBuffer>();
	packetBuffer->Init(1024, sizeof(Packet::PACKET_HEADER));
}

void SBNet::ClientInfo::Init(const UINT32 index, HANDLE iocpHandle_)
{
	clientIndex = index;
	mIOCPHandle = iocpHandle_;
}

UINT32 SBNet::ClientInfo::GetClientIndex() {
	return clientIndex;
}

UINT32 SBNet::ClientInfo::GetSessionIndex() {
	return sessionIndex;
}

bool SBNet::ClientInfo::IsConnect() {
	return mIsConnect == 1;
}

SOCKET SBNet::ClientInfo::GetSock() {
	return sock;
}

UINT64 SBNet::ClientInfo::GetLatestClosedTimeSec() {
	return mLatestClosedTimeSec;
}

char* SBNet::ClientInfo::RecvBuffer() {
	return mRecvBuf;
}

bool SBNet::ClientInfo::Connect(HANDLE iocpHandle_, SOCKET socket_)
{
	sock = socket_;
	mIsConnect = 1;

	Clear();

	if (BindIOCompletionPort(iocpHandle_) == false)
	{
		return false;
	}

	return BindRecv();
}

void SBNet::ClientInfo::Disconnect(bool bIsForce)
{
	struct linger stLinger = { 0, 0 };


	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	shutdown(sock, SD_BOTH);
	setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	mIsConnect = 0;
	mLatestClosedTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	
	closesocket(sock);
	sock = INVALID_SOCKET;
}

void SBNet::ClientInfo::Clear()
{
}

bool SBNet::ClientInfo::PostAccept(SOCKET listenSock_, const UINT64 curTimeSec_)
{
	mLatestClosedTimeSec = UINT32_MAX;

	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == sock)
	{
		printf_s("client Socket WSASocket Error : %d\n", GetLastError());
		return false;
	}

	ZeroMemory(&mAcceptContext, sizeof(Packet::OverlappedEx));

	DWORD bytes = 0;
	DWORD flags = 0;
	mAcceptContext.wsaBuf.len = 0;
	mAcceptContext.wsaBuf.buf = nullptr;
	mAcceptContext.operation = Packet::IOOperation::ACCEPT;
	mAcceptContext.clientIndex = clientIndex;

	if (FALSE == AcceptEx(listenSock_, sock, mAcceptBuf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPWSAOVERLAPPED) & (mAcceptContext)))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("AcceptEx Error : %d\n", GetLastError());
			return false;
		}
	}

	mLatestClosedTimeSec = curTimeSec_;

	return true;
}

bool SBNet::ClientInfo::AcceptCompletion()
{
	if (Connect(mIOCPHandle, sock) == false)
	{
		return false;
	}

	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);

	inet_ntop(AF_INET, &(stClientAddr.sin_addr), clientIP, 32 - 1);

	return true;
}

bool SBNet::ClientInfo::BindIOCompletionPort(HANDLE iocpHandle_)
{
	auto hIOCP = CreateIoCompletionPort((HANDLE)GetSock()
		, iocpHandle_
		, (ULONG_PTR)(this), 0);

	if (hIOCP == INVALID_HANDLE_VALUE)
	{
		printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
		return false;
	}

	return true;
}

bool SBNet::ClientInfo::BindRecv()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	mRecvOverlappedEx.wsaBuf.len = SBNet::MAX_SOCK_RECVBUF;
	mRecvOverlappedEx.wsaBuf.buf = mRecvBuf;
	mRecvOverlappedEx.operation = Packet::IOOperation::RECV;

	int nRet = WSARecv(sock,
		&(mRecvOverlappedEx.wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mRecvOverlappedEx),
		NULL);

	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool SBNet::ClientInfo::SendMsg(const UINT32 dataSize_, char* pMsg_, Packet::IOOperation operation)
{
	auto* sendOverlappedEx = new Packet::OverlappedEx;
	ZeroMemory(sendOverlappedEx, sizeof(Packet::OverlappedEx));
	sendOverlappedEx->wsaBuf.len = dataSize_;
	sendOverlappedEx->wsaBuf.buf = new char[dataSize_];
	CopyMemory(sendOverlappedEx->wsaBuf.buf, pMsg_, dataSize_);
	sendOverlappedEx->operation = operation;
	sendOverlappedEx->clientIndex = clientIndex;

	std::lock_guard<std::mutex> guard(mSendLock);

	mSendDataqueue.push(sendOverlappedEx);

	if (mSendDataqueue.size() == 1)
	{
		SendIO(sendOverlappedEx);
	}

	return true;
}

void SBNet::ClientInfo::SendCompleted(const UINT32 dataSize_)
{
	std::lock_guard<std::mutex> guard(mSendLock);

	auto data = mSendDataqueue.front();
	mSendDataqueue.pop();

	delete[] data->wsaBuf.buf;
	delete data;

	if (mSendDataqueue.empty() == false)
	{
		SendIO(mSendDataqueue.front());
	}
}

bool SBNet::ClientInfo::SendIO(Packet::OverlappedEx* sendOverlappedEx)
{
	DWORD dwRecvNumBytes = 0;
	int nRet = WSASend(sock,
		&(sendOverlappedEx->wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED)sendOverlappedEx,
		NULL);

	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool SBNet::ClientInfo::SetSocketOption()
{
	/*if (SOCKET_ERROR == setsockopt(mSock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)GIocpManager->GetListenSocket(), sizeof(SOCKET)))
	{
	printf_s("[DEBUG] SO_UPDATE_ACCEPT_CONTEXT error: %d\n", GetLastError());
	return false;
	}*/

	int opt = 1;
	if (SOCKET_ERROR == setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int)))
	{
		printf_s("[DEBUG] TCP_NODELAY error: %d\n", GetLastError());
		return false;
	}

	opt = 0;
	if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)&opt, sizeof(int)))
	{
		printf_s("[DEBUG] SO_RCVBUF change error: %d\n", GetLastError());
		return false;
	}

	return true;
}
