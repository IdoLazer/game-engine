#include "Platformer.h"
#include "PlatformerData.h"
#include "PlatformerWorld.h"
#include "Player.h"
#include "PlatformerInputManager.h"
#include "Levels/Levels.h"
#include "Cursor.h"
#include "Falcon.h"

using namespace Engine;

void Platformer::Initialize()
{
    const LevelData &level = *Levels::ALL[m_currentLevel];

    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / PlatformerConstants::GRID_WORLD_SIZE.x;
    m_grid = Grid(cellSize, PlatformerConstants::GRID_WORLD_SIZE);

    PlatformerWorld *world = nullptr;
    Player *player = nullptr;
    Falcon *falcon = nullptr;

    for (const auto &entityInfo : PlatformerData::ENTITY_DATA)
    {
        auto *entity = GetScene()->Instantiate(entityInfo);
        if (auto *gridEntity = dynamic_cast<GridEntity *>(entity))
            gridEntity->SetGrid(&m_grid);
        if (auto *w = dynamic_cast<PlatformerWorld *>(entity))
        {
            w->SetTileGrid(level.tileGrid);
            w->SetCoordSystem(&m_grid.GetCoordinateSystem());
            world = w;
        }
        if (auto *p = dynamic_cast<Player *>(entity))
            player = p;
        if (auto *f = dynamic_cast<Falcon *>(entity))
            falcon = f;
    }

    if (!world || !player || !falcon)
        throw std::runtime_error("Failed to instantiate required entities (PlatformerWorld, Player, Falcon)");

    player->SetWorld(world);
    falcon->SetWorld(world);
    player->SetFalcon(falcon);
    
    Vec2 spawnPos = m_hasSpawnOverride
        ? (m_spawnType == SpawnType::Entry
            ? world->FindEntrySpawn(m_spawnRow)
            : world->FindReturnSpawn(m_spawnRow))
        : world->FindDefaultSpawn();
    m_hasSpawnOverride = false;

    if (world->IsSolid(Vec2(spawnPos.x, spawnPos.y + 1.0f)))
        spawnPos.y += 0.5f - player->GetGridSize().y / 2.0f;

    player->SetGridPosition(spawnPos);

    // Tile-collision-driven level transitions
    m_nextLevelSub     = player->OnNextLevel().Subscribe([this](const int &row)     { GoToNextLevel(row); });
    m_previousLevelSub = player->OnPreviousLevel().Subscribe([this](const int &row) { GoToPreviousLevel(row); });
    m_reloadLevelSub   = player->OnReloadLevel().Subscribe([this]()                 { ReloadCurrentLevel(); });

    // Input manager — owns all keyboard routing
    m_inputManager = std::make_unique<PlatformerInputManager>();

    m_moveSub     = m_inputManager->OnMove().Subscribe(player, &Player::SetDirection);
    m_jumpSub     = m_inputManager->OnJump().Subscribe(player, &Player::Jump);
    m_jumpStopSub = m_inputManager->OnJumpStop().Subscribe(player, &Player::StopJump);

    m_aimSub      = m_inputManager->OnAim().Subscribe(falcon, &Falcon::StartAiming);
    m_releaseSub  = m_inputManager->OnRelease().Subscribe(falcon, &Falcon::ReleaseAiming);

    m_debugNextLevelSub     = m_inputManager->OnNextLevel().Subscribe([this]()     { GoToNextLevel(-1); });
    m_debugPreviousLevelSub = m_inputManager->OnPreviousLevel().Subscribe([this]() { GoToPreviousLevel(-1); });
    m_debugReloadLevelSub   = m_inputManager->OnReloadLevel().Subscribe([this]()   { ReloadCurrentLevel(); });

    m_exitSub = Keyboard::OnKeyPressed().Subscribe([this](const Key &key)
    {
        if (key == Key::Escape)
            Close();
    });
    
    m_cursor = GetScene()->GetFirstEntityOfType<Cursor>();
    if (m_cursor)
    {
        Mouse::SetCursorVisibility(false);
        m_cursorMoveSub = m_inputManager->OnCursorMove().Subscribe(m_cursor, &Cursor::SetPosition);
    }

    falcon->SetCursor(m_cursor);

    m_inputManager->NotifyInitialState();
}

void Platformer::Update(float deltaTime)
{
}

void Platformer::Shutdown()
{
    m_nextLevelSub.Unsubscribe();
    m_previousLevelSub.Unsubscribe();
    m_reloadLevelSub.Unsubscribe();
    m_moveSub.Unsubscribe();
    m_jumpSub.Unsubscribe();
    m_jumpStopSub.Unsubscribe();
    m_debugNextLevelSub.Unsubscribe();
    m_debugPreviousLevelSub.Unsubscribe();
    m_debugReloadLevelSub.Unsubscribe();
    m_exitSub.Unsubscribe();
    m_cursorMoveSub.Unsubscribe();
    m_aimSub.Unsubscribe();
    m_releaseSub.Unsubscribe();
}

// --- Level Navigation ---

void Platformer::GoToNextLevel(int row)
{
    m_currentLevel++;
    if (m_currentLevel < static_cast<int>(Levels::ALL.size()))
    {
        if (row >= 0)
        {
            m_hasSpawnOverride = true;
            m_spawnType = SpawnType::Entry;
            m_spawnRow = row;
        }
        ReloadScene();
    }
    else
        Close();
}

void Platformer::GoToPreviousLevel(int row)
{
    m_currentLevel--;
    if (m_currentLevel >= 0)
    {
        if (row >= 0)
        {
            m_hasSpawnOverride = true;
            m_spawnType = SpawnType::Return;
            m_spawnRow = row;
        }
        ReloadScene();
    }
    else
        m_currentLevel = 0;
}

void Platformer::ReloadCurrentLevel()
{
    m_spawnType = SpawnType::Entry;
    ReloadScene();
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
