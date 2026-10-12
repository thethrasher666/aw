// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/core/object.hxx>

namespace aw::bind::core
{
    namespace
    {
        // A trampoline is normally used to let Python override virtual methods, but Object has none to
        // override today - this one exists purely so pybind11's smart_holder can reclaim ownership of a
        // child in remove_child() below. Without a registered trampoline, once any Python wrapper exists
        // for an Object (which is unavoidable - you need a handle to pass to remove_child in the first
        // place), pybind11 refuses to hand ownership of a returned std::unique_ptr back to Python at all.
        class ObjectTrampoline : public aw::core::Object, public py::trampoline_self_life_support
        {
        public:
            using aw::core::Object::Object;
        };
    } // namespace

    void wrapObject(py::module_& m)
    {
        // py::smart_holder (rather than pybind11's classic std::unique_ptr holder) is required for
        // add_child()/remove_child() below to transfer ownership of a child to/from C++'s _children.
        py::class_<aw::core::Object, ObjectTrampoline, py::smart_holder>(m, "Object")
        // init_alias (rather than init) always constructs through ObjectTrampoline, even for a plain
        // (non-subclassed) Object() - remove_child()'s ownership reclaim below depends on every
        // instance having a trampoline_self_life_support base, not just Python-subclassed ones.
        .def(py::init_alias<aw::core::Object*>(), py::arg("parent") = nullptr, "Constructor.")
        .def(
        "add_child",
        // std::unique_ptr<aw::Object> disowns the Python wrapper for `child` on the way in, handing
        // ownership to this object's _children. Thanks to ObjectTrampoline's life-support tracking,
        // `child` stays fully usable afterwards - it's the same object, just C++-owned instead of
        // Python-owned - and remove_child() below can hand ownership back to this exact wrapper. The
        // one real risk: if `child` is never passed to remove_child() and this object (or an ancestor)
        // is destroyed first, any Python handle still referencing it becomes a dangling reference -
        // Object has no "about to be destroyed" signal today.
        [](aw::core::Object& self, std::unique_ptr<aw::core::Object> child) { self.addChild<aw::core::Object>(std::move(child)); },
        py::arg("child"),
        "Takes ownership of an object, making it a child of this object.")
        .def(
        "children",
        [](aw::core::Object const& self)
        {
            std::vector<aw::core::Object*> children;
            children.reserve(self.children().size());
            for (auto const& child : self.children())
            {
                children.push_back(child.get());
            }
            return children;
        },
        py::return_value_policy::reference_internal,
        "Get the children owned by this object.")
        .def("name", &aw::core::Object::name, "Get the name of this object.")
        .def("parent", &aw::core::Object::parent, py::return_value_policy::reference, "Get the parent object of this object.")
        .def("remove_child", &aw::core::Object::removeChild, py::arg("child"), "Relinquish ownership of a child, so that it outlives this object.")
        .def("set_object_name", &aw::core::Object::setObjectName, py::arg("name"), "Set the name of this object.")
        .def("tr", &aw::core::Object::tr, py::arg("key"), "Translate a string based on the current locale.");
    }
} // namespace aw::bind::core
