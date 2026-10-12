//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vector>
#include <aw/core/event-dispatcher.hxx>
#include <aw/gui/screen.hxx>

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

        /// Processes macOS AppKit events for the current thread.
        auto processEvents(EventLoopFlags flags) -> bool override;

        /// Get the list of available screens.
        /// \return A vector of Screen objects representing the available screens.
        [[nodiscard]] static auto screens() -> std::vector<Screen>;
    };
} // namespace aw::gui::platform
