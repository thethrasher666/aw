//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/widgets/main-window.hxx>
#include <aw/widgets/menu.hxx>

#include <memory>

namespace aw::examples
{
    class ExampleWindow final : public aw::widgets::MainWindow
    {
    public:
        ExampleWindow();

    private:
        void onFileExit();

    private:
        gui::Action* _exitAction{};
    };
} // namespace aw::examples
