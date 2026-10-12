//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/widgets/layout.hxx>

namespace aw::bind::widgets
{
    namespace
    {
        // Life-support trampolines, mirroring ObjectTrampoline in awcore's wrap-object.cxx. Neither
        // class exposes a virtual for Python to override here - VerticalLayout::update() is a complete
        // C++ implementation, and letting Python subclasses override Layout::update() to write custom
        // layout algorithms is a bigger design question, deferred for now. They still need to exist
        // because Widget::setLayout() transfers ownership of a Layout via unique_ptr, which requires
        // every instance - not just Python-subclassed ones - to carry a trampoline_self_life_support
        // base.
        class LayoutTrampoline : public aw::widgets::Layout, public py::trampoline_self_life_support
        {
        public:
            using aw::widgets::Layout::Layout;
        };

        class VerticalLayoutTrampoline : public aw::widgets::VerticalLayout, public py::trampoline_self_life_support
        {
        public:
            using aw::widgets::VerticalLayout::VerticalLayout;
        };
    } // namespace

    void wrapLayouts(py::module_& m)
    {
        // py::smart_holder is required to match aw::Object's holder further up the chain.
        py::class_<aw::widgets::Layout, aw::core::Object, LayoutTrampoline, py::smart_holder> layoutCls(m, "Layout");
        // init_alias (rather than init) always constructs through LayoutTrampoline, even for a plain
        // (non-subclassed) Layout() - Widget::set_layout()'s ownership transfer depends on every
        // instance having a trampoline_self_life_support base.
        layoutCls.def(py::init_alias<>(), "Constructor.");
        layoutCls.def(
        "add_widget", &aw::widgets::Layout::addWidget, py::arg("widget"), py::return_value_policy::reference_internal, "Adds a widget to the layout, taking ownership of it.");
        layoutCls.def("update", &aw::widgets::Layout::update, "Recomputes and applies the geometry of all widgets managed by this layout.");

        py::class_<aw::widgets::VerticalLayout, aw::widgets::Layout, VerticalLayoutTrampoline, py::smart_holder> verticalLayoutCls(m, "VerticalLayout");
        verticalLayoutCls.def(py::init_alias<>(), "Constructor.");
    }
} // namespace aw::bind::widgets
