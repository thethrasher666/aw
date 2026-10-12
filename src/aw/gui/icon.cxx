//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/logging.hxx"
#include "aw/core/resource.hxx"
#include "aw/gui/icon.hxx"

namespace aw::gui
{
    Icon::Icon(std::string const& path)
    {
        if (auto const resource = core::findResource(path); resource)
        {
            _qoiImage = std::make_unique<base::QoiImage>();
            if (!_qoiImage->read(resource->data))
            {
                awLogWarning("Failed to read QOI image from resource: {}", path);
            }
        }
    }

    Icon::~Icon()
    {
    }
} // namespace aw::gui
