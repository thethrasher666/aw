//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/base/qoi.hxx>
#include <aw/gui/api.hxx>

#include <memory>
#include <string>

namespace aw::gui
{
    /// Represents an icon that can be used in the GUI.
    /// Currently, only supports .QOI files.
    class Icon
    {
    public:
        /// Constructor.
        /// \param path The path to the icon file in the resources.
        AW_GUI_API explicit Icon(std::string const& path);

        /// Destructor.
        AW_GUI_API ~Icon();

        /// Get the pixel data.
        /// \return A pointer to the pixel data.
        [[nodiscard]] auto data() const -> uint8_t const*
        {
            return _qoiImage->data();
        }

        /// Check if the icon is valid.
        /// \return true if the icon is valid, false otherwise.
        [[nodiscard]] auto isValid() const
        {
            return _qoiImage ? true : false;
        }

        /// Get the length of the pixel data.
        /// \return The length of the pixel data.
        [[nodiscard]] auto length() const -> size_t
        {
            return _qoiImage->length();
        }

        /// Get the width of the image.
        /// \return The width of the image, in pixels.
        [[nodiscard]] auto width() const -> uint32_t
        {
            return _qoiImage->width();
        }

        /// Get the height of the image.
        /// \return The height of the image, in pixels.
        [[nodiscard]] auto height() const -> uint32_t
        {
            return _qoiImage->height();
        }

    private:
        std::unique_ptr<base::QoiImage> _qoiImage; ///< The QOI image data for the icon.
    };
} // namespace aw::gui
