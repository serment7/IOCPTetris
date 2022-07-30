#include "pch.h"
#include "LobbyScene.h"
#include "../System/Renderer.h"
#include "../System/Input.h"
#include "../System/Server.h"
#include "../../IOCPTetrisCommon/Packet.hh"
#include "Game.h"
#include <thread>
#include <chrono>
#include <cassert>
#include <memory>

using namespace IOCPTetris;
using namespace DirectX;

IOCPTetris::LobbyScene::LobbyScene()
    : BaseScene()
{
}

void IOCPTetris::LobbyScene::Start()
{
    {
        Packet::RES_LOGIN res;

        if (Game::Get()->GetPlayerSessionIndex() == 0)
        {
            Server::Get()->SendSync(Packet::PACKET_ID::REQ_LOGIN, Packet::REQ_LOGIN(), res);
            assert(res.result.resultID == ResultID::SUCCESS);
        }

    };

    {
        Packet::REQ_ROOM_LIST req;
        Packet::RES_ROOM_LIST res;

        req.roomPage = currentRoomPage;

        Server::Get()->SendSync(Packet::PACKET_ID::REQ_ROOM_LIST, req, res);
    }
}

void IOCPTetris::LobbyScene::Update(const float& elapsedTime)
{
    if (Game::Get()->input->IsPressLeftButton())
    {
        XMFLOAT2 mousePos = Game::Get()->input->GetMousePos();

        for (auto& r : m_buttons)
        {
            if (r->IsPressed(mousePos))
            {
                r->OnPressed();
            }
        }
    }

    static float updateRoomInfoInterval = 0.f;

    updateRoomInfoInterval += elapsedTime;

    if (updateRoomInfoInterval > 5.0f)
    {
        updateRoomInfoInterval = 0.f;

        Packet::REQ_ROOM_LIST req;

        req.roomPage = currentRoomPage;
        Server::Get()->Send(Packet::PACKET_ID::REQ_ROOM_LIST, req);
    }
}

void IOCPTetris::LobbyScene::Stop()
{
}

void IOCPTetris::LobbyScene::Render()
{
    for (auto& r : m_buttons)
    {
        Game::Get()->renderer->Render(r.get());
    }
}

void IOCPTetris::LobbyScene::SetupRoomPage(int roomPage, std::vector<Common::RoomInfo> roomInfos)
{
    currentRoomPage = roomPage;
    
    m_buttons.clear();

    for (int i = 0; i < Common::ROOM_CNT_PER_PAGE; ++i)
    {
        const auto& roomInfo = roomInfos[i];
        auto newButton = std::make_shared<RoomButton>(300, 150);
        newButton->SetRoomInfo(roomInfo.roomIndex, roomInfo.currentPlayerCnt, roomInfo.maxPlayerCnt);
        newButton->SetPos(100 + ((i % 2) ? 350 : 0), 100 + (int)(i * 0.5f) * 200);
        m_buttons.push_back(newButton);
    }
}
