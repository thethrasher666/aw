//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/object.hxx>
#include <aw/core/rect.hxx>
#include <aw/gui/window.hxx>
#include <aw/widgets/api.hxx>

#include <memory>
#include <string>
#include <string_view>

namespace aw::widgets
{
    class Layout; ///< Forward declaration.

    /// Base class for all widgets.
    class Widget : public core::Object, private gui::WindowDelegate
    {
    public:
        /// Constructor.
        /// \param parent The parent object of this widget.
        /// \param flags The flags for this widget window.
        AW_WIDGETS_API explicit Widget(Widget* parent = nullptr, gui::WindowFlags flags = gui::WindowFlags::Widget);

        /// Destructor.
        AW_WIDGETS_API ~Widget() override;

        /// Get the frame height of this widget.
        /// \return The frame height of this widget in pixels.
        [[nodiscard]] auto height() const -> int32_t
        {
            return _geometry.height();
        }

        /// Hide the widget. Also hides the widgets managed by this widget's layout, if any.
        AW_WIDGETS_API void hide();

        /// Set the geometry of this widget.
        /// \param geometry The new geometry, in parent-relative coordinates.
        AW_WIDGETS_API void setGeometry(core::Rect const& geometry);

        /// Set the layout of this widget, taking ownership of it.
        /// \param layout The new layout; any previously installed layout is destroyed.
        AW_WIDGETS_API void setLayout(std::unique_ptr<Layout> layout);

        /// Set the title of this widget.
        /// \param title The new title of this widget.
        AW_WIDGETS_API void setTitle(std::string const& title);

        /// Set the icon of this window.
        /// \param icon The new icon of this window.
        AW_WIDGETS_API void setWindowIcon(gui::Icon const& icon);

        /// Set the presentation state of this widget.
        /// \param state The new window state.
        AW_WIDGETS_API void setWindowState(gui::WindowState state);

        /// Show the widget. Also shows the widgets managed by this widget's layout, if any.
        AW_WIDGETS_API void show();

        /// Update the widget. The base implementation recurses into the widgets managed by this
        /// widget's layout, if any; overrides that do their own work should call Widget::update()
        /// to keep that propagation.
        AW_WIDGETS_API virtual void update();

        /// Get the frame width of this widget.
        /// \return The frame width of this widget in pixels.
        [[nodiscard]] auto width() const -> int32_t
        {
            return _geometry.width();
        }

        /// Get the window associated with this widget, if any.
        /// \return The window associated with this widget, or nullptr if there is no window.
        [[nodiscard]] auto window() -> gui::Window*
        {
            return _window.get();
        }

    protected:
        /// Handles close requests from the native platform.
        AW_WIDGETS_API void closeEvent(core::CloseEvent& event) override;

        /// Generic event entry point.
        /// \return true if the event was handled.
        [[nodiscard]] AW_WIDGETS_API auto event(core::Event& event) -> bool override;

        /// Invoked when the native platform requests this widget to close.
        /// \return true to accept the close and destroy the native window; false to ignore it.
        [[nodiscard]] AW_WIDGETS_API auto onCloseRequested() -> bool override;

        /// Invoked when the native geometry of this widget changes.
        AW_WIDGETS_API void onGeometryChanged(core::Rect const& geometry) override;

        /// Invoked when the native platform window has been destroyed.
        AW_WIDGETS_API void onNativeWindowDestroyed() override;

    private:
        core::Rect                   _geometry;                          ///< The geometry of this widget.
        gui::WindowFlags             _flags{ gui::WindowFlags::Widget }; ///< The window behavior flags.
        Layout*                      _layout{};                          ///< The layout managing this widget's children, if any.
        Widget*                      _parent{};                          ///< The parent widget, if any.
        std::unique_ptr<gui::Window> _window;                            ///< Optional backing window.
    };
} // namespace aw::widgets
