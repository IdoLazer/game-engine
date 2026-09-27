#include "Platformer.h"
#include "PlatformerConstants.h"
#include "PlatformerWorld.h"
#include "Player.h"
#include "PlatformerInputManager.h"
#include "Levels/LevelSet.h"
#include "Cursor.h"
#include "Falcon.h"

using namespace Engine;

namespace
{
    constexpr const char *PLATFORMER_SCENE = "assets/scenes/platformer.scene";
}

void Platformer::Initialize()
{
    SceneData *rootScene = ResourceManager::Load<SceneData>(PLATFORMER_SCENE, CacheMode::Refresh);
    if (!rootScene || rootScene->IsEmpty())
        throw std::runtime_error(std::string("Failed to load ") + PLATFORMER_SCENE);

    m_levelPaths = FindLevelPaths(*rootScene);
    if (m_levelPaths.empty())
        throw std::runtime_error(std::string("No LevelSet levels in ") + PLATFORMER_SCENE);

    // The level list is re-read on every reload, so a level deleted from it can leave the index
    // past the end.
    if (m_currentLevel >= static_cast<int>(m_levelPaths.size()))
        m_currentLevel = static_cast<int>(m_levelPaths.size()) - 1;

    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / PlatformerConstants::GRID_WORLD_SIZE.x;
    m_grid = Grid(cellSize, PlatformerConstants::GRID_WORLD_SIZE);

    SceneData *levelScene = ResourceManager::Load<SceneData>(m_levelPaths[m_currentLevel], CacheMode::Refresh);
    if (!levelScene || levelScene->IsEmpty())
        throw std::runtime_error("Failed to load level " + m_levelPaths[m_currentLevel]);

    // Level document first: its background and tiles must render behind the shared entities.
    for (const SceneData *scene : {levelScene, rootScene})
        for (const Scene::EntityInfo &entityInfo : scene->GetEntities())
            GetScene()->Instantiate(entityInfo);

    auto *world = GetScene()->GetFirstEntityOfType<PlatformerWorld>();
    auto *player = GetScene()->GetFirstEntityOfType<Player>();
    auto *falcon = GetScene()->GetFirstEntityOfType<Falcon>();

    if (!world || !player || !falcon)
        throw std::runtime_error("Failed to instantiate required entities (PlatformerWorld, Player, Falcon)");

    for (GridEntity *gridEntity : GetScene()->GetAllEntitiesOfType<GridEntity>())
        gridEntity->SetGrid(&m_grid);

    world->SetCoordSystem(&m_grid.GetCoordinateSystem());

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
    m_glideSub    = m_inputManager->OnGlide().Subscribe(player, &Player::Glide);
    m_stopGlideSub = m_inputManager->OnStopGlide().Subscribe(player, &Player::StopGlide);

    m_aimSub      = m_inputManager->OnAim().Subscribe(falcon, &Falcon::StartAiming);
    m_releaseSub  = m_inputManager->OnRelease().Subscribe(falcon, &Falcon::ReleaseAiming);
    m_retrieveSub = m_inputManager->OnRetrieve().Subscribe(falcon, &Falcon::Retrieve);

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
    m_retrieveSub.Unsubscribe();
    m_glideSub.Unsubscribe();
    m_stopGlideSub.Unsubscribe();
}

// --- Level Navigation ---

void Platformer::GoToNextLevel(int row)
{
    m_currentLevel++;
    if (m_currentLevel < static_cast<int>(m_levelPaths.size()))
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
