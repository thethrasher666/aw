//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "entry-point.hxx"
#include "main-window.hxx"

#include <aw/widgets/application.hxx>

namespace aw::examples
{
    auto entry_point(std::vector<std::string> const& arguments) -> bool
    {
        auto app = std::make_unique<aw::widgets::WidgetsApplication>(arguments);

        auto mainWindow = std::make_unique<ExampleWindow>();

        mainWindow->show();

        return app->run();
    }
} // namespace aw::examples
