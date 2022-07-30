#pragma once

#include "BaseScene.h"
#include "Game/RoomButton.h"
#include "../../IOCPTetrisCommon/Common.h"
#include<vector>

namespace IOCPTetris
{
	class LobbyScene : public BaseScene
	{
	public:
		LobbyScene();
		// BaseScene을(를) 통해 상속됨
		virtual void Start() override;
		virtual void Update(const float&) override;
		virtual void Stop() override;
		virtual void Render() override;

		void SetupRoomPage(int roomPage, std::vector<Common::RoomInfo> roomInfos);

	private:
		int currentRoomPage = 1;

		std::vector<std::shared_ptr<RoomButton>> m_buttons;
	};
}