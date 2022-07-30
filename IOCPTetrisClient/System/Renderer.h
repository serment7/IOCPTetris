#pragma once

#include "pch.h"
#include "ResourceLoader.h"
#include "DeviceResources.h"
#include "Game/Button.h"
#include <vector>
#include <iostream>
#include <cmath>

using namespace DirectX;

class Renderer
{
public:
	Renderer(ID3D11Device1* device, ID3D11DeviceContext* context);

	void Render(std::vector<IOCPTetris::BlockInfo> objects, ResourceLoader* resourceManager, IOCPTetris::Vector2D _pos);
	void Render(std::vector<Button*> objects);
	void Render(Button* object);


	void Draw(IOCPTetris::Vector2D _pos, ResourceObject* resourceObject);

	DirectX::SimpleMath::Vector2 PositionToVector2(IOCPTetris::Vector2D _pos);

	void Draw(DirectX::SimpleMath::Vector2 pos, ResourceObject* resourceObject);

	void DrawText(std::vector<ButtonText> buttonTexts);
	void DrawText(DirectX::SimpleMath::Vector2 pos, const wchar_t* text);
	void DrawRect(RECT rect);

	void Reset();

	void Begin(const std::shared_ptr<DX::DeviceResources>& deviceResources);
	void End();
	DirectX::SimpleMath::Vector3 LerpLine(long x, long y)
	{
		DirectX::SimpleMath::Vector3 result;

		result.x = Lerp(-1.f, 1.f, x / (float)DEFAULT_WIDTH_SIZE);
		result.y = Lerp(1.f, -1.f, y / (float)DEFAULT_HEIGHT_SIZE);
		
		return result;
	}

	DirectX::SimpleMath::Vector2 LerpPos(long x, long y)
	{
		DirectX::SimpleMath::Vector2 result;

		result.x = Lerp(0.f, x, scale.x);
		result.y = Lerp(0.f, y, scale.y);

		return result;
	}

private:
	float Lerp(float A, float B, float Alpha)
	{
		return A * (1 - Alpha) + B * Alpha;
	}

	ID3D11DeviceContext* m_context;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	XMFLOAT2 scale = XMFLOAT2(1,1);
	XMFLOAT3 center = XMFLOAT3(0,0,0);
	int captionSize = 0;
};