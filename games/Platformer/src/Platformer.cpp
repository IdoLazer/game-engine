#include "Platformer.h"
#include "PlatformerWorld.h"
#include "Player.h"
#include "PlatformerInputManager.h"
#include "Levels/LevelSet.h"
#include "Levels/LevelExit.h"
#include "Cursor.h"
#include "LevelCamera.h"
#include "Falcon.h"

#include <algorithm>

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
    m_inputSubscriptions.push_back(m_inputManager->OnNextLevel().Subscribe([this]()     { GoToListedLevel(1); }));
    m_inputSubscriptions.push_back(m_inputManager->OnPreviousLevel().Subscribe([this]() { GoToListedLevel(-1); }));
    m_inputSubscriptions.push_back(m_inputManager->OnReloadLevel().Subscribe([this]()   { ReloadCurrentLevel(); }));
    m_inputSubscriptions.push_back(m_inputManager->OnQuit().Subscribe([this]()          { Close(); }));
}

void Platformer::Initialize()
{
    SceneData *rootScene = ResourceManager::Load<SceneData>(PLATFORMER_SCENE, SCENE_CACHE_MODE);
    if (!rootScene || rootScene->IsEmpty())
        throw std::runtime_error(std::string("Failed to load ") + PLATFORMER_SCENE);

    // Entities render in the order instantiated, so the level lands where the document lists LevelSet.
    for (const Scene::EntityInfo &entityInfo : rootScene->GetEntities())
    {
        Entity *entity = GetScene()->Instantiate(entityInfo);
        if (auto *levelSet = dynamic_cast<LevelSet *>(entity))
            InstantiateCurrentLevel(*levelSet);
    }

    m_world = GetScene()->GetFirstEntityOfType<PlatformerWorld>();
    m_player = GetScene()->GetFirstEntityOfType<Player>();
    auto *falcon = GetScene()->GetFirstEntityOfType<Falcon>();

    if (!m_world || !m_player || !falcon)
        throw std::runtime_error("Failed to instantiate required entities (PlatformerWorld, Player, Falcon)");

    auto *camera = GetScene()->GetFirstEntityOfType<LevelCamera>();
    Vec2 cellCount = m_world->GetCellCount();
    float visibleRows = camera && camera->GetVisibleRows() > 0.0f ? camera->GetVisibleRows() : cellCount.y;
    float cellSize = Renderer2D::GetCamera().GetWorldHeight() / visibleRows;
    m_grid = Grid(cellSize, cellCount);

    for (GridEntity *gridEntity : GetScene()->GetAllEntitiesOfType<GridEntity>())
        gridEntity->SetGrid(&m_grid);

    m_world->SetCoordSystem(&m_grid.GetCoordinateSystem());
    m_world->SetNavigator(this);
    m_world->PlaceRegions();

    m_player->SetWorld(m_world);
    falcon->SetWorld(m_world);
    m_player->SetFalcon(falcon);
    
    Vec2 spawnPos = m_world->FindSpawnCell(m_spawnName, m_exitCell);
    m_exitCell.reset();

    // If the spawn cell is right above a solid tile, spawn the player on the ground instead of in the middle of the cell
    if (m_world->IsSolid(Vec2(spawnPos.x, spawnPos.y + 1.0f)))
        spawnPos.y += 0.5f - m_player->GetGridSize().y / 2.0f;

    m_player->SetGridPosition(spawnPos);

    // The grid is centered on the world origin.
    if (camera)
        camera->Follow(m_player, Rect(Vec2::Zero, cellCount * (cellSize * 0.5f)));

    m_cursor = GetScene()->GetFirstEntityOfType<Cursor>();
    if (m_cursor)
        Mouse::SetCursorVisibility(false);

    falcon->SetCursor(m_cursor);

    m_inputManager->Bind(*m_player, *falcon, m_cursor);
}

void Platformer::Update(float deltaTime)
{
    if (!m_player || !m_world)
        return;

    Vec2 cell = m_grid.GetCellFromGridPosition(m_player->GetGridPosition());

    if (const LevelExit *levelExit = m_world->FindExitAt(cell))
    {
        GoToLevel(levelExit->GetTargetLevel(), levelExit->GetTargetSpawn(), cell);
    }
    else if (m_world->IsDeadly(cell))
    {
        ReloadCurrentLevel();
    }
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

    if (m_currentLevel.empty())
        m_currentLevel = m_levelPaths.front();

    SceneData *levelScene = ResourceManager::Load<SceneData>(m_currentLevel, SCENE_CACHE_MODE);
    if (!levelScene || levelScene->IsEmpty())
        throw std::runtime_error("Failed to load level " + m_currentLevel);

    for (const Scene::EntityInfo &entityInfo : levelScene->GetEntities())
        GetScene()->Instantiate(entityInfo);
}

// --- Level Navigation ---

void Platformer::GoToLevel(const std::string &level, const std::string &spawnName, const std::optional<Vec2> &exitCell)
{
    if (level.empty())
    {
        Close();
        return;
    }

    m_currentLevel = level;
    m_spawnName = spawnName;
    m_exitCell = exitCell;
    ReloadScene();
}

// Steps through the root document's level list, for the debug keys.
void Platformer::GoToListedLevel(int step)
{
    auto current = std::find(m_levelPaths.begin(), m_levelPaths.end(), m_currentLevel);
    int index = current == m_levelPaths.end() ? -1 : static_cast<int>(current - m_levelPaths.begin());
    int target = index + step;

    if (target < 0 || target >= static_cast<int>(m_levelPaths.size()))
        return;

    GoToLevel(m_levelPaths[target], "", std::nullopt);
}

void Platformer::ReloadCurrentLevel()
{
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
