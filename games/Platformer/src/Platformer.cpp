#include "Platformer.h"
#include "PlatformerData.h"
#include "PlatformTile.h"
#include <iostream>
#include "Player.h"

using namespace Engine;

void Platformer::Initialize()
{
    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / PlatformerConstants::GRID_WORLD_SIZE.x;

    m_grid = Grid(cellSize, PlatformerConstants::GRID_WORLD_SIZE);
    
    for (const auto &entityInfo : PlatformerData::ENTITY_DATA)
    {
        auto *entity = GetScene()->Instantiate(entityInfo);
        if (auto *gridEntity = dynamic_cast<GridEntity *>(entity))
        {
            gridEntity->SetGrid(&m_grid);
        }
    }
    
    m_exitSub = Keyboard::OnKeyPressed().Subscribe([this](const Key &key)
    {
        if (key == Key::Escape)
            Close();
    });
}

void Platformer::Update(float deltaTime)
{
}

void Platformer::Shutdown()
{
}

Engine::WindowConfig Platformer::GetWindowConfig() const
{
    Engine::WindowConfig config;
    config.title = "Platformer";
    config.width = 800;
    config.height = 600;
    config.fullscreen = false;
    config.resizable = true;
    return config;
}

namespace Engine
{
    Engine::Application *CreateApplication()
    {
        return new Platformer();
    }
}
