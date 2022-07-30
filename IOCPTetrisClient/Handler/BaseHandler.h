#pragma once

#include <algorithm>
#include "../../IOCPTetrisCommon/Packet.hh"
#include "Game.h"

class IHandler
{
public:
	virtual void SetRequest(char* _req, INT16 size) = 0;
	virtual void Response(char* buffer, INT16 size) = 0;
	virtual ~IHandler() = default;

protected:
	IHandler() = default;
};

template<typename Req, typename Res>
class BaseHandler : public IHandler
{
public:
	virtual ~BaseHandler() = default;
	void SetRequest(char* _req, INT16 size) final override
	{
		auto resSize = sizeof(Req);
		assert(resSize == size);
		std::copy(_req, _req + size, reinterpret_cast<char*>(&req));
	}

	void Response(char* _res, INT16 size) final override
	{
		auto resSize = sizeof(Res);
		assert(resSize == size);
		Res res;
		std::copy(_res, _res + size, reinterpret_cast<char*>(&res));
		OnResponse(std::move(res));
	}

protected:
	BaseHandler() = default;
	Req req;
	virtual void OnResponse(const Res&& res) = 0;
};