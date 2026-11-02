#pragma once

enum class FalconResponse { Latch, Return };

// Something the falcon can be sent to, deciding what happens when it gets there.
class IFalconTarget
{
public:
    virtual ~IFalconTarget() = default;
    virtual FalconResponse OnFalconArrived() = 0;
    // A latched falcon was retrieved.
    virtual void OnFalconLeft() = 0;
};
