#include "pch.h"
#include "InGame.h"
#include "../System/ResourceLoader.h"
#include "../System/Renderer.h"
#include "../System/Input.h"
#include "../System/Server.h"

#include <vector>
#include <array>

IOCPTetris::InGameScene::InGameScene(int _roomIndex)
	:BaseScene()
{
	SetRoomIndex(_roomIndex);
}

void IOCPTetris::InGameScene::Start()
{
	tetrisBoard = new TetrisBoard();
	tetrisBoard->Init(Game::Get()->GetPlayerSessionIndex());

	enemyBoard = new TetrisBoard();
	enemyBoard->Init(0);

	{
		Packet::REQ_ROOM_ENTER req;
		Packet::RES_ROOM_ENTER res;

		req.roomIndex = GetRoomIndex();

		Server::Get()->SendSync(Packet::PACKET_ID::REQ_ROOM_ENTER, req, res);
	}

	gameStartButton = std::make_unique<GameStartButton>(200, 75);
	gameStartButton->SetPos(700,900);

	gameLeaveButton = std::make_unique<GameLeaveButton>(200, 75);
	gameLeaveButton->SetPos(930, 900);
}

void IOCPTetris::InGameScene::Update(const float& elapsedTime)
{
	
	if (Game::Get()->input->IsPressLeftButton())
	{
		XMFLOAT2 mousePos = Game::Get()->input->GetMousePos();

		if (gameStartButton->IsPressed(mousePos))
		{
			gameStartButton->OnPressed();
		}
		else if (gameLeaveButton->IsPressed(mousePos))
		{
			gameLeaveButton->OnPressed();
		}
	}

	if (tetrisBoard->IsPlaying() == false)
	{
		return;
	}

	auto inputType = Game::Get()->input->GetInput();
	if (inputType.empty() == false)
	{
		tetrisBoard->QueueInput(inputType);
	}

	// 업데이트 후 블럭 제너레이트가 필요한지 확인
	if (tetrisBoard->GetRemainFallingBlock() < Common::PACKET_FALLING_BLOCK_SIZE)
	{
		Server::Get()->Send(Packet::PACKET_ID::REQ_FALLING_BLOCK, Packet::REQ_FALLING_BLOCK());
	}

	tetrisBoard->Update(elapsedTime);


	/// <summary>
	/// 키 입력 서버로 보내기
	/// </summary>
	/// <param name="elapsedTime"></param>
	/// <returns></returns>
	auto appliedInputs = tetrisBoard->GetAppliedInputs();

	if(appliedInputs.empty() == false)
	{
		Packet::REQ_GAME_INPUT req;

		for (int i = 0; i < Common::PACKET_KEY_INPUT_SIZE; ++i)
		{
			req.inputs[i] = appliedInputs.front();
			appliedInputs.pop();
			++req.inputCnt;

			if (appliedInputs.empty())
			{
				break;
			}
		}
		Server::Get()->Send(Packet::PACKET_ID::REQ_GAME_INPUT, req);
	}
}

void IOCPTetris::InGameScene::Stop()
{
}

void IOCPTetris::InGameScene::Render()
{	
	const auto& result = tetrisBoard->GetRenderInfo();
	Game::Get()->renderer->Render(result, Game::Get()->resourceLoader, Vector2D(4, 0));

	const auto& enemyRender = enemyBoard->GetRenderInfo();
	Game::Get()->renderer->Render(enemyRender, Game::Get()->resourceLoader, Vector2D(24, 0));

	Game::Get()->renderer->Render(gameStartButton.get());
	Game::Get()->renderer->Render(gameLeaveButton.get());
}

void IOCPTetris::InGameScene::AddFallingBlock(IOCPTetris::ShapeType shape)
{
	tetrisBoard->QueueNextBlock(shape);
	enemyBoard->QueueNextBlock(shape);
}

void IOCPTetris::InGameScene::SetRoomIndex(int _roomIndex)
{
	roomIndex = _roomIndex;
}

int IOCPTetris::InGameScene::GetRoomIndex()
{
	return roomIndex;
}
