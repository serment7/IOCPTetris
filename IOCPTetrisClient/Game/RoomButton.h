#pragma once

#include "Button.h"
#include "ButtonText.h"

class RoomButton : public Button
{
public:
	RoomButton(int width, int height)
		: Button(width,height)
	{
		roomInfo = std::make_shared<ButtonText>(0, 0, L"");
		AddButtonText(roomInfo);
	}
	virtual ~RoomButton() = default;

	void SetRoomInfo(int _roomIndex, int _playerNumber, int _maxPlayerNumber)
	{
		roomIndex = _roomIndex;
		playerNumber = _playerNumber;
		maxPlayerNumber = _maxPlayerNumber;

		std::wstring newText;
		if (roomIndex == 0)
		{
			newText = L"EMPTY";
		}
		else
		{
			newText = std::to_wstring(playerNumber) + L"/" + std::to_wstring(maxPlayerNumber);
		}

		roomInfo->SetText(std::move(newText));
	}
	// Button을(를) 통해 상속됨
	virtual void OnPressed() override;

private:
	int roomIndex = 0;
	int playerNumber = 0;
	int maxPlayerNumber = 0;

	std::shared_ptr<ButtonText> roomInfo;
};