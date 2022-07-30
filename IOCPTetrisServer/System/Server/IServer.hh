#pragma once

#include "../../ServerConfig.hh"

namespace SBNet
{
	class IServer
	{
	public:
		virtual void Init(const ServerConfig& _config) = 0;
		virtual void Start() = 0;
		virtual void Shutdown() = 0;

		virtual ~IServer() = default;
	protected:
		IServer() = default;
		
	};

}
