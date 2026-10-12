//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>

namespace aw::core
{
    /// Enumeration of event types
    enum class EventType
    {
        None,        ///< No event type
        CloseWindow, ///< Window close event
    };

    /// Base class for events
    class Event
    {
    public:
        /// Constructor.
        /// \param type The type of the event.
        explicit Event(EventType type) : _type(type)
        {
        }

        /// Get the type of the event
        /// \return The type of the event
        [[nodiscard]] auto type() const -> EventType
        {
            return _type;
        }

        /// Accept this event.
        void accept()
        {
            _accepted = true;
        }

        /// Ignore this event.
        void ignore()
        {
            _accepted = false;
        }

        /// Check whether this event is accepted.
        [[nodiscard]] auto isAccepted() const -> bool
        {
            return _accepted;
        }

    private:
        EventType _type{};           ///< The type of the event.
        bool      _accepted{ true }; ///< Whether the event is accepted.
    };

    /// Framework close event for windows.
    class CloseEvent : public Event
    {
    public:
        CloseEvent() : Event(EventType::CloseWindow)
        {
        }
    };
} // namespace aw::core
