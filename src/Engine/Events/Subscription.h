#pragma once

#include <functional>

namespace Engine
{

    /// RAII handle that automatically unsubscribes from an Event when destroyed.
    /// Move-only — cannot be copied, but can be transferred to a new owner.
    class Subscription
    {
    public:
        // --- Constructors & Destructors ---
        Subscription() = default;
        ~Subscription();

        // --- Move Semantics ---
        Subscription(Subscription &&other) noexcept;
        Subscription &operator=(Subscription &&other) noexcept;

        // --- Non-Copyable ---
        Subscription(const Subscription &) = delete;
        Subscription &operator=(const Subscription &) = delete;

        // --- Operations ---
        void Unsubscribe();

        // --- Accessors ---
        bool IsActive() const;

    private:
        explicit Subscription(std::function<void()> unsubscriber);

        template <typename... Args>
        friend class EventSubscriber;

        std::function<void()> m_unsubscribe;
    };

}
