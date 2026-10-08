#include "Platformer.h"
#include "PlatformerConstants.h"
#include "PlatformerWorld.h"
#include "Player.h"
#include "PlatformerInputManager.h"
#include "Levels/LevelSet.h"
#include "Levels/LevelExit.h"
#include "Levels/SpawnPoint.h"
#include "Cursor.h"
#include "Falcon.h"

#include <algorithm>
#include <iostream>
#include <set>

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

    std::optional<Rect> FindAnchor(const PlatformerWorld &world, const std::string &anchor)
    {
        if (anchor.size() != 1)
            return std::nullopt;

        return world.GetTileGrid().FindAnchor(anchor[0]);
    }

    // Reports every anchor an entity names that the picture lacks, and every one drawn that no entity names.
    void ReportAnchorProblems(const std::string &level, const PlatformerWorld &world,
                              const std::vector<SpawnPoint *> &spawnPoints, const std::vector<LevelExit *> &levelExits)
    {
        std::set<char> named;

        auto check = [&](const std::string &owner, const std::string &anchor)
        {
            if (anchor.size() != 1)
            {
                std::cerr << level << ": " << owner << " needs an Anchor of exactly one character, not '"
                          << anchor << "'" << std::endl;
                return;
            }

            named.insert(anchor[0]);

            if (!FindAnchor(world, anchor))
            {
                std::cerr << level << ": " << owner << " names Anchor '" << anchor
                          << "', which is not drawn in the picture" << std::endl;
            }
        };

        for (const SpawnPoint *spawnPoint : spawnPoints)
            check("SpawnPoint '" + spawnPoint->GetName() + "'", spawnPoint->GetAnchor());

        for (const LevelExit *levelExit : levelExits)
            check("LevelExit", levelExit->GetAnchor());

        for (const auto &[glyph, region] : world.GetTileGrid().GetAnchors())
        {
            if (!named.contains(glyph))
            {
                std::cerr << level << ": '" << glyph
                          << "' is drawn in the picture but no SpawnPoint or LevelExit names it" << std::endl;
            }
        }
    }

    // The SpawnPoint called `name`, or the level's first when `name` is empty or the level has none by it.
    const SpawnPoint *FindSpawnPoint(const std::string &level, const std::vector<SpawnPoint *> &spawnPoints,
                                     const std::string &name)
    {
        if (!name.empty())
        {
            for (const SpawnPoint *spawnPoint : spawnPoints)
            {
                if (spawnPoint->GetName() == name)
                    return spawnPoint;
            }

            std::cerr << level << ": no SpawnPoint named '" << name << "'" << std::endl;
        }

        return spawnPoints.empty() ? nullptr : spawnPoints.front();
    }

    // Clamps `value` so a body of `halfExtent` stays within [min, max], centering it when it cannot fit.
    float KeepInside(float value, float min, float max, float halfExtent)
    {
        if (max - min < halfExtent * 2.0f)
            return (min + max) * 0.5f;

        return std::clamp(value, min + halfExtent, max - halfExtent);
    }

    // Standing on the region's bottom edge - or, arriving through an exit, at the spot in the
    // region that matches where the player left the exit.
    Vec2 SpawnPosition(const Rect &region, const Vec2 &halfExtents, const std::optional<Vec2> &carry)
    {
        Vec2 min = region.Min();
        Vec2 max = region.Max();

        if (!carry)
            return Vec2(region.center.x, max.y - halfExtents.y);

        return Vec2(KeepInside(min.x + carry->x * (max.x - min.x), min.x, max.x, halfExtents.x),
                    KeepInside(min.y + carry->y * (max.y - min.y), min.y, max.y, halfExtents.y));
    }
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
    
    std::vector<SpawnPoint *> spawnPoints = GetScene()->GetAllEntitiesOfType<SpawnPoint>();
    std::vector<LevelExit *> levelExits = GetScene()->GetAllEntitiesOfType<LevelExit>();
    ReportAnchorProblems(m_currentLevel, *world, spawnPoints, levelExits);

    for (const LevelExit *levelExit : levelExits)
    {
        if (std::optional<Rect> bounds = FindAnchor(*world, levelExit->GetAnchor()))
            m_exits.push_back(ExitZone{*bounds, levelExit->GetTargetLevel(), levelExit->GetTargetSpawn()});
    }

    const SpawnPoint *spawnPoint = FindSpawnPoint(m_currentLevel, spawnPoints, m_spawn.name);
    std::optional<Rect> spawnRegion = spawnPoint ? FindAnchor(*world, spawnPoint->GetAnchor()) : std::nullopt;

    if (spawnRegion)
    {
        player->SetGridPosition(SpawnPosition(*spawnRegion, player->GetGridSize() / 2.0f, m_spawn.carry));
    }
    else
    {
        std::cerr << m_currentLevel << ": no usable SpawnPoint, placing the player at 1, 1" << std::endl;
        player->SetGridPosition(Vec2(1.0f, 1.0f));
    }

    m_spawn.carry.reset();

    m_cursor = GetScene()->GetFirstEntityOfType<Cursor>();
    if (m_cursor)
        Mouse::SetCursorVisibility(false);

    falcon->SetCursor(m_cursor);

    m_inputManager->Bind(*player, *falcon);
}

void Platformer::Update(float deltaTime)
{
    if (!m_player || !m_world)
        return;

    Vec2 position = m_player->GetGridPosition();

    for (const ExitZone &exit : m_exits)
    {
        if (!exit.bounds.Contains(position))
            continue;

        if (exit.targetLevel.empty())
        {
            Close();
            return;
        }

        Vec2 fromMin = position - exit.bounds.Min();
        Vec2 size = exit.bounds.halfExtents * 2.0f;
        GoToLevel(exit.targetLevel, Spawn{exit.targetSpawn, Vec2(fromMin.x / size.x, fromMin.y / size.y)});
        return;
    }

    if (m_world->IsDeadly(m_grid.GetCellFromGridPosition(position)))
        ReloadCurrentLevel();
}

void Platformer::Shutdown()
{
    m_inputManager->Unbind();
    m_exits.clear();
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

void Platformer::GoToLevel(const std::string &level, Spawn spawn)
{
    m_currentLevel = level;
    m_spawn = std::move(spawn);
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

    GoToLevel(m_levelPaths[target], Spawn{});
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
