//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/gui/application.hxx>
#include <aw/widgets/api.hxx>

namespace aw::widgets
{
    /// The WidgetsApplication class manages widgets, styles and other GUI elements for the application.
    class WidgetsApplication : public gui::GuiApplication
    {
    public:
        /// Constructor.
        /// \param arguments The command line arguments passed to the application.
        AW_WIDGETS_API explicit WidgetsApplication(std::vector<std::string> const& arguments);
    };
} // namespace aw::widgets
