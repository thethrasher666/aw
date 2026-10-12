//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/application.hxx"
#include "aw/core/object.hxx"
#include "aw/core/translation.hxx"

#include <algorithm>
#include <format>

namespace aw::core
{
    Object::Object(Object* parent) : _parent(parent)
    {
    }

    Object::~Object()
    {
        disconnectAll();
    }

    auto Object::removeChild(Object* child) -> std::unique_ptr<Object>
    {
        auto const position = std::find_if(_children.begin(), _children.end(), [child](auto const& candidate) { return candidate.get() == child; });

        if (position == _children.end())
        {
            return nullptr;
        }

        auto removed = std::move(*position);
        _children.erase(position);
        removed->_parent = nullptr;

        return removed;
    }

    auto Object::tr(std::string const& key) const -> std::string
    {
        auto const keyName = std::format("{}.{}", _name, key);
        auto const result = findTranslation(CoreApplication::instance()->locale(), keyName);
        return result.has_value() ? std::string(result.value()) : keyName;
    }

    void Object::adoptChild(std::unique_ptr<Object> child)
    {
        if (!child)
        {
            return;
        }

        child->_parent = this;
        _children.push_back(std::move(child));
    }

    void Object::destroyChildren()
    {
        _children.clear();
    }
} // namespace aw::core
