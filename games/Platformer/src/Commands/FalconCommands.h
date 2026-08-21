#pragma once

#include <Engine.h>
#include "../Falcon.h"

class StartAimingCommand : public Engine::Command
{
public:
    explicit StartAimingCommand(Falcon &falcon) : m_falcon(falcon) {}
    void Execute() override { m_falcon.StartAiming(); }
    const char *GetName() const override { return "StartAimingCommand"; }

private:
    Falcon &m_falcon;
};
