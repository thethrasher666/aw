//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "app.hxx"
#include "entry-point.hxx"

namespace aw::examples
{
    auto entry_point(std::vector<std::string> const& arguments) -> bool
    {
        auto app = std::make_unique<ExampleApplication>(arguments);

        return app->run();
    }
} // namespace aw::examples
