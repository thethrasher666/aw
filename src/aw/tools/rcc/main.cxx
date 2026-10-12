//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "args.hxx"
#include "diagnosticx.hxx"
#include "generator.hxx"
#include "manifest.hxx"

#include <toml++/toml.hpp>

auto main(int argc, char* argv[]) -> int32_t
{
    aw::rcc::CliOptions options;
    if (!aw::rcc::parseArgs(argc, argv, options))
    {
        return EXIT_FAILURE;
    }

    if (options.help)
    {
        aw::rcc::printUsage(argv[0]);
        return EXIT_SUCCESS;
    }

    try
    {
        auto const manifest = aw::rcc::parseManifest(options.input);
        auto const baseDir = options.input.parent_path();
        auto const contents = aw::rcc::generate(manifest, baseDir, options.language);
        if (!contents)
        {
            return EXIT_FAILURE;
        }

        if (options.output.empty())
        {
            std::print("{}", *contents);
        }
        else
        {
            std::ofstream file(options.output);
            if (!file)
            {
                aw::rcc::logError("cannot open", options.output.generic_string());
                return EXIT_FAILURE;
            }

            file << *contents;
            file.close();
        }
    }
    catch (toml::parse_error const& e)
    {
        aw::rcc::logError(options.input.string() + ":", e.description());
        return EXIT_FAILURE;
    }
    catch (std::exception const& e)
    {
        aw::rcc::logError(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
