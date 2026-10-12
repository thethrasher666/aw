//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "app.hxx"
#include "main-window.hxx"

namespace aw::examples
{
    ExampleApplication::ExampleApplication(std::vector<std::string> const& arguments) : WidgetsApplication(arguments)
    {
    }

    void ExampleApplication::onBeforeRun()
    {
        _mainWindow = std::make_unique<ExampleGraphicsWindow>();
        _mainWindow->show();

        setContinuousUpdateEnabled(true);
    }

    void ExampleApplication::onFrame()
    {
        _mainWindow->update();
    }

    void ExampleApplication::onAfterRun()
    {
        setContinuousUpdateEnabled(false);
        _mainWindow.reset();
    }
} // namespace aw::examples
