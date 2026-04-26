#include <gtest/gtest.h>
#include "Events/Event.h"

using namespace Engine;

// --- Basic Subscribe and Notify ---

TEST(EventTest, SubscriberIsCalledOnNotify)
{
    Event<int> event;
    int received = 0;

    auto sub = event.Subscribe([&received](const int &value)
                               { received = value; });

    event.Notify(42);
    EXPECT_EQ(received, 42);
}

TEST(EventTest, MultipleSubscribersAllCalled)
{
    Event<int> event;
    int countA = 0, countB = 0;

    auto subA = event.Subscribe([&countA](const int &) { countA++; });
    auto subB = event.Subscribe([&countB](const int &) { countB++; });

    event.Notify(1);
    EXPECT_EQ(countA, 1);
    EXPECT_EQ(countB, 1);
}

TEST(EventTest, NoSubscribersDoesNothing)
{
    Event<int> event;
    event.Notify(42); // should not crash
}

// --- Multiple and struct arguments ---

TEST(EventTest, MultipleArgTypes)
{
    Event<int, float> event;
    int gotInt = 0;
    float gotFloat = 0.0f;

    auto sub = event.Subscribe([&](const int &i, const float &f)
                               { gotInt = i; gotFloat = f; });

    event.Notify(7, 3.14f);
    EXPECT_EQ(gotInt, 7);
    EXPECT_FLOAT_EQ(gotFloat, 3.14f);
}

TEST(EventTest, StructArgs)
{
    struct Damage { int amount; };

    Event<Damage> event;
    int received = 0;

    auto sub = event.Subscribe([&received](const Damage &d)
                               { received = d.amount; });

    event.Notify(Damage{25});
    EXPECT_EQ(received, 25);
}

// --- Subscription lifecycle ---

TEST(EventTest, UnsubscribesOnDestruction)
{
    Event<int> event;
    int calls = 0;

    {
        auto sub = event.Subscribe([&calls](const int &) { calls++; });
        event.Notify(1);
        EXPECT_EQ(calls, 1);
    } // sub destroyed here

    event.Notify(2);
    EXPECT_EQ(calls, 1); // no additional call
}

TEST(EventTest, MoveSubscription)
{
    Event<int> event;
    int calls = 0;

    Subscription outer;
    {
        auto inner = event.Subscribe([&calls](const int &) { calls++; });
        outer = std::move(inner);
    } // inner destroyed, but subscription moved to outer

    event.Notify(1);
    EXPECT_EQ(calls, 1); // still subscribed via outer
}

TEST(EventTest, ReassignmentUnsubscribesPrevious)
{
    Event<int> eventA;
    Event<int> eventB;
    int callsA = 0, callsB = 0;

    auto sub = eventA.Subscribe([&callsA](const int &) { callsA++; });
    sub = eventB.Subscribe([&callsB](const int &) { callsB++; });

    eventA.Notify(1);
    eventB.Notify(1);
    EXPECT_EQ(callsA, 0); // unsubscribed from A
    EXPECT_EQ(callsB, 1);
}

TEST(EventTest, SubscriberCountTracked)
{
    Event<int> event;
    EXPECT_EQ(event.GetSubscriberCount(), 0u);

    auto subA = event.Subscribe([](const int &) {});
    EXPECT_EQ(event.GetSubscriberCount(), 1u);

    {
        auto subB = event.Subscribe([](const int &) {});
        EXPECT_EQ(event.GetSubscriberCount(), 2u);
    }

    EXPECT_EQ(event.GetSubscriberCount(), 1u);
}

// --- Member function subscription ---

TEST(EventTest, MemberFunctionSubscription)
{
    struct Listener
    {
        int value = 0;
        void OnEvent(const int &v) { value = v; }
    };

    Event<int> event;
    Listener listener;

    auto sub = event.Subscribe(&listener, &Listener::OnEvent);
    event.Notify(99);
    EXPECT_EQ(listener.value, 99);
}

// --- Unsubscribe during notification ---

TEST(EventTest, UnsubscribeDuringNotification)
{
    Event<int> event;
    int calls = 0;
    Subscription sub;

    sub = event.Subscribe([&](const int &)
                          {
                              calls++;
                              sub.Unsubscribe();
                          });

    event.Notify(1);
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(event.GetSubscriberCount(), 0u);
}

// --- Explicit Unsubscribe ---

TEST(EventTest, ExplicitUnsubscribe)
{
    Event<int> event;
    int calls = 0;

    auto sub = event.Subscribe([&calls](const int &) { calls++; });
    event.Notify(1);
    EXPECT_EQ(calls, 1);
    EXPECT_TRUE(sub.IsActive());

    sub.Unsubscribe();
    EXPECT_FALSE(sub.IsActive());

    event.Notify(2);
    EXPECT_EQ(calls, 1); // no additional call
}

TEST(EventTest, DoubleUnsubscribeIsSafe)
{
    Event<int> event;
    auto sub = event.Subscribe([](const int &) {});

    sub.Unsubscribe();
    sub.Unsubscribe(); // should not crash or double-remove
    EXPECT_EQ(event.GetSubscriberCount(), 0u);
}

// --- EventSubscriber cannot Notify ---

TEST(EventTest, EventSubscriberCannotNotify)
{
    // Event<int> has Notify(), but EventSubscriber<int> does not.
    // This is a compile-time guarantee — we verify the reference type works.
    Event<int> event;
    EventSubscriber<int> &subscriberView = event;

    int received = 0;
    auto sub = subscriberView.Subscribe([&received](const int &v)
                                        { received = v; });

    event.Notify(42);
    EXPECT_EQ(received, 42);
}
