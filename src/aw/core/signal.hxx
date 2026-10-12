//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>

#include <atomic>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace aw::core
{
    template <typename... T>
    class Signal; ///< Forward declaration.

    namespace detail
    {
        /// Detects std::shared_ptr/std::weak_ptr specializations, to disambiguate overload resolution
        /// in Signal::connect() between a tracked pointer and a plain (untracked) one.
        template <typename T>
        constexpr bool isTrackedPointer = false;

        template <typename T>
        constexpr bool isTrackedPointer<std::shared_ptr<T>> = true;

        template <typename T>
        constexpr bool isTrackedPointer<std::weak_ptr<T>> = true;

        class SlotState; ///< Forward declaration.

        /// Interface implemented by Signal, used by SlotState to report its own disconnection
        /// so the owning signal can remove it from its slot list.
        class Cleanable
        {
        public:
            /// Destructor.
            ///
            /// Cleans up the slot state and notifies the owning signal if necessary.
            virtual ~Cleanable() = default;

            /// Cleans up the slot state and notifies the owning signal if necessary.
            ///
            /// \param slotState The slot state to clean.
            virtual void clean(SlotState* slotState) = 0;
        };

        /// Slot state independent of the slot's argument and callable types, so that Connection
        /// and ScopedConnection can interact with a slot without knowing its concrete type.
        class SlotState
        {
        public:
            /// Constructor.
            ///
            /// \param cleaner The cleaner responsible for this slot state.
            explicit SlotState(Cleanable& cleaner) : _cleaner(cleaner)
            {
            }

            /// Destructor.
            ///
            /// Cleans up the slot state and notifies the owning signal if necessary.
            virtual ~SlotState() = default;

            /// Checks if the slot is currently connected.
            ///
            /// \return True if the slot is connected, false otherwise.
            [[nodiscard]] virtual auto connected() const noexcept -> bool
            {
                return _connected;
            }

            /// Disconnects the slot.
            ///
            /// \return True if the slot was connected before disconnection, false otherwise.
            auto disconnect() noexcept -> bool
            {
                bool const wasConnected = _connected.exchange(false);
                if (wasConnected)
                {
                    _cleaner.clean(this);
                }
                return wasConnected;
            }

            /// Checks if the slot is currently blocked.
            ///
            /// \return True if the slot is blocked, false otherwise.
            [[nodiscard]] auto blocked() const noexcept -> bool
            {
                return _blocked;
            }

            /// Blocks the slot, preventing it from being invoked.
            void block() noexcept
            {
                _blocked = true;
            }

            /// Unblocks the slot, allowing it to be invoked.
            void unblock() noexcept
            {
                _blocked = false;
            }

            /// Gets the index of the slot in the signal's slot list.
            ///
            /// \return The index of the slot.
            [[nodiscard]] auto index() const noexcept -> std::size_t
            {
                return _index;
            }

            /// Sets the index of the slot in the signal's slot list.
            ///
            /// \param index The new index of the slot.
            void setIndex(std::size_t index) noexcept
            {
                _index = index;
            }

        private:
            Cleanable&        _cleaner;
            std::size_t       _index{};
            std::atomic<bool> _connected{ true };
            std::atomic<bool> _blocked{ false };
        };

        /// Type independent base class for a slot, so a Signal can hold a homogeneous list of
        /// slots regardless of the member function pointer and object types they were built from.
        template <typename... Args>
        class SlotBase : public SlotState
        {
        public:
            /// Constructor.
            ///
            /// \param cleaner The cleaner responsible for this slot base.
            explicit SlotBase(Cleanable& cleaner) : SlotState(cleaner)
            {
            }

            /// Invokes the slot with the given arguments.
            ///
            /// \param args The arguments to pass to the slot.
            void operator()(Args... args)
            {
                // Deliberately calls SlotState::connected() rather than the (possibly overridden)
                // connected(): a tracked slot must still run callSlot() while merely expired, so it
                // can lazily disconnect itself. Only an explicit disconnect() should skip it here.
                if (SlotState::connected() && !blocked())
                {
                    callSlot(args...);
                }
            }

        protected:
            virtual void callSlot(Args... args) = 0;
        };

        /// A slot that calls a member function on an object referenced by a plain pointer. The
        /// caller is responsible for the object outliving the connection.
        template <typename Pmf, typename Ptr, typename... Args>
        class Slot final : public SlotBase<Args...>
        {
        public:
            /// Constructor.
            ///
            /// \param cleaner The cleaner responsible for this slot.
            /// \param pmf The member function pointer to call.
            /// \param ptr The pointer to the object on which to call the member function.
            Slot(Cleanable& cleaner, Pmf pmf, Ptr* ptr) : SlotBase<Args...>(cleaner), _pmf(pmf), _ptr(ptr)
            {
            }

        protected:
            /// Calls the member function on the object with the given arguments.
            ///
            /// \param args The arguments to pass to the member function.
            void callSlot(Args... args) override
            {
                std::invoke(_pmf, _ptr, args...);
            }

        private:
            Pmf  _pmf;
            Ptr* _ptr;
        };

        /// A slot that calls a member function on an object tracked by a std::weak_ptr. The slot
        /// is automatically treated as disconnected, and lazily disconnected, once the object expires.
        template <typename Pmf, typename U, typename... Args>
        class TrackedSlot final : public SlotBase<Args...>
        {
        public:
            /// Constructor.
            ///
            /// \param cleaner The cleaner responsible for this slot.
            /// \param pmf The member function pointer to call.
            /// \param ptr The weak pointer to the object on which to call the member function.
            TrackedSlot(Cleanable& cleaner, Pmf pmf, std::weak_ptr<U> ptr) : SlotBase<Args...>(cleaner), _pmf(pmf), _ptr(std::move(ptr))
            {
            }

            /// Determines whether the slot is still connected to its signal.
            ///
            /// \return True if the slot is connected, false otherwise.
            [[nodiscard]] auto connected() const noexcept -> bool override
            {
                return !_ptr.expired() && SlotState::connected();
            }

        protected:
            /// Calls the member function on the object with the given arguments if the object is still alive.
            ///
            /// \param args The arguments to pass to the member function.
            void callSlot(Args... args) override
            {
                auto const locked = _ptr.lock();
                if (!locked)
                {
                    SlotState::disconnect();
                    return;
                }
                std::invoke(_pmf, locked, args...);
            }

        private:
            Pmf              _pmf;
            std::weak_ptr<U> _ptr;
        };
    } // namespace detail

    /// Allows interaction with an ongoing slot connection, such as blocking or disconnecting it.
    /// A connection is not a RAII object: nothing needs to keep one alive for the signal-slot
    /// connection itself to remain active.
    class Connection
    {
    public:
        /// Constructor.
        Connection() = default;

        /// Destructor.
        virtual ~Connection() = default;

        /// Copy constructor for the connection.
        Connection(Connection const&)                     = default;

        /// Copy assignment operator for the connection.
        /// \return A reference to this connection after the assignment.
        auto operator=(Connection const&) -> Connection&  = default;

        /// Move constructor for the connection.
        Connection(Connection&&) noexcept                 = default;

        /// Move assignment operator for the connection.
        /// \return A reference to this connection after the assignment.
        auto operator=(Connection&&) noexcept -> Connection& = default;

        /// Determine whether the underlying slot still exists (regardless of its connected state).
        /// \return True if the slot is still valid, false otherwise.
        [[nodiscard]] auto valid() const noexcept -> bool
        {
            return !_state.expired();
        }

        /// Determine whether the slot is still connected to its signal.
        ///
        /// \return True if the slot is connected, false otherwise.
        [[nodiscard]] auto connected() const noexcept -> bool
        {
            auto const state = _state.lock();
            return state && state->connected();
        }

        /// Disconnect the slot from its signal.
        ///
        /// \return true if the slot was connected and has now been disconnected.
        auto disconnect() noexcept -> bool
        {
            auto const state = _state.lock();
            return state && state->disconnect();
        }

        /// Determine whether the slot is currently blocked.
        ///
        /// \return True if the slot is blocked, false otherwise.
        [[nodiscard]] auto blocked() const noexcept -> bool
        {
            auto const state = _state.lock();
            return state && state->blocked();
        }

        /// Block the slot: it is skipped on emission without being disconnected.
        ///
        /// \note This does not disconnect the slot; it merely prevents it from being invoked during signal emission.
        void block() noexcept
        {
            if (auto const state = _state.lock())
            {
                state->block();
            }
        }

        /// Unblock a previously blocked slot.
        ///
        /// \note The slot will be invoked again during signal emission if it was previously blocked.
        void unblock() noexcept
        {
            if (auto const state = _state.lock())
            {
                state->unblock();
            }
        }

    protected:
        template <typename...>
        friend class Signal;

        /// Construct a connection from a weak pointer to the slot state.
        ///
        /// \param state The weak pointer to the slot state.
        /// \note This constructor is used internally by the Signal class to create connections.
        explicit Connection(std::weak_ptr<detail::SlotState> state) noexcept : _state(std::move(state))
        {
        }

        std::weak_ptr<detail::SlotState> _state;
    };

    /// A RAII version of Connection: disconnects the underlying slot upon destruction.
    class ScopedConnection final : public Connection
    {
    public:
        /// Default constructor.
        ScopedConnection() = default;

        /// Construct a scoped connection from a regular connection.
        ///
        /// \param connection The connection to manage.
        /// \note The ScopedConnection will automatically disconnect the underlying slot when it is destroyed.
        /*implicit*/ ScopedConnection(Connection const& connection) : Connection(connection)
        {
        }

        /// Construct a scoped connection from a movable connection.
        ///
        /// \param connection The connection to manage.
        /// \note The ScopedConnection will automatically disconnect the underlying slot when it is destroyed.
        /*implicit*/ ScopedConnection(Connection&& connection) noexcept : Connection(std::move(connection))
        {
        }

        /// Destructor for the scoped connection. Automatically disconnects the underlying slot.
        ~ScopedConnection() override
        {
            disconnect();
        }

        /// Deleted copy constructor and copy assignment operator to prevent copying of scoped connections.
        ScopedConnection(ScopedConnection const&)                    = delete;

        /// Deleted copy assignment operator to prevent copying of scoped connections.
        auto operator=(ScopedConnection const&) -> ScopedConnection& = delete;

        /// Move constructor for the scoped connection.
        ///
        /// \param other The scoped connection to move.
        ScopedConnection(ScopedConnection&& other) noexcept : Connection(std::move(other._state))
        {
        }

        /// Move assignment operator for the scoped connection.
        ///
        /// \param other The scoped connection to move.
        /// \return A reference to this scoped connection after the assignment.
        auto operator=(ScopedConnection&& other) noexcept -> ScopedConnection&
        {
            if (this != &other)
            {
                disconnect();
                _state = std::move(other._state);
            }
            return *this;
        }
    };

    /// Base class for intrusive lifetime tracking of objects connected to a Signal by raw pointer.
    /// Deriving from Observer makes the signal automatically disconnect that slot once
    /// disconnectAll() runs, which derived classes should call from their own destructor.
    ///
    /// This is an alternative to tracking lifetime through std::shared_ptr/std::weak_ptr, for
    /// objects that are not managed that way.
    class Observer
    {
    public:
        /// Destructor for the observer. Ensures that all connected slots are disconnected.
        virtual ~Observer() = default;

    protected:
        /// Disconnect all slots connected to this object.
        ///
        /// To avoid invoking a slot on a semi-destructed instance, derived classes should call
        /// this method in their destructor, before their own state becomes invalid.
        ///
        /// \note This method is thread-safe.
        void disconnectAll()
        {
            std::scoped_lock const lock(_mutex);
            _connections.clear();
        }

    private:
        template <typename...>
        friend class Signal;

        void addConnection(Connection connection)
        {
            std::scoped_lock const lock(_mutex);
            _connections.emplace_back(std::move(connection));
        }

        std::mutex                    _mutex;
        std::vector<ScopedConnection> _connections;
    };

    /// An implementation of the observer pattern for member function slots.
    ///
    /// A Signal holds a list of slots, each a member function bound to an object, and calls them
    /// all in connection order whenever it is emitted. Safety: connection, disconnection and
    /// emission are all thread-safe.
    ///
    /// \tparam T The argument types of the signal and its slots.
    template <typename... T>
    class Signal final : private detail::Cleanable
    {
        using SlotBase = detail::SlotBase<T...>;
        using SlotPtr  = std::shared_ptr<SlotBase>;

    public:
        /// Default constructor for the signal.
        ///
        /// \note The signal starts in an unblocked state with no connected slots.
        Signal() = default;

        /// Destructor for the signal. Disconnects all connected slots.
        ///
        /// \note This method is thread-safe.
        ~Signal() override
        {
            disconnectAll();
        }

        /// Deleted copy constructor and copy assignment operator to prevent copying of signals.
        Signal(Signal const&)                    = delete;

        /// Deleted copy assignment operator to prevent copying of signals.
        ///
        /// \return A reference to this signal after the assignment.
        auto operator=(Signal const&) -> Signal& = delete;

        /// Deleted move constructor and move assignment operator to prevent moving of signals.
        Signal(Signal&&)                         = delete;

        /// Deleted move assignment operator to prevent moving of signals.
        ///
        /// \return A reference to this signal after the assignment.
        auto operator=(Signal&&) -> Signal&      = delete;

        /// Emit the signal: every connected, unblocked slot is called with the supplied arguments.
        /// \param args The arguments to emit.
        template <typename... U>
        void operator()(U&&... args) const
        {
            if (_blocked)
            {
                return;
            }

            std::vector<SlotPtr> slots;
            {
                std::scoped_lock const lock(_mutex);
                slots = _slots;
            }

            for (auto const& slot : slots)
            {
                (*slot)(args...);
            }
        }

        /// Connect a member function to an object referenced by a pointer, or a pointer-like type
        /// such as std::unique_ptr (anything std::to_address() accepts).
        ///
        /// If the pointee derives from Observer, the connection is registered with it so that it
        /// disconnects this slot itself once destroyed. Otherwise, no lifetime tracking is
        /// performed: the caller must ensure the object outlives the connection, or disconnect it
        /// manually (e.g. via the returned Connection).
        ///
        /// \param pmf A pointer to the member function to call.
        /// \param ptr A pointer, or pointer-like object, to the object the member function should be called on.
        /// \return A connection object that can be used to interact with the slot.
        template <typename Pmf, typename Ptr>
        requires std::is_member_function_pointer_v<Pmf> && (!detail::isTrackedPointer<std::remove_cvref_t<Ptr>>)
        auto connect(Pmf pmf, Ptr&& ptr) -> Connection
        {
            using Pointee = std::remove_reference_t<decltype(*std::to_address(ptr))>;

            auto       slot       = std::make_shared<detail::Slot<Pmf, Pointee, T...>>(cleaner(), pmf, std::to_address(ptr));
            Connection connection = addSlot(std::move(slot));

            if constexpr (std::derived_from<Pointee, Observer>)
            {
                std::to_address(ptr)->addConnection(connection);
            }

            return connection;
        }

        /// Connect a member function to an object tracked by std::shared_ptr. The slot is
        /// automatically skipped, and lazily disconnected, once the object has expired.
        ///
        /// \param pmf A pointer to the member function to call.
        /// \param ptr A shared pointer to the object the member function should be called on.
        /// \return A connection object that can be used to interact with the slot.
        template <typename Pmf, typename U>
        requires std::is_member_function_pointer_v<Pmf>
        auto connect(Pmf pmf, std::shared_ptr<U> const& ptr) -> Connection
        {
            return connect(pmf, std::weak_ptr<U>(ptr));
        }

        /// Connect a member function to an object tracked by std::weak_ptr. The slot is
        /// automatically skipped, and lazily disconnected, once the object has expired.
        ///
        /// \param pmf A pointer to the member function to call.
        /// \param ptr A weak pointer to the object the member function should be called on.
        /// \return A connection object that can be used to interact with the slot.
        template <typename Pmf, typename U>
        requires std::is_member_function_pointer_v<Pmf>
        auto connect(Pmf pmf, std::weak_ptr<U> ptr) -> Connection
        {
            auto slot = std::make_shared<detail::TrackedSlot<Pmf, U, T...>>(cleaner(), pmf, std::move(ptr));
            return addSlot(std::move(slot));
        }

        /// Connect a member function, returning a ScopedConnection that disconnects the slot
        /// automatically once it goes out of scope.
        /// \param args The arguments to forward to connect().
        /// \return A scoped connection object that disconnects the slot upon destruction.
        template <typename... Args>
        auto connectScoped(Args&&... args) -> ScopedConnection
        {
            return connect(std::forward<Args>(args)...);
        }

        /// Disconnect all slots connected to this signal.
        void disconnectAll()
        {
            std::scoped_lock const lock(_mutex);
            _slots.clear();
        }

        /// Block signal emission: connected slots are retained but skipped until unblock() is called.
        void block() noexcept
        {
            _blocked = true;
        }

        /// Unblock signal emission.
        void unblock() noexcept
        {
            _blocked = false;
        }

        /// Determine whether signal emission is currently blocked.
        /// \return True if signal emission is currently blocked, false otherwise.
        [[nodiscard]] auto blocked() const noexcept -> bool
        {
            return _blocked;
        }

        /// Get the number of connected slots.
        /// \return The number of slots currently connected to this signal.
        [[nodiscard]] auto slotCount() const -> std::size_t
        {
            std::scoped_lock const lock(_mutex);
            return _slots.size();
        }

    private:
        /// Access this signal as a detail::Cleanable, for use by connect() overloads that build a
        /// slot outside of a Signal member function (e.g. via std::make_shared), where the private
        /// base class conversion would otherwise be inaccessible.
        auto cleaner() -> detail::Cleanable&
        {
            return *this;
        }

        void clean(detail::SlotState* state) override
        {
            std::scoped_lock const lock(_mutex);
            auto const             index = state->index();
            if (index < _slots.size() && _slots[index].get() == state)
            {
                std::swap(_slots[index], _slots.back());
                _slots[index]->setIndex(index);
                _slots.pop_back();
            }
        }

        auto addSlot(SlotPtr slot) -> Connection
        {
            std::weak_ptr<detail::SlotState> state = slot;

            std::scoped_lock const lock(_mutex);
            slot->setIndex(_slots.size());
            _slots.push_back(std::move(slot));

            return Connection(std::move(state));
        }

    private:
        mutable std::mutex   _mutex;
        std::vector<SlotPtr> _slots;
        std::atomic<bool>    _blocked{ false };
    };
} // namespace aw::core
