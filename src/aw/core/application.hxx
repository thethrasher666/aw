//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/locale.hxx>
#include <aw/core/object.hxx>
#include <aw/core/thread.hxx>

#include <memory>
#include <string>
#include <vector>

namespace aw::core
{
    namespace platform
    {
        class PlatformApplication; ///< Forward declaration.
    }

    /// The CoreApplication class provides an event loop for applications without an interface.
    class CoreApplication : public Object
    {
    public:
        /// Constructor.
        /// \param arguments The command line arguments passed to the application.
        AW_CORE_API explicit CoreApplication(std::vector<std::string> const& arguments);

        /// Destructor.
        AW_CORE_API virtual ~CoreApplication();

        /// Disable copying.
        CoreApplication(CoreApplication const&) = delete;

        /// Disable assingment.
        auto operator=(CoreApplication const&) -> CoreApplication& = delete;

        /// Disable moving.
        CoreApplication(CoreApplication&&) = delete;

        /// Disable move assignment.
        auto operator=(CoreApplication&&) -> CoreApplication& = delete;

        /// Get the command line arguments passed to the application.
        /// \return A const reference to the vector of command line arguments.
        [[nodiscard]] auto arguments() const -> std::vector<std::string> const&
        {
            return _arguments;
        }

        /// Get the singleton instance of the application.
        /// \return A pointer to the singleton instance of the application if one has been allocated; null pointer otherwise.
        [[nodiscard]] AW_CORE_API static auto instance() -> CoreApplication*;

        /// Determine whether the application drives a frame after every pass over the event queue.
        /// \return true if continuous updates are enabled.
        [[nodiscard]] auto isContinuousUpdateEnabled() const -> bool
        {
            return _continuousUpdate;
        }

        /// Get the locale of the application.
        /// \return A const reference to the locale of the application.
        [[nodiscard]] auto locale() const -> Locale const&
        {
            return _locale;
        }

        /// Run the application.
        /// \return true if the application ran successfully, false otherwise.
        [[nodiscard]] AW_CORE_API auto run() -> bool;

        /// Enable or disable continuous updates. When enabled the run loop stops blocking on the
        /// native event queue and calls onFrame() once per iteration, which is what a renderer needs.
        /// \param enabled true to drive a frame every iteration, false to idle until the next event.
        void setContinuousUpdateEnabled(bool enabled)
        {
            _continuousUpdate = enabled;
        }

        /// Quit the application.
        AW_CORE_API void quit();

    protected:
        /// Called before the application starts running.
        virtual void onBeforeRun()
        {
        }

        /// Called after the application has finished running.
        virtual void onAfterRun()
        {
        }

        /// Called once per run-loop iteration while continuous updates are enabled.
        virtual void onFrame()
        {
        }

    private:
        std::vector<std::string> const                 _arguments;          ///< The command line arguments passed to the application.
        bool                                           _continuousUpdate{}; ///< Whether the run loop drives a frame every iteration.
        Locale                                         _locale;             ///< The locale of the application.
        std::unique_ptr<platform::PlatformApplication> _platform;           ///< The platform-specific implementation of the application.
        Thread*                                        _thread{};           ///< Implicit main thread, owned as a child of this object.
    };
} // namespace aw::core
