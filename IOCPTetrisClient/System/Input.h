#pragma once

#include "pch.h"
#include <queue>

using namespace IOCPTetris;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Input
{
public:
	Input(HWND _hWnd)
		:hWnd(_hWnd)
	{

	}
	void Init()
	{
		m_keyboard = std::make_unique<Keyboard>();
		m_mouse = std::make_unique<Mouse>();
		m_mouse->SetWindow(hWnd);
		m_mouse->SetMode(Mouse::Mode::MODE_ABSOLUTE);
	}

	std::queue<InputType> GetInput()
	{
		std::queue<InputType> result;
		auto kb = m_keyboard->GetState();
		auto mouse = m_mouse->GetState();

		m_keys.Update(kb);

		if (m_keys.pressed.Down)
		{
			lastKey = Keyboard::Keys::Down;
			result.push(InputType::DOWN);
		}
		else if (m_keys.pressed.Left)
		{
			lastKey = Keyboard::Keys::Left;
			result.push(InputType::LEFT);
		}
		else if (m_keys.pressed.Right)
		{
			lastKey = Keyboard::Keys::Right;
			result.push(InputType::RIGHT);
		}
		else if (m_keys.pressed.Z)
		{
			lastKey = Keyboard::Keys::Z;
			result.push(InputType::ROTATELEFT);
		}
		else if (m_keys.pressed.X)
		{
			lastKey = Keyboard::Keys::X;
			result.push(InputType::ROTATERIGHT);
		}
		else if (m_keys.pressed.Up)
		{
			lastKey = Keyboard::Keys::Up;
			result.push(InputType::ROTATERIGHT);
		}
		else if (m_keys.pressed.C)
		{
			lastKey = Keyboard::Keys::C;
			result.push(InputType::HOLD);
		}
		else if (m_keys.pressed.Space)
		{
			lastKey = Keyboard::Keys::Space;
			result.push(InputType::DROP);
		}

		/*if (lastKey != Keyboard::Keys::None)
		{
			static int pressed = 0;
			if (kb.IsKeyDown(lastKey))
			{
				return lastInputType;
			}
		}*/
		return result;
	}

	Vector2 GetMousePos()
	{
		auto mouse = m_mouse->GetState();
		return Vector2(mouse.x, mouse.y);
	}

	bool IsPressLeftButton()
	{
		auto mouse = m_mouse->GetState();
		m_mouseButtons.Update(mouse);

		if (m_mouseButtons.leftButton == Mouse::ButtonStateTracker::PRESSED)
		{
			return true;
		}

		return false;
	}

	void OnActivate()
	{
		m_keys.Reset();
		m_mouseButtons.Reset();
	}

	void OnResuming()
	{
		m_keys.Reset();
		m_mouseButtons.Reset();
	}

private:

	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	Keyboard::KeyboardStateTracker m_keys;
	Mouse::ButtonStateTracker m_mouseButtons;
	InputType lastInputType = InputType::NONE;
	Keyboard::Keys lastKey = Keyboard::Keys::None;
	HWND hWnd;
};

