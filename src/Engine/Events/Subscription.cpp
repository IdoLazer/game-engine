#include "Subscription.h"

namespace Engine
{

// --- Constructors & Destructors ---

    Subscription::Subscription(std::function<void()> unsubscriber)
        : m_unsubscribe(std::move(unsubscriber))
    {
    }

    Subscription::~Subscription()
    {
        Unsubscribe();
    }

// --- Move Semantics ---

    Subscription::Subscription(Subscription &&other) noexcept
        : m_unsubscribe(std::move(other.m_unsubscribe))
    {
        other.m_unsubscribe = nullptr;
    }

    Subscription &Subscription::operator=(Subscription &&other) noexcept
    {
        if (this != &other)
        {
            Unsubscribe();

            m_unsubscribe = std::move(other.m_unsubscribe);
            other.m_unsubscribe = nullptr;
        }
        return *this;
    }

// --- Operations ---

    void Subscription::Unsubscribe()
    {
        if (m_unsubscribe)
        {
            m_unsubscribe();
            m_unsubscribe = nullptr;
        }
    }

// --- Accessors ---

    bool Subscription::IsActive() const
    {
        return m_unsubscribe != nullptr;
    }

}
