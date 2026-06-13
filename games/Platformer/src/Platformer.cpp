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
    {
        player->SetWorld(world);

        if (m_hasSpawnOverride)
        {
            Engine::Vec2 spawnPos = (m_spawnType == SpawnType::Entry)
                ? world->FindEntrySpawn(m_spawnRow)
                : world->FindReturnSpawn(m_spawnRow);
            player->SetGridPosition(spawnPos);
            m_hasSpawnOverride = false;
        }
        else
        {
            player->SetGridPosition(world->FindDefaultSpawn());
        }
    }

    if (player)
    {
        m_nextLevelSub = player->OnNextLevel().Subscribe([this](const int &row)
        {
            m_currentLevel++;
            if (m_currentLevel < static_cast<int>(Levels::ALL.size()))
            {
                m_hasSpawnOverride = true;
                m_spawnType = SpawnType::Entry;
                m_spawnRow = row;
                ReloadScene();
            }
            else
                Close();
        });
        m_previousLevelSub = player->OnPreviousLevel().Subscribe([this](const int &row)
        {
            m_currentLevel--;
            if (m_currentLevel >= 0)
            {
                m_hasSpawnOverride = true;
                m_spawnType = SpawnType::Return;
                m_spawnRow = row;
                ReloadScene();
            }
            else
                m_currentLevel = 0;
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
    m_nextLevelSub.Unsubscribe();
    m_previousLevelSub.Unsubscribe();
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
