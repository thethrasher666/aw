//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/object.hxx>
#include <aw/gui/icon.hxx>

#include <unordered_map>

namespace aw::gui
{
    /// This class provides an abstraction for user commands that can be added to different user interface components.
    class Action : public core::Object
    {
    public:
        core::Signal<> triggered;

    public:
        /// Constructor.
        /// \param parent The parent object, if any.
        AW_GUI_API explicit Action(core::Object* parent = nullptr);

        /// Constructor.
        /// \param text The text.
        /// \param parent The parent object, if any.
        AW_GUI_API explicit Action(std::string const& text, core::Object* parent = nullptr);

        /// Constructor.
        /// \param icon The icon.
        /// \param text The text.
        /// \param parent The parent object, if any.
        AW_GUI_API explicit Action(Icon const& icon, std::string const& text, core::Object* parent = nullptr);

        /// Activate the action.
        AW_GUI_API void activate();

        /// Get the text.
        /// \return A valid object.
        [[nodiscard]] auto ID() -> uint32_t const
        {
            return _ID;
        }

        /// Get the text.
        /// \return A valid object.
        [[nodiscard]] auto text() -> std::string const&
        {
            return _text;
        }

    private:
        uint32_t          _ID{}; ///< The ID.
        std::string const _text; ///< The text.
    };

    /// A singleton registry for actions.
    class ActionRegistry final
    {
    public:
        /// Destructor.
        ~ActionRegistry() = default;

        /// Find's a registered action by its ID.
        /// \param ID The ID of the action.
        /// \return An action, if found; nothing otherwise.
        [[nodiscard]] auto findActionByID(uint32_t const ID) -> Action*;

        /// Get the instance.
        /// \return A valid object;
        [[nodiscard]] static auto instance() -> ActionRegistry*;

        /// Get the next ID.
        /// \return A valid integer.
        [[nodiscard]] auto nextID() const -> uint32_t
        {
            return _currentID + 1;
        }

        /// Register an action.
        /// \param action The action.
        void registerAction(Action* action);

    private:
        ActionRegistry() = default;

    private:
        uint32_t _currentID{ 1000 }; ///< The current ID;
        std::unordered_map<uint32_t, Action*> _actionsByID;
    };
} // namespace aw::gui
