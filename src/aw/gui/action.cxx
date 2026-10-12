//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/gui/action.hxx"

namespace aw::gui
{
    Action::Action(core::Object* parent) : Object(parent)
    {
    }

    Action::Action(std::string const& text, core::Object* parent) : Object(parent), _text(text)
    {
        _ID = ActionRegistry::instance()->nextID();
        ActionRegistry::instance()->registerAction(this);
    }

    Action::Action(Icon const& /*icon*/, std::string const& text, core::Object* parent) : Object(parent), _text(text)
    {
        _ID = ActionRegistry::instance()->nextID();
        ActionRegistry::instance()->registerAction(this);
    }

    void Action::activate()
    {
        triggered();
    }

    auto ActionRegistry::findActionByID(uint32_t const ID) -> Action*
    {
        if (_actionsByID.contains(ID))
        {
            return _actionsByID[ID];
        }

        return nullptr;
    }

    auto ActionRegistry::instance() -> ActionRegistry*
    {
        static ActionRegistry instance;
        return &instance;
    }

    void ActionRegistry::registerAction(Action* action)
    {
        if (!_actionsByID.contains(action->ID()))
        {
            _actionsByID[action->ID()] = action;
            ++_currentID;
        }
    }
} // namespace aw::gui
