//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>

#include <concepts>
#include <format>
#include <source_location>
#include <type_traits>

namespace aw::core
{
    enum class MessageType
    {
        Debug,    ///< A message used for debugging purposes and will not be shown in production builds.
        Info,     ///< An Informational message, not an error or warning.
        Warning,  ///< A warning message, indicating a potential issue that may not prevent the program from running.
        Critical, ///< A critical message, indicating a problem that may prevent the program from running correctly.
        Fatal     ///< A fatal error message, indicating a critical problem that will halt the program's execution.
    };

    /// Logs a message with the specified message type.
    /// \param type The type of the message (Debug, Info, Warning, Error, Fatal).
    /// \param message The message to be logged.
    AW_CORE_API void logMessage(MessageType type, std::string const& message);

} // namespace aw::core

/// A format string that also captures the location of its call site.
/// \tparam ...Args The types of the format arguments.
template <typename... Args>
struct AwFmtStrWithSrcLoc
{
    std::format_string<Args...> format;   ///< The compile-time checked format string.
    std::source_location        location; ///< The location the format string was supplied from.

    /// Implicitly captures the call site alongside the format string.
    /// \param fmt The format string, converted the same way std::format_string accepts one.
    /// \param loc The call site; defaults to the location of the argument expression itself.
    template <typename T>
    requires std::convertible_to<T const&, std::format_string<Args...>> consteval AwFmtStrWithSrcLoc(T const& fmt, std::source_location loc = std::source_location::current())
        : format(fmt), location(loc)
    {
    }
};

/// Helper function to log a debug message, automatically tagged with its call site.
/// \tparam ...Args The types of the format arguments.
/// \param fmt The format string and its call site.
/// \param ...args The format arguments.
template <typename... Args>
void awLogDebug(AwFmtStrWithSrcLoc<std::type_identity_t<Args>...> fmt, Args&&... args)
{
    auto const message = std::format(fmt.format, std::forward<Args>(args)...);
    aw::core::logMessage(aw::core::MessageType::Debug, std::format("{}({}): {}", fmt.location.file_name(), fmt.location.line(), message));
}

/// Helper function to log an info message, automatically tagged with its call site.
/// \tparam ...Args The types of the format arguments.
/// \param fmt The format string and its call site.
/// \param ...args The format arguments.
template <typename... Args>
void awLogInfo(AwFmtStrWithSrcLoc<std::type_identity_t<Args>...> fmt, Args&&... args)
{
    auto const message = std::format(fmt.format, std::forward<Args>(args)...);
    aw::core::logMessage(aw::core::MessageType::Info, std::format("{}({}): {}", fmt.location.file_name(), fmt.location.line(), message));
}

/// Helper function to log a warning message, automatically tagged with its call site.
/// \tparam ...Args The types of the format arguments.
/// \param fmt The format string and its call site.
/// \param ...args The format arguments.
template <typename... Args>
void awLogWarning(AwFmtStrWithSrcLoc<std::type_identity_t<Args>...> fmt, Args&&... args)
{
    auto const message = std::format(fmt.format, std::forward<Args>(args)...);
    aw::core::logMessage(aw::core::MessageType::Warning, std::format("{}({}): {}", fmt.location.file_name(), fmt.location.line(), message));
}

/// Helper function to log a critical message, automatically tagged with its call site.
/// \tparam ...Args The types of the format arguments.
/// \param fmt The format string and its call site.
/// \param ...args The format arguments.
template <typename... Args>
void awLogCritical(AwFmtStrWithSrcLoc<std::type_identity_t<Args>...> fmt, Args&&... args)
{
    auto const message = std::format(fmt.format, std::forward<Args>(args)...);
    aw::core::logMessage(aw::core::MessageType::Critical, std::format("{}({}): {}", fmt.location.file_name(), fmt.location.line(), message));
}

/// Helper function to log a fatal message, automatically tagged with its call site.
/// \tparam ...Args The types of the format arguments.
/// \param fmt The format string and its call site.
/// \param ...args The format arguments.
template <typename... Args>
void awLogFatal(AwFmtStrWithSrcLoc<std::type_identity_t<Args>...> fmt, Args&&... args)
{
    auto const message = std::format(fmt.format, std::forward<Args>(args)...);
    aw::core::logMessage(aw::core::MessageType::Fatal, std::format("{}({}): {}", fmt.location.file_name(), fmt.location.line(), message));
}
