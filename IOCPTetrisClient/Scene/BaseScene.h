#pragma once

#include "../DeviceResources.h"
#include "Game.h"
#include <memory>

class Input;
class ResourceLoader;
class Renderer;

namespace IOCPTetris
{
	class BaseScene
	{
	public:
		virtual void Start() = 0;
		virtual void Update(const float&) = 0;
		virtual void Stop() = 0;
		virtual void Render() = 0;
		virtual ~BaseScene() = default;
		void Reset();
		void Init();
	protected:
		BaseScene();
	};
}