//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace aw::bind::core
{
    /// Wraps the contents of application.hxx.
    void wrapApplication(py::module_& m);

    /// Wraps the contents of logging.hxx.
    void wrapLocale(py::module_& m);

    /// Wraps the contents of logging.hxx.
    void wrapLogging(py::module_& m);

    /// Wraps the contents of object.hxx.
    void wrapObject(py::module_& m);

    /// Wraps the contents of object.hxx.
    void wrapRect(py::module_& m);
} // namespace aw::bind::core
