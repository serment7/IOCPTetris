#pragma once

#include "IOCPTetrisDef.h"
#include <deque>
#include <algorithm>
#include <array>
#include <random>
#include <vector>
#include <chrono>
#include <cmath>
#include <queue>

namespace IOCPTetris
{
	class TetrisBoard
	{
	public:
		TetrisBoard() = default;
		virtual ~TetrisBoard()
		{
			SAFE_DELETE(hold);
			SAFE_DELETE(fallingBlock);
			while (nextFallingBlockQueue.empty() == false)
			{
				auto* nextShape = nextFallingBlockQueue.front();
				nextFallingBlockQueue.pop_front();
				SAFE_DELETE(nextShape);
			}
		}

		void Init(int _sessionIndex)
		{
			for (int height = 0; height < TETRIS_BOARD_HEIGHT; ++height)
			{
				for (int width = 0; width < TETRIS_BOARD_WIDTH; ++width)
				{
					board[height][width].SetPos(width, height);
					board[height][width].blockType = IOCPTetris::BlockType::NONE;
					board[height][width].resourceID = IOCPTetris::ResourceID::EMPTY;
				}
			}

			if (_sessionIndex != 0)
			{
				playerSessionIndex = _sessionIndex;
			}

			isPlaying = false;

			for (auto& i : nextFallingBlockQueue)
			{
				delete i;
			}

			nextFallingBlockQueue.clear();

			SAFE_DELETE(fallingBlock);
			SAFE_DELETE(hold);

			auto temp = std::queue<InputType>();
			applyQueue.swap(temp);
			temp = std::queue<InputType>();
			inputQueue.swap(temp);

			lastFallingTime = 0.f;
		}

		void Start()
		{
			isPlaying = true;
			canHold = true;
		}

		void Stop()
		{
			Init(playerSessionIndex);
		}

		void Update(float elapsedTime)
		{
			auto temp = std::queue<InputType>();
			applyQueue.swap(temp);

			if (isPlaying == false)
			{
				return;
			}

			if (fallingBlock == nullptr)
			{
				fallingBlock = DequeNextBlock();
				SetFallingBlock(fallingBlock);
			}

			// 키입력을 가져와 반영하자.
			while (inputQueue.empty() == false)
			{
				const auto& inputType = DequeInput();

				if (inputType == InputType::NONE)
				{
					continue;
				}
				else if (inputType == InputType::DOWN)
				{
					// UpdateFallingBlock 에서 시간을 0으로 맞추기 위함이다.
					lastFallingTime = -elapsedTime;
				}
				else if (inputType == InputType::HOLD && canHold == true)
				{
					if (hold == nullptr)
					{
						hold = fallingBlock;
						fallingBlock= DequeNextBlock();
						SetFallingBlock(fallingBlock);
					}
					else
					{
						std::swap(hold, fallingBlock);
					}

					hold = MakeFallingBlock(hold->blockShape);
					hold->pos.x = -3;
					hold->pos.y = 3;
					lastFallingTime = 0.f;
					
					canHold = false;
					applyQueue.push(inputType);
					continue;
				}
				MoveFallingBlock(inputType);
				applyQueue.push(inputType);
			}

			// 키들을 넘겨서 떨어지고 있는 블록을 업데이트하자.
			UpdateFallingBlock(elapsedTime);
			UpdateExpectBlock(elapsedTime);

			// 공격 정보로 쓸 수 있게 하자
			int outRemovedWallCnt = 0;

			RemoveWall(outRemovedWallCnt);
		}

		// 위에서부터 한 줄 꽉 채운 벽돌들을 제거한다.
		void RemoveWall(int& outRemovedWallCnt)
		{
			outRemovedWallCnt = 0;

			for (int height = 0; height < TETRIS_BOARD_HEIGHT; ++height)
			{
				int wallCount = 0;
				for (int width = 0; width < TETRIS_BOARD_WIDTH; ++width)
				{
					if (board[height][width].blockType == BlockType::WALL)
					{
						++wallCount;
					}
				}

				// 한 줄 전체가 WALL이라면 위의 WALL들을 내려준다.
				if (wallCount == TETRIS_BOARD_WIDTH)
				{
					DeleteLine(height);

					for (int moveHeigth = height - 1; moveHeigth >= 0; --moveHeigth)
					{
						if (IsAnyBlock(moveHeigth) == false)
						{
							break;
						}

						MoveToDown(moveHeigth);
					}
				}

			}
		}

		bool IsAnyBlock(int height)
		{
			for (int width = 0; width < TETRIS_BOARD_WIDTH; ++width)
			{
				if (board[height][width].blockType == BlockType::WALL)
				{
					return true;
				}
			}

			return false;
		}

		void DeleteLine(int height)
		{
			for (int width = 0; width < TETRIS_BOARD_WIDTH; ++width)
			{
				board[height][width].resourceID = ResourceID::EMPTY;
				board[height][width].blockType = BlockType::NONE;
			}
		}

		// 지정한 줄의 블록들을 한 줄 내린다.
		void MoveToDown(int height)
		{
			// 대상이 된 줄과 한 줄 아래가 유효한지 확인
			if (IsValidHeight(height) == false && IsValidHeight(height + 1) == false)
			{
				return;
			}

			for (int width = 0; width < TETRIS_BOARD_WIDTH; ++width)
			{
				board[height + 1][width].SetProperty(board[height][width]);
			}
			DeleteLine(height);
		}

		bool IsValidHeight(int height)
		{
			return 0 <= height && height < TETRIS_BOARD_HEIGHT;
		}

		void UpdateFallingBlock(float elapsedTime)
		{
			if (fallingBlock == nullptr)
			{
				fallingBlock = DequeNextBlock();
				SetFallingBlock(fallingBlock);
			}

			if (fallingBlock == nullptr)
			{
				return;
			}

			lastFallingTime += elapsedTime;
			if (lastFallingTime > FALLING_BLOCK_INTERVAL)
			{
				lastFallingTime = 0;
				// 블록 아래에 걸리는게 있는지 확인한다.
				if (CanMoveFallingBlock(Vector2D(0, 1)) == true)
				{
					MoveFallingBlock(InputType::DOWN);
					applyQueue.push(InputType::DOWN);
				}
				else
				{
					FallingBlockToWall();
					applyQueue.push(InputType::DROP);
				}
			}
		}

		void UpdateExpectBlock(float elapsedTime)
		{
			if (fallingBlock == nullptr)
			{
				return;
			}

			expectBlock = *fallingBlock;

			Vector2D vector(0, 0);
			while (CanMoveFallingBlock(vector))
			{
				vector.y += 1;
			}
			vector.y -= 1;

			expectBlock.pos += vector;
			expectBlock.SetBlockType(BlockType::NONE);
			expectBlock.SetResourceID(ResourceID::EXPECT);
		}

		bool CanMoveFallingBlock(Vector2D vector)
		{
			const auto& rr = fallingBlock->pos;
			for (int i = 0; i < 4; ++i)
			{
				const auto& dd = fallingBlock->blockInfo[i];
				int x = rr.x + dd.pos.x + vector.x;
				int y = rr.y + dd.pos.y + vector.y;

				if (y >= TETRIS_BOARD_HEIGHT)
				{
					return false;
				}
				else if (0 > x || x >= TETRIS_BOARD_WIDTH)
				{
					return false;
				}
				else if (y >= 0 && board[y][x].blockType == BlockType::WALL)
				{
					return false;
				}
			}

			return true;
		}

		void FallingBlockToWall()
		{
			for (int i = 0; i < 4; ++i)
			{
				int x = expectBlock.pos.x + expectBlock.blockInfo[i].pos.x;
				int y = expectBlock.pos.y + expectBlock.blockInfo[i].pos.y;
				board[y][x] = fallingBlock->blockInfo[i];
				board[y][x].pos.y = y;
				board[y][x].pos.x = x;
				board[y][x].blockType = BlockType::WALL;
			}
			SAFE_DELETE(fallingBlock);
			canHold = true;
		}

		void QueueInput(std::queue<InputType> inputType)
		{
			while (inputType.empty() != true)
			{
				inputQueue.push(inputType.front());
				//applyQueue.push(inputType.front());
				inputType.pop();
			}
		}

		void QueueInput(InputType inputType)
		{
			inputQueue.push(inputType);
		}

		InputType DequeInput()
		{
			if (inputQueue.empty() == true)
			{
				return InputType::NONE;
			}
			InputType deque = inputQueue.front();
			inputQueue.pop();
			return deque;
		}

		void QueueNextBlock(IOCPTetris::ShapeType type)
		{
			FallingBlock* newFallingBlock = nullptr;

			newFallingBlock = MakeFallingBlock(type);

			if (newFallingBlock != nullptr)
			{
				nextFallingBlockQueue.push_back(newFallingBlock);
			}
		}

		FallingBlock* MakeFallingBlock(ShapeType type)
		{
			FallingBlock* result = nullptr;
			switch (type)
			{
				case ShapeType::Z:
					result = new FallingBlock(ShapeType::Z, BlockShape::Z);
					break;
				case ShapeType::L:
					result = new FallingBlock(ShapeType::L, BlockShape::L);
					break;
				case ShapeType::O:
					result = new FallingBlock(ShapeType::O, BlockShape::O);
					break;
				case ShapeType::S:
					result = new FallingBlock(ShapeType::S, BlockShape::S);
					break;
				case ShapeType::I:
					result = new FallingBlock(ShapeType::I, BlockShape::I);
					break;
				case ShapeType::J:
					result = new FallingBlock(ShapeType::J, BlockShape::J);
					break;
				case ShapeType::T:
					result = new FallingBlock(ShapeType::T, BlockShape::T);
					break;
			}

			return result;
		}

		FallingBlock* DequeNextBlock()
		{
			if (nextFallingBlockQueue.empty() == true)
			{
				return nullptr;
			}

			FallingBlock* deque = nextFallingBlockQueue.front();
			nextFallingBlockQueue.pop_front();

			return deque;
		}

		std::vector<BlockInfo> GetRenderInfo()
		{
			std::vector<BlockInfo> result;

			for (int height = 0; height < TETRIS_BOARD_HEIGHT; ++height)
			{
				for (int width = 0;  width< TETRIS_BOARD_WIDTH; ++width)
				{
					result.push_back(board[height][width]);
				}
			}

			// 보드판에다가 내려오고 있는 블럭을 덮어씌어준다
			if (fallingBlock != nullptr)
			{
				for (int i = 0; i < 4; ++i)
				{
					auto copyBlock = expectBlock.blockInfo[i];
					copyBlock.pos.x += expectBlock.pos.x;
					copyBlock.pos.y += expectBlock.pos.y;
					result.push_back(copyBlock);

					copyBlock = fallingBlock->blockInfo[i];
					copyBlock.pos.x += fallingBlock->pos.x;
					copyBlock.pos.y += fallingBlock->pos.y;
					result.push_back(copyBlock);
				}
			}

			// 보드판에다가 내려오고 있는 블럭을 덮어씌어준다
			if (hold != nullptr)
			{
				auto holdBlockPos = hold->pos;

				for (int i = 0; i < 4; ++i)
				{
					auto copyBlock = hold->blockInfo[i];
					copyBlock.pos.x += holdBlockPos.x;
					copyBlock.pos.y += holdBlockPos.y;
					result.push_back(copyBlock);
				}
			}

			// 보드판에다가 내려오고 있는 블럭을 덮어씌어준다
			if (nextFallingBlockQueue.size() > 5)
			{
				for (int i = 0; i < 5; ++i)
				{
					auto nextBlockInfo = nextFallingBlockQueue.at(i)->blockInfo;

					for (int j = 0; j < 4; ++j)
					{
						auto copyBlock = nextBlockInfo[j];
						copyBlock.pos.x += 12;
						copyBlock.pos.y += i * 3 + 3;
						result.push_back(copyBlock);
					}
				}
			}
			
			return result;
		}

		size_t GetRemainFallingBlock()
		{
			return nextFallingBlockQueue.size();
		}

		std::queue<InputType>& GetAppliedInputs()
		{
			return applyQueue;
		}

		bool IsPlaying()
		{
			return isPlaying;
		}

	private:

		void MoveFallingBlock(InputType inputType)
		{
			Vector2D vector(0,0);

			switch (inputType)
			{
				case InputType::DOWN:
					vector.y = 1;
					break;
				case InputType::LEFT:
					vector.x = -1;
					break;
				case InputType::RIGHT:
					vector.x = 1;
					break;
				case InputType::DROP:
					FallingBlockToWall();
					SetFallingBlock(DequeNextBlock());
					return;
			}

			// 블럭 모양이 O면 회전은 못한다.
			if (fallingBlock->blockShape != ShapeType::O)
			{
				switch (inputType)
				{
					case InputType::ROTATELEFT:
						for (int i = 0; i < 4; ++i)
						{
							int curX = fallingBlock->blockInfo[i].pos.x;
							int curY = fallingBlock->blockInfo[i].pos.y;
							fallingBlock->blockInfo[i].pos.x = -roundf(curX * cosf(PI * 0.5f) - curY * sinf(PI * 0.5f));
							fallingBlock->blockInfo[i].pos.y = -roundf(curX * sinf(PI * 0.5f) + curY * cosf(PI * 0.5f));
						}
						return;
					case InputType::UP:
					case InputType::ROTATERIGHT:
						for (int i = 0; i < 4; ++i)
						{
							int curX = fallingBlock->blockInfo[i].pos.x;
							int curY = fallingBlock->blockInfo[i].pos.y;
							fallingBlock->blockInfo[i].pos.x = roundf(curX * cosf(PI * 0.5f) - curY * sinf(PI * 0.5f));
							fallingBlock->blockInfo[i].pos.y = roundf(curX * sinf(PI * 0.5f) + curY * cosf(PI * 0.5f));
						}
						return;
				}
			}

			if (CanMoveFallingBlock(vector))
			{
				fallingBlock->pos += vector;
			}
		}

		void SetFallingBlock(FallingBlock* _fallingBlock)
		{
			if (_fallingBlock == nullptr)
			{
				return;
			}

			if (fallingBlock != _fallingBlock)
			{
				SAFE_DELETE(fallingBlock);
			}

			fallingBlock = _fallingBlock;
			fallingBlock->SetPos(4, -1);

			lastFallingTime = 0.f;
		}

		BlockInfo board[TETRIS_BOARD_HEIGHT][TETRIS_BOARD_WIDTH];
		FallingBlock* hold = nullptr;
		FallingBlock* fallingBlock = nullptr;
		FallingBlock expectBlock;
		std::deque<FallingBlock*> nextFallingBlockQueue;
		std::queue<InputType> inputQueue;
		std::queue<InputType> applyQueue;
		bool canHold = false, isPlaying = false;
		float lastFallingTime = 0.f;
		int playerSessionIndex = 0;
	};

}