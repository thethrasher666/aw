//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace aw::bind::gui
{
    /// Wraps the contents of application.hxx.
    void wrapApplication(py::module_& m);

    /// Wraps the contents of icon.hxx.
    void wrapIcon(py::module_& m);

    /// Wraps the contents of screen.hxx.
    void wrapScreen(py::module_& m);

    /// Wraps the contents of window.hxx.
    void wrapWindow(py::module_& m);
} // namespace aw::bind::gui
