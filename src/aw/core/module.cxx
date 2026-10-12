//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/core/locale.hxx>
#include <aw/core/logging.hxx>
#include <aw/core/rect.hxx>
#include <aw/core/string.hxx>

namespace aw::bind::core
{
    void wrapLocale(py::module_& m)
    {
        py::class_<aw::core::Locale> cls(m, "Locale");
        cls.def(py::init<>(), "Constructor - uses the system's default locale.");
        cls.def(py::init<std::string_view>(), py::arg("locale"), "Explicit constructor.");
        cls.def("name", &aw::core::Locale::name, "Get the locale name.");
        cls.def(py::self == py::self);
        cls.def(
        "__eq__", [](aw::core::Locale const& self, std::string_view rhs) { return self == rhs; }, py::arg("other"), "Compare this locale against a locale string.");
        cls.def(
        "__hash__", [](aw::core::Locale const& self) { return py::hash(py::str(self.name())); }, "Hash based on the locale string, consistent with equality.");
    }

    void wrapLogging(py::module_& m)
    {
        py::enum_<aw::core::MessageType>(m, "MessageType")
        .value("Debug", aw::core::MessageType::Debug, "A message used for debugging purposes and will not be shown in production builds.")
        .value("Info", aw::core::MessageType::Info, "An informational message, not an error or warning.")
        .value("Warning", aw::core::MessageType::Warning, "A warning message, indicating a potential issue that may not prevent the program from running.")
        .value("Critical", aw::core::MessageType::Critical, "A critical message, indicating a problem that may prevent the program from running correctly.")
        .value("Fatal", aw::core::MessageType::Fatal, "A fatal error message, indicating a critical problem that will halt the program's execution.");

        m.def("log_message", &aw::core::logMessage, py::arg("type"), py::arg("message"), "Logs a message with the specified message type.");
    }

    void wrapRect(py::module_& m)
    {
        py::class_<aw::core::Rect> cls(m, "Rect");
        cls.def(py::init<>(), "Constructor.");
        cls.def(py::init<int32_t, int32_t, int32_t, int32_t>(), py::arg("left"), py::arg("top"), py::arg("width"), py::arg("height"), "Explicit constructor.");
        cls.def("height", &aw::core::Rect::height, "Get the height of the rectangle.");
        cls.def("width", &aw::core::Rect::width, "Get the width of the rectangle.");
        cls.def("x", &aw::core::Rect::x, "Get the left coordinate of the rectangle.");
        cls.def("y", &aw::core::Rect::y, "Get the top coordinate of the rectangle.");

    }
} // namespace aw::bind::core

PYBIND11_MODULE(awcore, m)
{
    m.doc() = "AW Core module.";

    aw::bind::core::wrapLocale(m);
    aw::bind::core::wrapLogging(m);
    aw::bind::core::wrapObject(m);
    aw::bind::core::wrapRect(m);
    aw::bind::core::wrapApplication(m);
}
