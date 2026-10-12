//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "args.hxx"
#include "diagnosticx.hxx"
#include "generator.hxx"
#include "manifest.hxx"

#include <fstream>
#include <print>
#include <toml++/toml.hpp>

auto main(int argc, char* argv[]) -> int32_t
{
    aw::linguist::CliOptions options;
    if (!aw::linguist::parseArgs(argc, argv, options))
    {
        return EXIT_FAILURE;
    }

    if (options.help)
    {
        aw::linguist::printUsage(argv[0]);
        return EXIT_SUCCESS;
    }

    try
    {
        auto const manifest = aw::linguist::parseManifest(options.input);
        auto const contents = aw::linguist::generate(manifest);
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
                aw::linguist::logError("cannot open", options.output.generic_string());
                return EXIT_FAILURE;
            }

            file << *contents;
            file.close();
        }
    }
    catch (toml::parse_error const& e)
    {
        aw::linguist::logError(options.input.string() + ":", e.description());
        return EXIT_FAILURE;
    }
    catch (std::exception const& e)
    {
        aw::linguist::logError(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
