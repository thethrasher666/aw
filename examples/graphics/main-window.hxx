//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/vulkan-widgets/vulkan-widget.hxx>
#include <aw/widgets/main-window.hxx>
#include <aw/widgets/layout.hxx>

namespace aw::examples
{
    class ExampleGraphicsWindow final : public aw::widgets::MainWindow
    {
    public:
        ExampleGraphicsWindow();

    private:
        aw::widgets::VerticalLayout* _layout{};
        aw::vulkan::VulkanWidget*    _vulkanWidget{};
    };
} // namespace aw::examples
