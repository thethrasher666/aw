//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "main-window.hxx"

#include <aw/core/application.hxx>

namespace aw::examples
{
    ExampleWindow::ExampleWindow()
    {
        setObjectName("ExampleWindow");
        setTitle(tr("title"));
        setWindowIcon(aw::gui::Icon(":/project"));

        if (auto* menubar = setMenuBar(std::make_unique<widgets::MenuBar>()); menubar)
        {
            if (auto* fileMenu = menubar->addMenu(std::make_unique<widgets::Menu>(tr("File"))); fileMenu)
            {
                _exitAction = fileMenu->addAction(tr("Exit"));
                _exitAction->triggered.connect(&ExampleWindow::onFileExit, this);
            }
        }
    }

    void ExampleWindow::onFileExit()
    {
        core::CoreApplication::instance()->quit();
    }
} // namespace aw::examples
