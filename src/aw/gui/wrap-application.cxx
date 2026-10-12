//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/gui/application.hxx>
#include <aw/gui/window.hxx>

namespace aw::bind::gui
{
    void wrapApplication(py::module_& m)
    {
        // aw::CoreApplication (registered by awcore) uses pybind11's default holder, so GuiApplication
        // must too - a registered inheritance chain requires a consistent holder type throughout.
        py::class_<aw::gui::GuiApplication, aw::core::CoreApplication> cls(m, "GuiApplication");
        cls.def(py::init<std::vector<std::string> const&>(), py::arg("arguments"), "Constructor.");
        // Mirrors CoreApplication::instance() in awcore's wrap-application.cxx: static accessor
        // returning a non-owning observer, so return_value_policy::reference is required.
        cls.def_static("instance", &aw::gui::GuiApplication::instance, py::return_value_policy::reference, "Get the singleton instance as a GuiApplication, if available.");
        cls.def("notify_top_level_window_closed", &aw::gui::GuiApplication::notifyTopLevelWindowClosed,
                py::arg("window"),
                "Notify that a top-level window has been destroyed.");
        cls.def("register_top_level_window", &aw::gui::GuiApplication::registerTopLevelWindow, py::arg("window"), "Register a visible top-level window.");
        cls.def_static("screens", &aw::gui::GuiApplication::screens, "Get the list of available screens.");
    }
} // namespace aw::bind::gui
