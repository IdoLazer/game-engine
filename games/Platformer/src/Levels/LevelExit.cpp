#include "LevelExit.h"

BEGIN_TYPE_REGISTER(LevelExit)
    REGISTER_PROPERTY(std::string, Anchor, &LevelExit::m_anchor)
    REGISTER_PROPERTY(std::string, TargetLevel, &LevelExit::m_targetLevel)
    REGISTER_PROPERTY(std::string, TargetSpawn, &LevelExit::m_targetSpawn)
END_TYPE_REGISTER()
