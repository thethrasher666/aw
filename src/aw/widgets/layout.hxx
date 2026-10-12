//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/object.hxx>
#include <aw/widgets/widget.hxx>

#include <memory>
#include <vector>

namespace aw::widgets
{
    /// The base class of all geometry managers.
    /// A layout owns the widgets added to it, and is itself owned by the widget it manages.
    class Layout : public core::Object
    {
    public:
        /// Constructor.
        AW_WIDGETS_API Layout();

        /// Adds a widget to the layout, taking ownership of it.
        /// \param widget The widget to add to the layout.
        /// \return A non-owning pointer to the added widget.
        AW_WIDGETS_API auto addWidget(std::unique_ptr<Widget> widget) -> Widget*;

        /// Recomputes and applies the geometry of all widgets managed by this layout.
        AW_WIDGETS_API virtual void update();

    protected:
        /// Get the parent widget that owns this layout.
        /// \return The parent widget of this layout, or nullptr if it has none.
        [[nodiscard]] AW_WIDGETS_API auto parentWidget() const -> Widget*
        {
            return _parent;
        }

        /// Get the widgets managed by this layout.
        /// \return The widgets managed by this layout.
        [[nodiscard]] AW_WIDGETS_API auto widgets() const -> std::vector<Widget*> const&
        {
            return _widgets;
        }

    private:
        friend class Widget; ///< Widget::setLayout() attaches the layout to the widget it manages.

        Widget*              _parent{}; ///< The parent widget of this layout.
        std::vector<Widget*> _widgets;  ///< The widgets managed by this layout, owned as children of this object.
    };

    /// Lines up widgets horizontally.
    class HorizontalLayout : public Layout
    {
    public:
        /// Constructor.
        AW_WIDGETS_API HorizontalLayout();

        /// Recomputes and applies the geometry of all widgets managed by this layout.
        AW_WIDGETS_API void update() override;
    };

    /// Lines up widgets vertically.
    class VerticalLayout : public Layout
    {
    public:
        /// Constructor.
        AW_WIDGETS_API VerticalLayout();

        /// Recomputes and applies the geometry of all widgets managed by this layout.
        AW_WIDGETS_API void update() override;
    };
} // namespace aw::widgets
