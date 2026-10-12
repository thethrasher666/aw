//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/widgets/application.hxx>

namespace aw::bind::widgets
{
    void wrapApplication(py::module_& m)
    {
        // aw::GuiApplication (registered by awgui) uses pybind11's default holder, so
        // WidgetsApplication must too - a registered inheritance chain requires a consistent
        // holder type throughout.
        py::class_<aw::widgets::WidgetsApplication, aw::gui::GuiApplication> cls(m, "WidgetsApplication");
        cls.def(py::init<std::vector<std::string> const&>(), py::arg("arguments"), "Constructor.");
    }
} // namespace aw::bind::widgets
