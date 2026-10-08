#include "SpawnPoint.h"

BEGIN_TYPE_REGISTER(SpawnPoint)
    REGISTER_PROPERTY(std::string, Name, &SpawnPoint::m_name)
    REGISTER_PROPERTY(std::string, Anchor, &SpawnPoint::m_anchor)
END_TYPE_REGISTER()
