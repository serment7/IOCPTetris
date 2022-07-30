#pragma once

#include <WinSock2.h>
#include <unordered_map>

const float PI = 3.141529f;
const int BLOCKSIZE = 48;
#define BLOCK(x) x*BLOCKSIZE
#define SAFE_DELETE(x) if(x != nullptr) delete x; x = nullptr;
const int DEFAULT_WIDTH_SIZE = 1920;
const int DEFAULT_HEIGHT_SIZE = 1080;

namespace IOCPTetris
{
	const int TETRIS_BOARD_WIDTH = 10;
	const int TETRIS_BOARD_HEIGHT = 20;
	const float FALLING_BLOCK_INTERVAL = 1.0f;
	const float INPUT_COOLTIME = 0.03f;
	static const wchar_t* RESOURCE_ROOT_PATH = L"Resource";

	enum class ResourceID : short
	{
		NONE,
		BLOCK_RED,
		BLOCK_ORANGE,
		BLOCK_YELLOW,
		BLOCK_GREEN,
		BLOCK_MINT,
		BLOCK_PURPLE,
		BLOCK_PINK,
		BLOCK_WALL,
		GRID,
		EMPTY,
		EXPECT,
		END,
	};

	class ResourceInfo
	{
	public:
		ResourceInfo(RECT _rect, const wchar_t* _path)
			: rect(_rect), path(_path)
		{

		}
		RECT rect;
		const wchar_t* path = nullptr;
	};

	static const std::unordered_map<ResourceID, ResourceInfo> RESOURCE_DIC = {
		//left, top, rigth, bottom
		{ResourceID::BLOCK_RED, ResourceInfo(RECT({BLOCK(0),BLOCK(0),BLOCK(1),BLOCK(1) }), L"tetrio.png")},
		{ResourceID::BLOCK_ORANGE, ResourceInfo(RECT({BLOCK(1),BLOCK(0),BLOCK(2),BLOCK(1)  }), L"tetrio.png")},
		{ResourceID::BLOCK_YELLOW, ResourceInfo(RECT({BLOCK(2),BLOCK(0),BLOCK(3),BLOCK(1) }), L"tetrio.png")},
		{ResourceID::BLOCK_GREEN, ResourceInfo(RECT({BLOCK(3),BLOCK(0),BLOCK(4),BLOCK(1) }), L"tetrio.png")},
		{ResourceID::BLOCK_MINT, ResourceInfo(RECT({BLOCK(4),BLOCK(0),BLOCK(5),BLOCK(1) }), L"tetrio.png")},
		{ResourceID::BLOCK_PURPLE, ResourceInfo(RECT({BLOCK(0),BLOCK(1),BLOCK(1),BLOCK(2) }), L"tetrio.png")},
		{ResourceID::BLOCK_PINK, ResourceInfo(RECT({BLOCK(1),BLOCK(1),BLOCK(2),BLOCK(2) }), L"tetrio.png")},
		{ResourceID::BLOCK_WALL, ResourceInfo(RECT({BLOCK(3),BLOCK(1),BLOCK(4),BLOCK(2) }), L"tetrio.png")},
		{ResourceID::EMPTY, ResourceInfo(RECT({BLOCK(2),BLOCK(1),BLOCK(3),BLOCK(2) }), L"tetrio.png")},
		{ResourceID::EXPECT, ResourceInfo(RECT({BLOCK(0),BLOCK(0),BLOCK(1),BLOCK(1) }), L"alpha.png")},
		{ResourceID::GRID, ResourceInfo(RECT({0, 0,1024,1024 }), L"grid.png")},
	};

	enum class InputType : short
	{
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		HOLD,
		ROTATERIGHT,
		ROTATELEFT,
		DROP,
	};

	enum class BlockType : short
	{
		NONE,
		FALLING,
		WALL,
	};

	enum class ShapeType : short
	{
		NONE,
		Z,
		L,
		O,
		S,
		I,
		J,
		T,
	};

	class Vector2D
	{
	public:
		Vector2D(int _x = 0, int _y = 0)
			:x(_x), y(_y)
		{

		}

		Vector2D operator+(const Vector2D& _vector2D)
		{
			return Vector2D(x + _vector2D.x, y + _vector2D.y);
		}

		Vector2D operator+=(const Vector2D& _vector2D)
		{
			x += _vector2D.x;
			y += _vector2D.y;
			return Vector2D(x, y);
		}

		int x = 0, y = 0;
	};

	class BlockInfo
	{
	public:
		BlockInfo(ResourceID _resourceID = ResourceID::EMPTY, BlockType _blockType = BlockType::NONE, int _x = 0, int _y = 0)
			: resourceID(_resourceID), blockType(_blockType), pos(_x, _y)
		{

		}

		BlockInfo(const BlockInfo& _copy)
			:resourceID(_copy.resourceID), blockType(_copy.blockType), pos(_copy.pos)
		{
			
		}

		void SetProperty(const BlockInfo& _copy)
		{
			resourceID = _copy.resourceID;
			blockType = _copy.blockType;
		}

		void SetPos(int x, int y)
		{
			pos.x = x;
			pos.y = y;
		}

		Vector2D pos;
		float rotate = 0.0f;
		// 색깔로 구분하기 위한 데이터
		ResourceID resourceID = ResourceID::EMPTY;
		BlockType blockType = BlockType::NONE;
	};
	namespace BlockShape
	{
		static const BlockInfo Z[4] = {
		   BlockInfo(ResourceID::BLOCK_RED, BlockType::FALLING, -1, -1),
		   BlockInfo(ResourceID::BLOCK_RED, BlockType::FALLING, 0, -1),
		   BlockInfo(ResourceID::BLOCK_RED, BlockType::FALLING, 0, 0),
		   BlockInfo(ResourceID::BLOCK_RED, BlockType::FALLING, 1, 0)
		};

		static const BlockInfo L[4] = {
		   BlockInfo(ResourceID::BLOCK_ORANGE, BlockType::FALLING, -1, 0),
		   BlockInfo(ResourceID::BLOCK_ORANGE, BlockType::FALLING, 0, 0),
		   BlockInfo(ResourceID::BLOCK_ORANGE, BlockType::FALLING, 1, 0),
		   BlockInfo(ResourceID::BLOCK_ORANGE, BlockType::FALLING, 1, -1)
		};

		static const BlockInfo O[4] = {
		   BlockInfo(ResourceID::BLOCK_YELLOW, BlockType::FALLING, 0, 0),
		   BlockInfo(ResourceID::BLOCK_YELLOW, BlockType::FALLING, 1, 0),
		   BlockInfo(ResourceID::BLOCK_YELLOW, BlockType::FALLING, 0, -1),
		   BlockInfo(ResourceID::BLOCK_YELLOW, BlockType::FALLING, 1, -1)
		};

		static const BlockInfo S[4] = {
		   BlockInfo(ResourceID::BLOCK_GREEN, BlockType::FALLING, -1, 0),
		   BlockInfo(ResourceID::BLOCK_GREEN, BlockType::FALLING, 0, 0),
		   BlockInfo(ResourceID::BLOCK_GREEN, BlockType::FALLING, 0, -1),
		   BlockInfo(ResourceID::BLOCK_GREEN, BlockType::FALLING, 1, -1)
		};

		static const BlockInfo I[4] = {
		   BlockInfo(ResourceID::BLOCK_MINT, BlockType::FALLING, -1, 0),
		   BlockInfo(ResourceID::BLOCK_MINT, BlockType::FALLING, 0, 0),
		   BlockInfo(ResourceID::BLOCK_MINT, BlockType::FALLING, 1, 0),
		   BlockInfo(ResourceID::BLOCK_MINT, BlockType::FALLING, 2, 0)
		};

		static const BlockInfo J[4] = {
		   BlockInfo(ResourceID::BLOCK_PURPLE, BlockType::FALLING, -1, -1),
		   BlockInfo(ResourceID::BLOCK_PURPLE, BlockType::FALLING, -1, 0),
		   BlockInfo(ResourceID::BLOCK_PURPLE, BlockType::FALLING, 0, 0),
		   BlockInfo(ResourceID::BLOCK_PURPLE, BlockType::FALLING, 1, 0)
		};

		static const BlockInfo T[4] = {
		   BlockInfo(ResourceID::BLOCK_PINK, BlockType::FALLING, -1, 0),
		   BlockInfo(ResourceID::BLOCK_PINK, BlockType::FALLING, 0, 0),
		   BlockInfo(ResourceID::BLOCK_PINK, BlockType::FALLING, 1, 0),
		   BlockInfo(ResourceID::BLOCK_PINK, BlockType::FALLING, 0, -1)
		};
	}

	class FallingBlock
	{
	public:
		FallingBlock() = default;

		FallingBlock(ShapeType _shapeType, const BlockInfo* _blockInfo)
		{
			blockShape = _shapeType;
			blockInfo[0] = _blockInfo[0];
			blockInfo[1] = _blockInfo[1];
			blockInfo[2] = _blockInfo[2];
			blockInfo[3] = _blockInfo[3];
		}

		FallingBlock(const FallingBlock& _copy)
		{
			blockShape = _copy.blockShape;
			pos = _copy.pos;
			blockInfo[0] = _copy.blockInfo[0];
			blockInfo[1] = _copy.blockInfo[1];
			blockInfo[2] = _copy.blockInfo[2];
			blockInfo[3] = _copy.blockInfo[3];
		}

		FallingBlock operator=(const FallingBlock& _copy)
		{
			pos = _copy.pos;
			blockInfo[0] = _copy.blockInfo[0];
			blockInfo[1] = _copy.blockInfo[1];
			blockInfo[2] = _copy.blockInfo[2];
			blockInfo[3] = _copy.blockInfo[3];
			return FallingBlock(*this);
		}

		Vector2D pos = { 0,0 };
		BlockInfo blockInfo[4];
		ShapeType blockShape = ShapeType::NONE;

		void SetPos(int _x, int _y)
		{
			pos.x = _x;
			pos.y = _y;
		}

		void SetBlockType(BlockType blockType)
		{
			blockInfo[0].blockType = blockType;
			blockInfo[1].blockType = blockType;
			blockInfo[2].blockType = blockType;
			blockInfo[3].blockType = blockType;
		}

		void SetResourceID(ResourceID resourceID)
		{
			blockInfo[0].resourceID = resourceID;
			blockInfo[1].resourceID = resourceID;
			blockInfo[2].resourceID = resourceID;
			blockInfo[3].resourceID = resourceID;
		}
	};

	struct TetrisGame
	{
		char field[20][10];
		bool hold;
		int blockgage = 10;
	};
}
