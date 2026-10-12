//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/rect.hxx>
#include <aw/gui/api.hxx>

#include <string>

namespace aw::gui
{
    /// Represents a screen in the system.
    class Screen final
    {
    public:
        /// Constructor.
        /// \param geometry The geometry of the screen in pixels.
        /// \param name The name of the screen.
        explicit Screen(core::Rect const& geometry, std::string const& name) : _geometry(geometry), _name(name)
        {
        }

        /// Get the geometry of the screen in pixels.
        /// \return The geometry of the screen in pixels.
        [[nodiscard]] AW_GUI_API auto geometry() const -> core::Rect;

        /// Get the name of the screen.
        /// \return The name of the screen.
        [[nodiscard]] auto name() const -> std::string const&
        {
            return _name;
        }

    private:
        core::Rect  _geometry; ///< The rectangular dimensions of the screen in pixels.
        std::string _name;     ///< The name of the screen.
    };
} // namespace aw::gui
