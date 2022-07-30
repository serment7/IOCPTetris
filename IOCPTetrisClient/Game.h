//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "System/ResourceLoader.h"
#include "System/Renderer.h"
#include "System/Input.h"
#include <mutex>

namespace IOCPTetris
{
    class BaseScene;
}

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    static Game* Get()
    {
        static Game instance;
        return &instance;
    }

    ~Game();

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize(int& width, int& height) const noexcept;

    template<typename T>
    T* GetCurrentScene()
    {
        return dynamic_cast<T*>(curScene);
    }

    void SetPlayerSessionIndex(int sessionIndex);
    int GetPlayerSessionIndex();

    IOCPTetris::BaseScene* GetPreservedScene();
    void SetPreservedScene(IOCPTetris::BaseScene*);

    // Device resources.
    std::shared_ptr<DX::DeviceResources>    m_deviceResources;

    ResourceLoader* resourceLoader = nullptr;
    Renderer* renderer = nullptr;
    Input* input = nullptr;

private:
    Game() noexcept(false);
    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
    IOCPTetris::BaseScene* curScene = nullptr;
    IOCPTetris::BaseScene* preservedScene = nullptr;
    int playerSessionIndex;
    std::mutex sceneGuard;

};
