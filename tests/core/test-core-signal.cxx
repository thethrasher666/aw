//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/core/signal.hxx>

#include <memory>
#include <vector>

namespace test
{
    namespace
    {
        /// A simple slot target that counts how many times it has been called.
        class Counter
        {
        public:
            explicit Counter(int& calls) : _calls(calls)
            {
            }

            void onSignal()
            {
                ++_calls;
            }

        private:
            int& _calls;
        };

        /// A slot target that records the last value it was called with.
        class ValueReceiver
        {
        public:
            void onValue(int value)
            {
                lastValue = value;
            }

            int lastValue{};
        };

        /// A slot target that records its own identity in a shared order list, to verify
        /// that slots are invoked in connection order.
        class OrderedSlot
        {
        public:
            OrderedSlot(std::vector<int>& order, int id) : _order(order), _id(id)
            {
            }

            void onSignal()
            {
                _order.push_back(_id);
            }

        private:
            std::vector<int>& _order;
            int               _id;
        };

        /// A slot target that disconnects itself from its Signal when destroyed.
        class ObservedWidget final : public aw::core::Observer
        {
        public:
            ~ObservedWidget() override
            {
                disconnectAll();
            }

            void onSignal()
            {
                ++calls;
            }

            int calls{};
        };
    } // namespace

    TEST_CASE("core.Signal.Calls a connected slot")
    {
        aw::core::Signal<> signal;
        int                calls = 0;
        Counter            counter(calls);

        signal.connect(&Counter::onSignal, &counter);
        signal();

        REQUIRE(calls == 1);
    }

    TEST_CASE("core.Signal.Connects to an object held by a smart pointer")
    {
        aw::core::Signal<> signal;
        int                calls = 0;
        auto               counter = std::make_unique<Counter>(calls);

        signal.connect(&Counter::onSignal, counter);
        signal();

        REQUIRE(calls == 1);
    }

    TEST_CASE("core.Signal.Passes arguments to its slots")
    {
        aw::core::Signal<int> signal;
        ValueReceiver          receiver;

        signal.connect(&ValueReceiver::onValue, &receiver);
        signal(42);

        REQUIRE(receiver.lastValue == 42);
    }

    TEST_CASE("core.Signal.Calls slots in connection order")
    {
        aw::core::Signal<> signal;
        std::vector<int>   order;

        OrderedSlot first(order, 1);
        OrderedSlot second(order, 2);

        signal.connect(&OrderedSlot::onSignal, &first);
        signal.connect(&OrderedSlot::onSignal, &second);
        signal();

        REQUIRE(order == std::vector<int>{ 1, 2 });
    }

    TEST_CASE("core.Signal.Disconnect stops further delivery to that slot")
    {
        aw::core::Signal<> signal;
        int                calls = 0;
        Counter            counter(calls);

        auto connection = signal.connect(&Counter::onSignal, &counter);

        signal();
        REQUIRE(calls == 1);

        REQUIRE(connection.disconnect());

        signal();
        REQUIRE(calls == 1);
        REQUIRE_FALSE(connection.disconnect());
    }

    TEST_CASE("core.Signal.ScopedConnection disconnects automatically")
    {
        aw::core::Signal<> signal;
        int                calls = 0;
        Counter            counter(calls);

        {
            auto scoped = signal.connectScoped(&Counter::onSignal, &counter);
            signal();
            REQUIRE(calls == 1);
        }

        signal();
        REQUIRE(calls == 1);
    }

    TEST_CASE("core.Signal.Skips a slot once its tracked shared_ptr object expires")
    {
        aw::core::Signal<> signal;
        int                calls  = 0;
        auto               widget = std::make_shared<Counter>(calls);

        signal.connect(&Counter::onSignal, widget);

        signal();
        REQUIRE(calls == 1);
        REQUIRE(signal.slotCount() == 1);

        widget.reset();

        signal();
        REQUIRE(calls == 1);
        REQUIRE(signal.slotCount() == 0);
    }

    TEST_CASE("core.Signal.Connects directly via weak_ptr")
    {
        aw::core::Signal<> signal;
        int                calls  = 0;
        auto               widget = std::make_shared<Counter>(calls);

        signal.connect(&Counter::onSignal, std::weak_ptr<Counter>(widget));

        signal();
        REQUIRE(calls == 1);

        widget.reset();

        signal();
        REQUIRE(calls == 1);
        REQUIRE(signal.slotCount() == 0);
    }

    TEST_CASE("core.Signal.Disconnects a slot when its Observer-derived object is destroyed")
    {
        aw::core::Signal<> signal;

        {
            ObservedWidget widget;
            signal.connect(&ObservedWidget::onSignal, &widget);

            signal();
            REQUIRE(widget.calls == 1);
        }

        REQUIRE(signal.slotCount() == 0);
        signal();
    }

    TEST_CASE("core.Signal.Block/unblock suppress and resume delivery")
    {
        aw::core::Signal<> signal;
        int                calls = 0;
        Counter            counter(calls);

        signal.connect(&Counter::onSignal, &counter);

        signal.block();
        REQUIRE(signal.blocked());
        signal();
        REQUIRE(calls == 0);

        signal.unblock();
        REQUIRE_FALSE(signal.blocked());
        signal();
        REQUIRE(calls == 1);
    }
} // namespace test
