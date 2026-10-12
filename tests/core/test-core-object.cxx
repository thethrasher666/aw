//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/core/object.hxx>

#include <memory>

namespace test
{
    namespace
    {
        /// An object that reports its lifetime through a shared counter.
        class TrackedObject final : public aw::core::Object
        {
        public:
            explicit TrackedObject(int& liveCount, aw::core::Object* parent = nullptr) : Object(parent), _liveCount(liveCount)
            {
                ++_liveCount;
            }

            ~TrackedObject() override
            {
                --_liveCount;
            }

        private:
            int& _liveCount;
        };
    } // namespace

    TEST_CASE("core.Object.Destroys its children")
    {
        int liveCount = 0;

        {
            aw::core::Object parent;

            auto* const child = parent.createChild<TrackedObject>(liveCount);
            auto* const grandChild = child->createChild<TrackedObject>(liveCount, child);

            REQUIRE(liveCount == 2);
            REQUIRE(parent.children().size() == 1);
            REQUIRE(child->children().size() == 1);
            REQUIRE(child->parent() == &parent);
            REQUIRE(grandChild->parent() == child);
        }

        REQUIRE(liveCount == 0);
    }

    TEST_CASE("core.Object.Adopts an existing object")
    {
        int liveCount = 0;

        {
            aw::core::Object parent;

            auto* const child = parent.addChild(std::make_unique<TrackedObject>(liveCount));

            REQUIRE(liveCount == 1);
            REQUIRE(child->parent() == &parent);
        }

        REQUIRE(liveCount == 0);
    }

    TEST_CASE("core.Object.Gives up a removed child")
    {
        int                               liveCount = 0;
        std::unique_ptr<aw::core::Object> removed;

        {
            aw::core::Object parent;

            auto* const child = parent.createChild<TrackedObject>(liveCount);
            removed = parent.removeChild(child);

            REQUIRE(removed.get() == child);
            REQUIRE(removed->parent() == nullptr);
            REQUIRE(parent.children().empty());
            REQUIRE(parent.removeChild(child) == nullptr);
        }

        REQUIRE(liveCount == 1);

        removed.reset();

        REQUIRE(liveCount == 0);
    }
} // namespace test
