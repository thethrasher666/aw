//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>
#include <cstdint>

namespace aw::core
{
    /// Represents a rectangle defined by its top-left and bottom-right coordinates.
    class Rect final
    {
    public:
        /// Default constructor.
        Rect() = default;

        /// Constructs a rectangle from the given coordinates.
        /// \param left The left coordinate of the rectangle.
        /// \param top The top coordinate of the rectangle.
        /// \param width The width of the rectangle.
        /// \param height The height of the rectangle.
        explicit Rect(int32_t left, int32_t top, int32_t width, int32_t height) : _x1(left), _y1(top), _x2(left + width), _y2(top + height)
        {
        }

        /// Get the height of the rectangle.
        /// \return The height of the rectangle.
        [[nodiscard]] auto height() const
        {
            return _y2 - _y1;
        }

        /// Get the width of the rectangle.
        /// \return The width of the rectangle.
        [[nodiscard]] auto width() const
        {
            return _x2 - _x1;
        }

        /// Get the left coordinate of the rectangle.
        /// \return The left coordinate of the rectangle.
        [[nodiscard]] auto x() const
        {
            return _x1;
        }

        /// Get the top coordinate of the rectangle.
        /// \return The top coordinate of the rectangle.
        [[nodiscard]] auto y() const
        {
            return _y1;
        }

    private:
        int32_t _x1{};
        int32_t _y1{};
        int32_t _x2{};
        int32_t _y2{};
    };
} // namespace aw::core
