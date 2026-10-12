//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "generator.hxx"

#include <filesystem>
#include <string_view>

namespace aw::rcc
{
    /// Represents the command line options for the resource compiler.
    struct CliOptions
    {
        bool                  help = false;             ///< Indicates whether the help option was specified.
        Language              language = Language::Cpp; ///< The generator selected with --generator.
        std::filesystem::path output;                   ///< The file the generated code is written to; stdout when unset.
        std::filesystem::path input;                    ///< The manifest file to compile.
    };

    /// Parses the command line arguments and populates the provided CliOptions structure.
    /// \param argc The number of command line arguments.
    /// \param argv An array of command line argument strings.
    /// \param options A reference to a CliOptions structure that will be populated with the parsed options.
    /// \return True if the arguments were successfully parsed; false otherwise.
    auto parseArgs(int argc, char* argv[], CliOptions& options) -> bool;

    /// Prints the usage information for the resource compiler.
    /// \param executable The name of the executable to display in the usage information.
    void printUsage(std::string_view executable);
} // namespace aw::rcc
