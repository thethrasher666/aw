//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "main-window.hxx"

namespace aw::examples
{
    ExampleGraphicsWindow::ExampleGraphicsWindow() : aw::widgets::MainWindow(nullptr)
    {
        setObjectName("ExampleGraphicsWindow");
        setTitle(tr("title"));
        setWindowIcon(aw::gui::Icon(":/project"));

        auto vulkanWidget = std::make_unique<aw::vulkan::VulkanWidget>(std::make_unique<aw::vulkan::VulkanContext>(), this);
        _vulkanWidget = vulkanWidget.get();

        auto layout = std::make_unique<aw::widgets::VerticalLayout>();
        _layout = layout.get();
        _layout->addWidget(std::move(vulkanWidget));

        setLayout(std::move(layout));
    }
} // namespace aw::examples
