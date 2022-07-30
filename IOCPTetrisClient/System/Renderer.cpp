#include "pch.h"
#include "Renderer.h"

Renderer::Renderer(ID3D11Device1* device, ID3D11DeviceContext* context)
	:m_context(context)
	,m_spriteBatch(std::make_unique<SpriteBatch>(context))
	,m_effect(std::make_unique<BasicEffect>(device))
	,m_font(std::make_unique<SpriteFont>(device, L"Resource/korean.spritefont"))
	,m_batch(std::make_unique<PrimitiveBatch<VertexPositionColor>>(context))
	,m_states(std::make_unique<CommonStates>(device))
{
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(
		device->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			m_inputLayout.ReleaseAndGetAddressOf()));
}

void Renderer::Render(std::vector<IOCPTetris::BlockInfo> objects, ResourceLoader* resourceManager, IOCPTetris::Vector2D _pos)
{
	for (auto object : objects)
	{
		auto result = resourceManager->GetResource(object.resourceID);
		if (result != nullptr)
		{
			Draw(object.pos + _pos, result);
		}
	}
}

void Renderer::Render(std::vector<Button*> objects)
{
	for (auto& i : objects)
	{
		Render(i);
	}
}

void Renderer::Render(Button* object)
{
	if (object->GetEnable() == false)
	{
		return;
	}

	DrawRect(object->GetRect());

	auto buttonTexts = object->GetButtonTexts();
	for (auto& buttonText : buttonTexts)
	{
		DrawText(buttonText.GetPos(), buttonText.GetText());
	}
}

void Renderer::Draw(IOCPTetris::Vector2D _pos, ResourceObject* resourceObject)
{
	const auto& pos = PositionToVector2(_pos);
	m_spriteBatch->Draw(resourceObject->texture.Get(), LerpPos(pos.x, pos.y), &resourceObject->tile,
		Colors::White, 0.f, resourceObject->origin, scale, DirectX::SpriteEffects::SpriteEffects_FlipBoth);
}


DirectX::SimpleMath::Vector2 Renderer::PositionToVector2(IOCPTetris::Vector2D _pos)
{
	return DirectX::SimpleMath::Vector2(BLOCK(_pos.x), BLOCK(_pos.y));
}

void Renderer::Draw(DirectX::SimpleMath::Vector2 pos, ResourceObject* resourceObject)
{
	m_spriteBatch->Draw(resourceObject->texture.Get(), LerpPos(pos.x, pos.y), &resourceObject->tile,
		Colors::White, 0.f, resourceObject->origin, scale);
}

void Renderer::DrawText(std::vector<ButtonText> buttonTexts)
{
	for (auto& i : buttonTexts)
	{
		DrawText(i.GetPos(), i.GetText());
	}
}

void Renderer::DrawText(DirectX::SimpleMath::Vector2 pos, const wchar_t* text)
{
	DirectX::SimpleMath::Vector2 origin = m_font->MeasureString(text) * 0.5f;
	m_font->DrawString(m_spriteBatch.get(), text, LerpPos(pos.x, pos.y), Colors::Black, 0.f, origin, scale);
}

void Renderer::DrawRect(RECT rect)
{
	VertexPositionColor line1(LerpLine(rect.left, rect.top), Colors::White);
	VertexPositionColor line2(LerpLine(rect.right, rect.top), Colors::White);
	VertexPositionColor line3(LerpLine(rect.right, rect.bottom), Colors::White);
	VertexPositionColor line4(LerpLine(rect.left, rect.bottom), Colors::White);
	m_batch->DrawQuad(line1, line2, line3, line4);
}

void Renderer::Reset()
{
	m_effect.reset();
	m_font.reset();
	m_batch.reset();
	m_spriteBatch.reset();
}

void Renderer::Begin(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	auto size = deviceResources->GetOutputSize();

	scale.x = (size.right - size.left) / (float)DEFAULT_WIDTH_SIZE;
	scale.y = (size.bottom - size.top) / (float)DEFAULT_HEIGHT_SIZE;

	captionSize = GetSystemMetrics(SM_CYCAPTION);

	center.x = (size.right - size.left) * 0.5f;
	center.y = (size.bottom - size.top) * 0.5f + static_cast<float>(captionSize);

	m_context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	m_context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	m_context->RSSetState(m_states->CullNone());
	m_effect->Apply(m_context);
	m_context->IASetInputLayout(m_inputLayout.Get());

	m_spriteBatch->Begin();
	m_batch->Begin();
}

void Renderer::End()
{
	m_batch->End();
	m_spriteBatch->End();
}
