#pragma once

#include "pch.h"
#include "BaseScene.h"
#include "Game/GameStartButton.h"
#include "Game/GameLeaveButton.h"
#include "../IOCPTetrisCommon/TetrisBoard.h"
#include <queue>

const float KeyInputCoolTime = 1.0f;
namespace IOCPTetris
{
	class TetrisBoard;

	class InGameScene :
		public BaseScene
	{
	public:
		InGameScene(int _roomIndex);
		virtual void Start();
		void Update(const float&);
		virtual void Stop();
		virtual void Render() override;
		void AddFallingBlock(IOCPTetris::ShapeType shape);

		TetrisBoard* GetMyBoard()
		{
			return tetrisBoard;
		}

		TetrisBoard* GetEnemyBoard()
		{
			return enemyBoard;
		}

		void GameStart()
		{
			tetrisBoard->Start();
			enemyBoard->Start();
			gameStartButton->SetEnable(false);
		}

		void GameStop()
		{
			tetrisBoard->Stop();
			enemyBoard->Stop();
			gameStartButton->SetEnable(true);
		}

		void SetRoomIndex(int);
		int GetRoomIndex();

	private:
		TetrisBoard* tetrisBoard = nullptr;
		TetrisBoard* enemyBoard = nullptr;
		float inputCoolTime = INPUT_COOLTIME;
		std::queue<IOCPTetris::InputType> updatedInputs;
		int roomIndex = 0;
		std::unique_ptr<GameStartButton> gameStartButton;
		std::unique_ptr<GameLeaveButton> gameLeaveButton;
	};
}
