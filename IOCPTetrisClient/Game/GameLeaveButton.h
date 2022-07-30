#pragma once

#include "Button.h"
#include "ButtonText.h"

class GameLeaveButton : public Button
{
public:
	GameLeaveButton(int width, int height)
		: Button(width, height)
	{
		buttonText = std::make_shared<ButtonText>(0, 0, L"Leave");
		AddButtonText(buttonText);
	}
	virtual ~GameLeaveButton() = default;

	// Button을(를) 통해 상속됨
	virtual void OnPressed() override;

private:

	std::shared_ptr<ButtonText> buttonText;
};