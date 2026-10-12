//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <filesystem>
#include <memory>
#include <span>

namespace aw::base
{
    /// Provides functionality for reading QOI images.
    class QoiImage final
    {
    public:
        /// Get the bit-depth of the image.
        /// \return The bit-depth of the image.
        [[nodiscard]] auto bitDepth() const -> uint32_t
        {
            return _bitDepth;
        }

        /// Get the pixel data.
        /// \return A pointer to the pixel data.
        [[nodiscard]] auto data() const -> uint8_t const*
        {
            return _data.get();
        }

        /// Get the height of the image.
        /// \return The height of the image.
        [[nodiscard]] auto height() const -> uint32_t
        {
            return _height;
        }

        /// Get the length of the pixel data.
        /// \return The length of the pixel data.
        [[nodiscard]] auto length() const -> size_t
        {
            return _length;
        }

        /// Reads a QOI image from a memory buffer.
        /// \param data A span of bytes representing the QOI image data.
        /// \return true if the image was read successfully; otherwise, false.
        [[nodiscard]] auto read(std::span<std::uint8_t const> data) -> bool;

        /// Reads a QOI image from the specified file path.
        /// \param path The file path of the QOI image to read.
        /// \return true if the image was read successfully; otherwise, false.
        [[nodiscard]] auto read(std::filesystem::path const& path) -> bool;

        /// Get the width of the image.
        /// \return The width of the image.
        [[nodiscard]] auto width() const -> uint32_t
        {
            return _width;
        }

    private:
        uint32_t                   _bitDepth{}; ///< The bit-depth of the image.
        std::unique_ptr<uint8_t[]> _data;       ///< The raw image data, as 8-bit RGBA.
        uint32_t                   _height{};   ///< The height of the image.
        size_t                     _length{};   ///< The length of the image data.
        uint32_t                   _width{};    ///< The width of the image.
    };
} // namespace aw::base
