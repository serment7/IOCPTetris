#include "..\IOCPTetrisServer\Game.h"
//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Scene/BaseScene.h"
#include "Scene/LobbyScene.h"
#include "System/Server.h"
#include<vector>
#include <typeinfo>
#include <iostream>

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace IOCPTetris;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	m_deviceResources = std::make_shared<DX::DeviceResources>();
	// TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
	//   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
	//   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
	if (curScene != nullptr)
	{
		curScene->Stop();
		SAFE_DELETE(curScene);
	}

	SAFE_DELETE(resourceLoader);
	SAFE_DELETE(renderer);
	SAFE_DELETE(input);

	Server::Get()->Shutdown();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	resourceLoader = new ResourceLoader(m_deviceResources->GetD3DDevice());
	resourceLoader->Init();
	renderer = new Renderer(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
	input = new Input(m_deviceResources->GetWindow());
	input->Init();

	Server::Get()->Init();
	Server::Get()->ConnectServer("127.0.0.1", 15001);

	curScene = new LobbyScene();
	curScene->Init();
	curScene->Start();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
	{
		Update(m_timer);
	});

	Render();
}

void Game::SetPlayerSessionIndex(int sessionIndex)
{
	playerSessionIndex = sessionIndex;
}

int Game::GetPlayerSessionIndex()
{
	return playerSessionIndex;
}

IOCPTetris::BaseScene* Game::GetPreservedScene()
{
	std::lock_guard<std::mutex> guard(sceneGuard);
	return preservedScene;
}

void Game::SetPreservedScene(IOCPTetris::BaseScene* scene)
{
	std::lock_guard<std::mutex> guard(sceneGuard);
	preservedScene = scene;
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.
	elapsedTime;

	Server::Get()->HandlerThread();

	if (curScene != nullptr)
	{
		curScene->Update(elapsedTime);

		auto nextScene = GetPreservedScene();
		SetPreservedScene(nullptr);

		if (nextScene != nullptr)
		{
			curScene->Reset();
			curScene->Stop();
			SAFE_DELETE(curScene);

			curScene = nextScene;
			curScene->Init();
			curScene->Start();
		}
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	float time = float(m_timer.GetTotalSeconds());

	// TODO: Add your rendering code here.
	context;
	
	renderer->Begin(m_deviceResources);

	if (curScene != nullptr)
	{
		curScene->Render();
	}

	renderer->End();


	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto const viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = DEFAULT_WIDTH_SIZE;
	height = DEFAULT_HEIGHT_SIZE;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();

	// TODO: Initialize device dependent objects here (independent of window size).
	device;

	auto context = m_deviceResources->GetD3DDeviceContext();

	//resourceManager = std::make_shared<ResourceManager>(device);
	//resourceManager->Init();
	//renderer = std::make_unique<Renderer>(context, resourceManager);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	// TODO: Initialize windows-size dependent objects here.
	auto size = m_deviceResources->GetOutputSize();
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
	curScene->Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
