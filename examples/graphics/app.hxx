//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "main-window.hxx"

#include <aw/widgets/application.hxx>

#include <memory>

namespace aw::examples
{
    class ExampleApplication final : public aw::widgets::WidgetsApplication
    {
    public:
        ExampleApplication(std::vector<std::string> const& arguments);

    protected:
        void onBeforeRun() final;
        void onAfterRun() final;
        void onFrame() final;

    private:
        std::unique_ptr<ExampleGraphicsWindow> _mainWindow;
    };
} // namespace aw::examples
