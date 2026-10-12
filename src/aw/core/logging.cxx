//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/logging.hxx"

#include <print>

namespace aw::core
{
    namespace
    {
        auto messageTypeToString(MessageType type) -> std::string
        {
            switch (type)
            {
            case MessageType::Info:
                return "Info";

            case MessageType::Warning:
                return "Warning";

            case MessageType::Critical:
                return "Critical";

            default:
                return "Unknown";
            }
        }
    } // namespace

    void logMessage(MessageType type, std::string const& message)
    {
        std::print("{}: {}\n", messageTypeToString(type), message);
    }
} // namespace aw::core
