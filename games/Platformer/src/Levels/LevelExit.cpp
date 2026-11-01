#include "LevelExit.h"
#include "../ILevelNavigator.h"
#include "../PlatformerWorld.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(LevelExit)
    REGISTER_PROPERTY(std::string, TargetLevel, &LevelExit::m_targetLevel)
    REGISTER_PROPERTY(std::string, TargetSpawn, &LevelExit::m_targetSpawn)
END_TYPE_REGISTER()

// --- Public Interface ---

bool LevelExit::Place(PlatformerWorld &world)
{
    if (!LevelRegion::Place(world))
        return false;

    m_collider.SetTrigger(true);
    world.GetPhysics().Add(m_collider);
    m_enteredSubscription = m_collider.OnEntered().Subscribe(this, &LevelExit::OnEntered);
    return true;
}

// --- Internal ---

void LevelExit::OnEntered(const Collider &other)
{
    GetWorld().GetNavigator().GoToLevel(m_targetLevel, m_targetSpawn, other.GetOwner().GetGridCell());
}
