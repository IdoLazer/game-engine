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

    // Scene documents are edited while the game runs only in Debug, so only Debug re-reads them.
#ifdef NDEBUG
    constexpr CacheMode SCENE_CACHE_MODE = CacheMode::Reuse;
#else
    constexpr CacheMode SCENE_CACHE_MODE = CacheMode::Refresh;
#endif
}

Platformer::Platformer()
    : m_inputManager(std::make_unique<PlatformerInputManager>())
{
    m_inputSubscriptions.push_back(m_inputManager->OnNextLevel().Subscribe([this]()     { GoToNextLevel(-1); }));
    m_inputSubscriptions.push_back(m_inputManager->OnPreviousLevel().Subscribe([this]() { GoToPreviousLevel(-1); }));
    m_inputSubscriptions.push_back(m_inputManager->OnReloadLevel().Subscribe([this]()   { ReloadCurrentLevel(); }));
    m_inputSubscriptions.push_back(m_inputManager->OnQuit().Subscribe([this]()          { Close(); }));
}

void Platformer::Initialize()
{
    SceneData *rootScene = ResourceManager::Load<SceneData>(PLATFORMER_SCENE, SCENE_CACHE_MODE);
    if (!rootScene || rootScene->IsEmpty())
        throw std::runtime_error(std::string("Failed to load ") + PLATFORMER_SCENE);

    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / PlatformerConstants::GRID_WORLD_SIZE.x;
    m_grid = Grid(cellSize, PlatformerConstants::GRID_WORLD_SIZE);

    // Entities render in the order instantiated, so the level lands where the document lists LevelSet.
    for (const Scene::EntityInfo &entityInfo : rootScene->GetEntities())
    {
        Entity *entity = GetScene()->Instantiate(entityInfo);
        if (auto *levelSet = dynamic_cast<LevelSet *>(entity))
            InstantiateCurrentLevel(*levelSet);
    }

    auto *world = GetScene()->GetFirstEntityOfType<PlatformerWorld>();
    auto *player = GetScene()->GetFirstEntityOfType<Player>();
    auto *falcon = GetScene()->GetFirstEntityOfType<Falcon>();

    if (!world || !player || !falcon)
        throw std::runtime_error("Failed to instantiate required entities (PlatformerWorld, Player, Falcon)");

    m_world = world;
    m_player = player;

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

    m_cursor = GetScene()->GetFirstEntityOfType<Cursor>();
    if (m_cursor)
        Mouse::SetCursorVisibility(false);

    falcon->SetCursor(m_cursor);

    m_inputManager->Bind(*player, *falcon, m_cursor);
}

void Platformer::Update(float deltaTime)
{
    if (!m_player || !m_world)
        return;

    Vec2 cell = m_grid.GetCellFromGridPosition(m_player->GetGridPosition());
    int row = static_cast<int>(cell.y);

    if (m_world->IsNextLevel(cell))
        GoToNextLevel(row);
    else if (m_world->IsPreviousLevel(cell))
        GoToPreviousLevel(row);
    else if (m_world->IsDeadly(cell))
        ReloadCurrentLevel();
}

void Platformer::Shutdown()
{
    m_inputManager->Unbind();
    m_player = nullptr;
    m_world = nullptr;
}

// --- Level Loading ---

void Platformer::InstantiateCurrentLevel(const LevelSet &levelSet)
{
    m_levelPaths = levelSet.GetLevels();
    if (m_levelPaths.empty())
        throw std::runtime_error(std::string("No LevelSet levels in ") + PLATFORMER_SCENE);

    // A level deleted from the list between reloads can leave the index past the end.
    if (m_currentLevel >= static_cast<int>(m_levelPaths.size()))
        m_currentLevel = static_cast<int>(m_levelPaths.size()) - 1;

    const std::string &levelPath = m_levelPaths[m_currentLevel];
    SceneData *levelScene = ResourceManager::Load<SceneData>(levelPath, SCENE_CACHE_MODE);
    if (!levelScene || levelScene->IsEmpty())
        throw std::runtime_error("Failed to load level " + levelPath);

    for (const Scene::EntityInfo &entityInfo : levelScene->GetEntities())
        GetScene()->Instantiate(entityInfo);
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
