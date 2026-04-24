#pragma once

#include <Engine.h>

class PlatformTile : public Engine::GridTile
{
    DECLARE_TYPE(PlatformTile, GridTile)

// --- Constructors & Destructors ---
public:
    PlatformTile() = default;
    ~PlatformTile() override = default;

// --- Lifecycle ---
};