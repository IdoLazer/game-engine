#include "GridEntity.h"

namespace Engine
{

// --- Type Registration ---

BEGIN_TYPE_REGISTER(GridEntity)
    REGISTER_PROPERTY(Grid*, Grid, &GridEntity::m_grid)
    REGISTER_PROPERTY(Vec2, GridPosition, &GridEntity::m_gridPosition)
    REGISTER_PROPERTY(Vec2, GridSize, &GridEntity::m_gridSize)
END_TYPE_REGISTER()

// --- Destructors ---

GridEntity::~GridEntity()
{
    if (m_grid && m_registered)
    {
        Vec2 cell = m_grid->GetCellFromGridPosition(m_gridPosition);
        m_grid->Unregister(this, cell);
    }
}

// --- Lifecycle ---

void GridEntity::Initialize()
{
    SyncGridToWorld();

    if (m_grid)
    {
        Vec2 cell = m_grid->GetCellFromGridPosition(m_gridPosition);
        m_grid->Register(this, cell);
        m_registered = true;
    }
}

// --- Accessors ---

Vec2 GridEntity::GetGridPosition() const { return m_gridPosition; }
Vec2 GridEntity::GetGridCell() const { return m_grid ? m_grid->GetCellFromGridPosition(m_gridPosition) : Vec2{}; }
Vec2 GridEntity::GetGridSize() const { return m_gridSize; }
Grid *GridEntity::GetGrid() const { return m_grid; }
const GridCoordinateSystem &GridEntity::GetCoordinateSystem() const { return m_grid->GetCoordinateSystem(); }
void GridEntity::SetGrid(Grid *grid) { m_grid = grid; }

void GridEntity::SetGridPosition(const Vec2 &gridPos)
{
    if (m_grid && m_registered)
    {
        Vec2 oldCell = m_grid->GetCellFromGridPosition(m_gridPosition);
        Vec2 newCell = m_grid->GetCellFromGridPosition(gridPos);
        if (!(oldCell == newCell))
            m_grid->UpdateRegistration(this, oldCell, newCell);
    }
    m_gridPosition = gridPos;
    SyncGridToWorld();
}

void GridEntity::SetGridSize(const Vec2 &gridSize)
{
    m_gridSize = gridSize;
    SyncGridToWorld();
}

// --- Internal ---

void GridEntity::SyncGridToWorld()
{
    if (m_grid)
    {
        m_worldPosition = m_grid->GridToWorld(m_gridPosition);
        float cellSize = m_grid->GetCellSize();
        m_worldSize = Vec2(m_gridSize.x * cellSize, m_gridSize.y * cellSize);
    }
}

}
