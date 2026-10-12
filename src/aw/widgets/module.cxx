//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

PYBIND11_MODULE(awwidgets, m)
{
    m.doc() = "AW Widgets module.";

    py::module_::import("awgui");

    aw::bind::widgets::wrapApplication(m);
    aw::bind::widgets::wrapWidget(m);
    aw::bind::widgets::wrapLayouts(m);
}
