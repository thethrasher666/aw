//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

// Widget::setLayout() takes a std::unique_ptr<Layout> by value - unique_ptr's destructor requires a
// complete type, so Layout must be fully defined here, not just forward-declared (as widget.hxx does).
#include <aw/widgets/layout.hxx>
#include <aw/widgets/widget.hxx>

namespace aw::bind::widgets
{
    namespace
    {
        // A life-support trampoline, mirroring ObjectTrampoline in awcore's wrap-object.cxx. Widget's
        // WindowDelegate/event-handling virtuals are protected or privately inherited and deliberately
        // left unbound for now, matching how Window's own virtuals were left unbound - so there's
        // nothing here for Python to override. It still needs to exist because Layout::addWidget()
        // below transfers ownership of a Widget via unique_ptr, which requires every instance - not
        // just Python-subclassed ones - to carry a trampoline_self_life_support base.
        class WidgetTrampoline : public aw::widgets::Widget, public py::trampoline_self_life_support
        {
        public:
            using aw::widgets::Widget::Widget;
        };
    } // namespace

    void wrapWidget(py::module_& m)
    {
        // py::smart_holder is required to match aw::Object's holder further up the chain.
        py::class_<aw::widgets::Widget, aw::core::Object, WidgetTrampoline, py::smart_holder> cls(m, "Widget");
        // init_alias (rather than init) always constructs through WidgetTrampoline, even for a plain
        // (non-subclassed) Widget() - set_layout()'s ownership transfer depends on every instance
        // having a trampoline_self_life_support base.
        cls.def(py::init_alias<aw::widgets::Widget*, aw::gui::WindowFlags>(), py::arg("parent") = nullptr, py::arg("flags") = aw::gui::WindowFlags::Widget, "Constructor.");
        cls.def("height", &aw::widgets::Widget::height, "Get the frame height of this widget.");
        cls.def("hide", &aw::widgets::Widget::hide, "Hide the widget.");
        cls.def("set_geometry", &aw::widgets::Widget::setGeometry, py::arg("geometry"), "Set the geometry of this widget, in parent-relative coordinates.");
        cls.def("set_layout",
                &aw::widgets::Widget::setLayout,
                py::arg("layout"),
                py::return_value_policy::reference_internal,
                "Set the layout of this widget, taking ownership of it. Any previously installed layout is destroyed.");
        cls.def("set_title", &aw::widgets::Widget::setTitle, py::arg("title"), "Set the title of this widget.");
        cls.def("set_window_icon", &aw::widgets::Widget::setWindowIcon, py::arg("icon"), "Set the icon of this widget's window.");
        cls.def("set_window_state", &aw::widgets::Widget::setWindowState, py::arg("state"), "Set the presentation state of this widget.");
        cls.def("show", &aw::widgets::Widget::show, "Show the widget.");
        cls.def("width", &aw::widgets::Widget::width, "Get the frame width of this widget.");
        cls.def("window", &aw::widgets::Widget::window, py::return_value_policy::reference_internal, "Get the window associated with this widget, if any.");
    }
} // namespace aw::bind::widgets
