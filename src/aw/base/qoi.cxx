//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/base/qoi.hxx"

#define QOI_IMPLEMENTATION
#include <qoi.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <vector>

namespace aw::base
{
    namespace
    {
        constexpr int kChannels = 4; ///< Images are always decoded to 8-bit RGBA.
    }

    auto QoiImage::read(std::span<std::uint8_t const> data) -> bool
    {
        if (data.empty() || data.size_bytes() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        {
            return false;
        }

        qoi_desc desc{};

        auto* pixels = static_cast<uint8_t*>(::qoi_decode(data.data(), static_cast<int>(data.size_bytes()), &desc, kChannels));
        if (!pixels)
        {
            return false;
        }

        _bitDepth = 8;
        _height = desc.height;
        _width = desc.width;
        _length = static_cast<size_t>(desc.width) * static_cast<size_t>(desc.height) * kChannels;

        _data = std::make_unique<uint8_t[]>(_length);
        std::memcpy(_data.get(), pixels, _length);

        std::free(pixels);

        return true;
    }

    auto QoiImage::read(std::filesystem::path const& path) -> bool
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream)
        {
            return false;
        }

        std::vector<uint8_t> const buffer{ std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };

        return read(buffer);
    }
} // namespace aw::base
