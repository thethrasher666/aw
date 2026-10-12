//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/signal.hxx>

#include <memory>
#include <string>
#include <vector>

namespace aw::core
{
    /// This is the base class of all objects in this framework.
    /// An object owns every child adopted through addChild() or createChild() and destroys them with itself.
    class Object : public Observer
    {
    public:
        /// Constructor.
        /// \param parent The parent object of this object.
        AW_CORE_API explicit Object(Object* parent = nullptr);

        /// Destructor. Destroys the children owned by this object.
        AW_CORE_API virtual ~Object();

        /// Disable copying.
        Object(Object const&) = delete;

        /// Disable assignment.
        auto operator=(Object const&) -> Object& = delete;

        /// Disable moving.
        Object(Object&&) = delete;

        /// Disable move assignment.
        auto operator=(Object&&) -> Object& = delete;

        /// Takes ownership of an object, making it a child of this object.
        /// \param child The object to adopt.
        /// \return A non-owning pointer to the adopted child, or nullptr if no object was given.
        template <typename T>
        auto addChild(std::unique_ptr<T> child) -> T*
        {
            auto* const adopted = child.get();
            adoptChild(std::move(child));
            return adopted;
        }

        /// Get the children owned by this object.
        /// \return The children owned by this object.
        [[nodiscard]] auto children() const -> std::vector<std::unique_ptr<Object>> const&
        {
            return _children;
        }

        /// Creates an object that is owned by this object.
        /// \param arguments The arguments to forward to the child's constructor.
        /// \return A non-owning pointer to the new child.
        template <typename T, typename... Arguments>
        auto createChild(Arguments&&... arguments) -> T*
        {
            return addChild(std::make_unique<T>(std::forward<Arguments>(arguments)...));
        }

        /// Get the name of this object.
        /// \return The name of this object.
        [[nodiscard]] auto name() const -> const std::string&
        {
            return _name;
        }

        /// Get the parent object of this object.
        /// \return The parent object of this object, or nullptr if this object has no parent.
        [[nodiscard]] auto parent() const -> Object*
        {
            return _parent;
        }

        /// Relinquish ownership of a child, so that it outlives this object.
        /// \param child The child to remove.
        /// \return The removed child, or nullptr if it is not owned by this object.
        [[nodiscard]] AW_CORE_API auto removeChild(Object* child) -> std::unique_ptr<Object>;

        /// Set the name of this object.
        /// \param name The name to set for this object.
        void setObjectName(std::string const& name)
        {
            _name = name;
        }

        /// Translate a string based on the current locale.
        /// \param key The key of the string to translate.
        /// \return The translated string if found, otherwise the key is returned.
        [[nodiscard]] AW_CORE_API auto tr(std::string const& key) const -> std::string;

    protected:
        /// Destroys the children owned by this object, for derived classes that must outlive them.
        AW_CORE_API void destroyChildren();

    private:
        /// Takes ownership of a child and adopts it into the child list.
        /// \param child The object to adopt.
        AW_CORE_API void adoptChild(std::unique_ptr<Object> child);

    private:
        std::vector<std::unique_ptr<Object>> _children;    ///< The children owned by this object.
        std::string                          _name;        ///< The name of this object.
        Object*                              _parent{};    ///< This object's parent.
    };
} // namespace aw::core
