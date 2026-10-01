#include "LevelSet.h"

BEGIN_TYPE_REGISTER(LevelSet)
    REGISTER_PROPERTY(std::vector<std::string>, Levels, &LevelSet::m_levels)
END_TYPE_REGISTER()
