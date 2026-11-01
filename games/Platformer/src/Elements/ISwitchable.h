#pragma once

// A level region another can switch on and off.
class ISwitchable
{
public:
    virtual ~ISwitchable() = default;
    virtual void OnSwitched(bool on) = 0;
};
