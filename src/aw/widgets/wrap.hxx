//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace aw::bind::widgets
{
    /// Wraps the contents of application.hxx.
    void wrapApplication(py::module_& m);

    /// Wraps the contents of the numerous layout headers.
    void wrapLayouts(py::module_& m);

    /// Wraps the contents of wrapWidget.hxx.
    void wrapWidget(py::module_& m);
} // namespace aw::bind::widgets
