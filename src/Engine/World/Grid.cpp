#include "Grid.h"
#include "../Entity/GridEntity.h"

namespace Engine
{
    void Grid::Register(GridEntity *entity, const Vec2 &cell)
    {
        int key = CellKey(cell);
        m_entityMap[key].push_back(entity);
    }

    void Grid::Unregister(GridEntity *entity, const Vec2 &cell)
    {
        int key = CellKey(cell);
        auto it = m_entityMap.find(key);
        if (it != m_entityMap.end())
        {
            auto &vec = it->second;
            vec.erase(std::remove(vec.begin(), vec.end(), entity), vec.end());
            if (vec.empty())
                m_entityMap.erase(it);
        }
    }

    void Grid::UpdateRegistration(GridEntity *entity, const Vec2 &oldCell, const Vec2 &newCell)
    {
        Unregister(entity, oldCell);
        Register(entity, newCell);
    }

    bool Grid::IsOccupied(const Vec2 &cell) const
    {
        int key = CellKey(cell);
        auto it = m_entityMap.find(key);
        return it != m_entityMap.end() && !it->second.empty();
    }
}