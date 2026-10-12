//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/event-dispatcher.hxx>
#include <aw/gui/screen.hxx>

#include <xcb/xcb.h>

#include <vector>

namespace aw::gui::platform
{
    /// The platform-specific implementation of the GuiApplication class.
    class GuiApplicationPlatform : public EventDispatcher
    {
    public:
        /// Constructor.
        explicit GuiApplicationPlatform();

        /// Destructor.
        ~GuiApplicationPlatform() override;

        /// Processes XCB events for the current thread.
        auto processEvents(EventLoopFlags flags) -> bool override;

        /// Get the shared XCB connection used by the GUI event loop.
        [[nodiscard]] static auto connection() -> xcb_connection_t*;

        /// Get the list of available screens.
        /// \return A vector of Screen objects representing the available screens.
        [[nodiscard]] static auto screens() -> std::vector<Screen>;

    private:
        xcb_connection_t* _connection{}; ///< The XCB connection used for event processing.
    };
} // namespace aw::gui::platform
