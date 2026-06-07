#include "Platformer.h"
#include "PlatformerData.h"
#include "PlatformerWorld.h"
#include "Player.h"
#include "Levels/Levels.h"

using namespace Engine;

void Platformer::Initialize()
{
    const LevelData &level = *Levels::ALL[m_currentLevel];

    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / PlatformerConstants::GRID_WORLD_SIZE.x;
    m_grid = Grid(cellSize, PlatformerConstants::GRID_WORLD_SIZE);
    
    PlatformerWorld *world = nullptr;
    Player *player = nullptr;

    for (const auto &entityInfo : PlatformerData::ENTITY_DATA)
    {
        auto *entity = GetScene()->Instantiate(entityInfo);
        if (auto *gridEntity = dynamic_cast<GridEntity *>(entity))
        {
            gridEntity->SetGrid(&m_grid);
        }
        if (auto *w = dynamic_cast<PlatformerWorld *>(entity))
        {
            w->SetTileGrid(level.tileGrid);
            w->SetCoordSystem(&m_grid.GetCoordinateSystem());
            world = w;
        }
        if (auto *p = dynamic_cast<Player *>(entity))
            player = p;
    }

    if (player && world)
        player->SetWorld(world);

    if (player)
    {
        m_levelEndSub = player->OnLevelEnd().Subscribe([this]()
        {
            m_currentLevel++;
            if (m_currentLevel < static_cast<int>(Levels::ALL.size()))
                ReloadScene();
            else
                Close();
        });
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
    m_levelEndSub.Unsubscribe();
    m_exitSub.Unsubscribe();
}

Engine::WindowConfig Platformer::GetWindowConfig() const
{
    Engine::WindowConfig config;
    config.title = "Platformer";
    config.width = 1200;
    config.height = 800;
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
