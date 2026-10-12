//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/widgets/layout.hxx"

namespace aw::widgets
{
    Layout::Layout()
    {
        setObjectName("Layout");
    }

    auto Layout::addWidget(std::unique_ptr<Widget> widget) -> Widget*
    {
        auto* const added = addChild(std::move(widget));

        if (!added)
        {
            return nullptr;
        }

        _widgets.push_back(added);
        update();

        return added;
    }

    void Layout::update()
    {
    }

    HorizontalLayout::HorizontalLayout()
    {
        setObjectName("HorizontalLayout");
    }

    void HorizontalLayout::update()
    {
        Widget* container = parentWidget();

        if (!container)
        {
            return;
        }

        int32_t const width = widgets().empty() ? 0 : container->width() / static_cast<int32_t>(widgets().size());

        int32_t x = 0;

        for (Widget* widget : widgets())
        {
            widget->setGeometry(core::Rect{ x, 0, width, container->height() });
            x += width;
        }
    }

    VerticalLayout::VerticalLayout()
    {
        setObjectName("VerticalLayout");
    }

    void VerticalLayout::update()
    {
        Widget* container = parentWidget();

        if (!container)
        {
            return;
        }

        int32_t const height = widgets().empty() ? 0 : container->height() / static_cast<int32_t>(widgets().size());

        int32_t y = 0;

        for (Widget* widget : widgets())
        {
            widget->setGeometry(core::Rect{ 0, y, container->width(), height });
            y += height;
        }
    }
} // namespace aw::widgets
