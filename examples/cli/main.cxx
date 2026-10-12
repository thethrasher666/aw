//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <aw/core/application.hxx>
#include <memory>

auto main(int32_t argc, char* argv[]) -> int
{
    std::vector<std::string> arguments(argv, argv + argc);
    auto                     app = std::make_unique<aw::core::CoreApplication>(arguments);

    return app->run() ? 0: -1;
}
