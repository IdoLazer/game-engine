#pragma once

#include "Subscription.h"
#include <algorithm>
#include <functional>
#include <vector>

namespace Engine
{

    /// EventSubscriber is the read-only side of an Event. External code can
    /// subscribe to it but cannot raise notifications. The owning class holds
    /// the full Event<Args...> and exposes EventSubscriber<Args...>& publicly.
    template <typename... Args>
    class EventSubscriber
    {
    public:
        // --- Constructors & Destructors ---
        EventSubscriber() = default;
        ~EventSubscriber() = default;

        // Non-copyable, non-movable (subscriptions hold pointers to us)
        EventSubscriber(const EventSubscriber &) = delete;
        EventSubscriber &operator=(const EventSubscriber &) = delete;
        EventSubscriber(EventSubscriber &&) = delete;
        EventSubscriber &operator=(EventSubscriber &&) = delete;

        // --- Subscription ---

        /// Subscribe with a callable (lambda, std::function, free function).
        Subscription Subscribe(std::function<void(const Args &...)> handler)
        {
            std::size_t id = m_nextId++;
            m_handlers.push_back({id, std::move(handler)});
            return Subscription([this, id]() { RemoveHandler(id); });
        }

        /// Subscribe with a member function pointer.
        template <typename TInstance>
        Subscription Subscribe(TInstance *instance, void (TInstance::*method)(const Args &...))
        {
            return Subscribe([instance, method](const Args &...args)
                             { (instance->*method)(args...); });
        }

        // --- Accessors ---
        std::size_t GetSubscriberCount() const { return m_handlers.size(); }

    protected:
        // --- Notification (accessible to Event<Args...>) ---

        void NotifySubscribers(const Args &...args)
        {
            m_isNotifying = true;

            for (auto &entry : m_handlers)
            {
                entry.handler(args...);
            }

            m_isNotifying = false;

            // Process any unsubscriptions that happened during notification
            for (auto id : m_pendingRemovals)
            {
                RemoveHandlerImmediate(id);
            }
            m_pendingRemovals.clear();
        }

    private:
        // --- Types ---
        struct HandlerEntry
        {
            std::size_t id;
            std::function<void(const Args &...)> handler;
        };

        // --- Fields ---
        std::vector<HandlerEntry> m_handlers;
        std::size_t m_nextId = 0;
        bool m_isNotifying = false;
        std::vector<std::size_t> m_pendingRemovals;

        // --- Internal ---

        void RemoveHandler(std::size_t id)
        {
            if (m_isNotifying)
            {
                m_pendingRemovals.push_back(id);
            }
            else
            {
                RemoveHandlerImmediate(id);
            }
        }

        void RemoveHandlerImmediate(std::size_t id)
        {
            std::erase_if(m_handlers, [id](const HandlerEntry &entry)
                          { return entry.id == id; });
        }
    };

    /// Event is the full interface — adds Notify() on top of EventSubscriber.
    /// The owning class holds Event<Args...> privately and exposes
    /// EventSubscriber<Args...>& through a public accessor.
    template <typename... Args>
    class Event : public EventSubscriber<Args...>
    {
    public:
        /// Notify all subscribers with the given arguments.
        void Notify(const Args &...args)
        {
            this->NotifySubscribers(args...);
        }
    };

}
