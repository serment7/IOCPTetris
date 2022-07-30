#pragma once

#include <WinSock2.h>
#include <algorithm>
#include <cassert>

class Player;

class IHandler
{
public:
	virtual void ReadReq(char* packet) = 0;
	virtual void WriteResponse(char* &packet, INT16& size) = 0;
	virtual void Logic(Player* player) = 0;
	virtual ~IHandler() = default;

protected:
	IHandler() = default;
};

template <typename Req, typename Res>
class BaseHandler : public IHandler
{
public:
	BaseHandler() = default;
	virtual ~BaseHandler() = default;
	virtual void ReadReq(char* packet)
	{
		//req = reinterpret_cast<Req*>(packet);
		std::copy(packet, packet + sizeof(Req), reinterpret_cast<char*>(&req));
		res.sequenceIndex = req.sequenceIndex;
		res.sessionIndex = req.sessionIndex;
		res.PacketId = req.PacketId + 1;
		res.PacketLength = sizeof(Res);
	}
	virtual void WriteResponse(char* &packet, INT16& size)
	{
		assert(packet == nullptr);

		packet = new char[sizeof(Res)];
		std::copy(reinterpret_cast<char*>(&res), reinterpret_cast<char*>(&res) + sizeof(Res), packet);
		size = sizeof(Res);

		std::copy(packet, packet + sizeof(Res), reinterpret_cast<char*>(&res));
	}

protected:
	Req req;
	Res res;
};