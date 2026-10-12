//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/application.hxx>
#include <aw/gui/screen.hxx>

#include <memory>
#include <unordered_set>
#include <vector>

namespace aw::gui
{
    namespace platform
    {
        class GuiApplicationPlatform; ///< Forward declaration.
    }
    class Window; ///< Forward declaration.

    /// The GuiApplication class manages windows, screens and input events for an application with an interface.
    class GuiApplication : public core::CoreApplication
    {
    public:
        /// Constructor.
        /// \param arguments The command line arguments passed to the application.
        AW_GUI_API explicit GuiApplication(std::vector<std::string> const& arguments);

        /// Destructor.
        AW_GUI_API virtual ~GuiApplication();

        /// Get the singleton instance as a GuiApplication, if available.
        [[nodiscard]] AW_GUI_API static auto instance() -> GuiApplication*;

        /// Notify that a top-level window has been destroyed.
        /// \param window The top-level window that has been destroyed.
        AW_GUI_API void notifyTopLevelWindowClosed(Window* window);

        /// Register a visible top-level window.
        /// \param window The top-level window to register.
        AW_GUI_API void registerTopLevelWindow(Window* window);

        /// Get the list of available screens.
        /// \return A vector of Screen objects representing the available screens.
        [[nodiscard]] AW_GUI_API static auto screens() -> std::vector<Screen>;

    private:
        std::unique_ptr<platform::GuiApplicationPlatform> _platform;
        std::unordered_set<Window*>                       _topLevelWindows;
        bool                                              _quitOnLastWindowClosed{ true };
    };
} // namespace aw::gui
