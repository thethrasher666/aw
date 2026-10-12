//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/gui/icon.hxx>
#include <aw/gui/screen.hxx>

namespace aw::bind::gui
{
    namespace
    {
        // data()/width()/height()/length() all dereference the underlying image unconditionally in
        // icon.hxx and are only safe to call when isValid() is true (an unreadable/missing resource
        // leaves it null) - without this guard, calling any of them on an invalid Icon segfaults the
        // interpreter instead of raising. Raise rather than returning a fallback value (e.g. 0 or empty
        // bytes) since either could be mistaken for a genuine zero-sized icon.
        void requireValid(aw::gui::Icon const& icon)
        {
            if (!icon.isValid())
            {
                throw std::runtime_error("Icon is not valid - it failed to load");
            }
        }
    } // namespace

    void wrapIcon(py::module_& m)
    {
        py::class_<aw::gui::Icon> cls(m, "Icon");
        cls.def(py::init<std::string const&>(), py::arg("path"), "Constructor. Loads a .QOI icon from the resources at the given path.");
        cls.def(
        "data",
        [](aw::gui::Icon const& self) -> py::bytes
        {
            requireValid(self);
            return py::bytes(reinterpret_cast<char const*>(self.data()), self.length());
        },
        "Get the pixel data. Raises if the icon failed to load.");
        cls.def(
        "height",
        [](aw::gui::Icon const& self)
        {
            requireValid(self);
            return self.height();
        },
        "Get the height of the image, in pixels. Raises if the icon failed to load.");
        cls.def("is_valid", &aw::gui::Icon::isValid, "Check if the icon loaded successfully.");
        cls.def(
        "length",
        [](aw::gui::Icon const& self)
        {
            requireValid(self);
            return self.length();
        },
        "Get the length of the pixel data. Raises if the icon failed to load.");
        cls.def(
        "width",
        [](aw::gui::Icon const& self)
        {
            requireValid(self);
            return self.width();
        },
        "Get the width of the image, in pixels. Raises if the icon failed to load.");
    }

    void wrapScreen(py::module_& m)
    {
        py::class_<aw::gui::Screen> cls(m, "Screen");
        cls.def(py::init<aw::core::Rect const&, std::string const&>(), py::arg("geometry"), py::arg("name"), "Constructor.");
        // Screen::geometry() is declared AW_GUI_API in screen.hxx but has no out-of-line definition
        // anywhere in the codebase yet (no screen.cxx exists) - binding it would fail to link. Omitted
        // until the C++ side implements it.
        cls.def("name", &aw::gui::Screen::name, "Get the name of the screen.");
    }
} // namespace aw::bind::gui

PYBIND11_MODULE(awgui, m)
{
    m.doc() = "AW Gui module.";

    py::module_::import("awcore");

    aw::bind::gui::wrapIcon(m);
    aw::bind::gui::wrapScreen(m);
    aw::bind::gui::wrapWindow(m);
    aw::bind::gui::wrapApplication(m);
}
