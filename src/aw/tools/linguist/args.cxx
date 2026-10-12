//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "args.hxx"
#include "diagnosticx.hxx"

#include <print>

namespace aw::linguist
{
    void printUsage(std::string_view executable)
    {
        std::print(
            "Usage: {} [options] <input>\n"
            "String compiler.\n\n"
            "Options:\n"
            "  --help                         Display help.\n"
            "  --output <file>                Write the generated code to <file>; defaults to stdout.\n",
            executable);
    }

    auto parseArgs(int argc, char* argv[], CliOptions& options) -> bool
    {
        for (int i = 1; i < argc; ++i)
        {
            auto const arg = std::string_view(argv[i]);

            auto requireValue = [&](std::string_view flag) -> char const*
            {
                if (i + 1 >= argc)
                {
                    logError("missing argument for", flag);
                    return nullptr;
                }
                return argv[++i];
            };

            if (arg == "--help")
            {
                options.help = true;
            }
            else if (arg == "--output")
            {
                auto const* value = requireValue(arg);
                if (!value)
                {
                    return false;
                }
                options.output = value;
            }
            else if (!arg.empty() && arg.front() == '-')
            {
                logError("unknown option", arg);
                return false;
            }
            else
            {
                if (!options.input.empty())
                {
                    logError("only one input file may be specified");
                    return false;
                }
                options.input = argv[i];
            }
        }

        if (options.input.empty())
        {
            printUsage(argv[0]);
            return false;
        }

        return true;
    }
} // namespace aw::linguist
