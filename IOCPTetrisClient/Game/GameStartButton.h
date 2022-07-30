#pragma once

#include "Button.h"
#include "ButtonText.h"

class GameStartButton : public Button
{
public:
	GameStartButton(int width, int height)
		: Button(width, height)
	{
		buttonText = std::make_shared<ButtonText>(0, 0, L"Start");
		AddButtonText(buttonText);
	}
	virtual ~GameStartButton() = default;

	// Button��(��) ���� ��ӵ�
	virtual void OnPressed() override;

private:

	std::shared_ptr<ButtonText> buttonText;
};