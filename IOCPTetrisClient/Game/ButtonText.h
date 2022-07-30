#pragma once

#include "pch.h"

using namespace DirectX::SimpleMath;

class ButtonText
{
public:
	ButtonText(int x, int y, const wchar_t* _text)
	{
		SetPos(x, y);
		SetText(_text);
	}

	Vector2 GetPos()
	{
		return pos;
	}

	void SetPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
	}

	const wchar_t* GetText()
	{
		return text.c_str();
	}

	void SetText(const wchar_t* _text)
	{
		text = _text;
	}

	void SetText(std::wstring&& _text)
	{
		text.swap(_text);
	}

private:
	Vector2 pos;
	std::wstring text;
};