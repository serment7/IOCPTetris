#pragma once
#include "pch.h"
#include <string>
#include "ButtonText.h"

using namespace IOCPTetris;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Button
{
public:
	Button(int width, int height)
	{
		collision.width = width;
		collision.height = height;
	}

	virtual ~Button()
	{
		buttonTexts.clear();
	}

	void SetPos(int x, int y)
	{
		collision.x = x;
		collision.y = y;
	}

	void AddButtonText(std::shared_ptr<ButtonText> button)
	{
		buttonTexts.push_back(button);
	}

	Vector2 GetPos()
	{
		return collision.Location();
	}

	std::vector<ButtonText> GetButtonTexts()
	{
		std::vector<ButtonText> result;
		auto center = collision.Center();

		for (auto& i : buttonTexts)
		{
			auto pos = i->GetPos();
			result.emplace_back(pos.x + center.x, pos.y + center.y, i->GetText());
		}

		return result;
	}

	RECT GetRect()
	{
		return collision;
	}

	bool IsPressed(Vector2 mousePos)
	{
		return isEnable && collision.Contains(mousePos);
	}

	void SetEnable(bool _isEnable)
	{
		isEnable = _isEnable;
	}

	bool GetEnable()
	{
		return isEnable;
	}

	virtual void OnPressed() = 0;

private:
	DirectX::SimpleMath::Rectangle collision;
	std::vector<std::shared_ptr<ButtonText>> buttonTexts;
	bool isEnable = true;
};
