//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/gui/window.hxx>

namespace aw::bind::gui
{
    namespace
    {
        // Native handles are opaque pointers meant for interop with other native/C libraries (e.g.
        // passing a surface handle into a Vulkan binding). There's no meaningful Python-side type for
        // a raw void*, so it's exposed as its integer address, the same convention other pybind11-based
        // GUI bindings use for native window handles.
        auto toAddress(void* pointer) -> uintptr_t
        {
            return reinterpret_cast<uintptr_t>(pointer);
        }
    } // namespace

    void wrapWindow(py::module_& m)
    {
        py::enum_<aw::gui::WindowFlags>(m, "WindowFlags")
        .value("Widget", aw::gui::WindowFlags::Widget, "Child window embedded in a parent.")
        .value("Window", aw::gui::WindowFlags::Window, "Top-level resizable window.")
        .value("Dialog", aw::gui::WindowFlags::Dialog, "Top-level dialog-style window.");

        py::enum_<aw::gui::WindowState>(m, "WindowState")
        .value("Normal", aw::gui::WindowState::Normal, "The window is in its normal state.")
        .value("Minimized", aw::gui::WindowState::Minimized, "The window is minimized to the taskbar or dock.")
        .value("Maximized", aw::gui::WindowState::Maximized, "The window is maximized to fill the available screen space.")
        .value("FullScreen", aw::gui::WindowState::FullScreen, "The window is in full-screen mode, covering the entire screen.");

        // py::smart_holder is required to match aw::Object's holder - pybind11 requires a consistent
        // holder type across a registered inheritance chain. No trampoline is registered here: unlike
        // Object's remove_child() dance, Window has no ownership-reclaim scenario, and overriding
        // Window's (protected) or WindowDelegate's (public) virtuals from Python is a bigger design
        // question - deferred, matching CoreApplication's own onBeforeRun/onAfterRun/onFrame protected
        // virtuals not being overridable from Python either.
        py::class_<aw::gui::Window, aw::core::Object, py::smart_holder> cls(m, "Window");
        cls.def(py::init(
                [](aw::gui::Window* parent, aw::gui::WindowFlags flags)
                {
                    // The `delegate` parameter is deliberately not exposed - WindowDelegate isn't bound to
                    // Python yet, so every Python-constructed Window is delegate-less for now.
                    return std::make_unique<aw::gui::Window>(parent, flags, nullptr);
                }),
                py::arg("parent") = nullptr,
                py::arg("flags") = aw::gui::WindowFlags::Window,
                "Constructor.");
        cls.def("height", &aw::gui::Window::height, "Get the frame height of this window.");
        cls.def("hide", &aw::gui::Window::hide, "Hide the window.");
        cls.def(
        "native_connection", [](aw::gui::Window const& self) { return toAddress(self.nativeConnection()); }, "Get the native connection of this window.");
        cls.def(
        "native_handle", [](aw::gui::Window const& self) { return toAddress(self.nativeHandle()); }, "Get the native handle of this window.");
        cls.def(
        "native_surface_handle",
        [](aw::gui::Window& self) { return toAddress(self.nativeSurfaceHandle()); },
        "Get the native handle to pass to a graphics API when creating a rendering surface.");
        cls.def("parent", &aw::gui::Window::parent, py::return_value_policy::reference, "Get the parent window, if any.");
        cls.def("set_geometry", &aw::gui::Window::setGeometry, py::arg("geometry"), "Set the geometry of this window.");
        cls.def("set_title", &aw::gui::Window::setTitle, py::arg("title"), "Set the title of this window.");
        cls.def("set_window_icon", &aw::gui::Window::setWindowIcon, py::arg("icon"), "Set the icon of this window.");
        cls.def("set_window_state", &aw::gui::Window::setWindowState, py::arg("state"), "Set the presentation state of this window.");
        cls.def("show", &aw::gui::Window::show, "Show the window.");
        cls.def("width", &aw::gui::Window::width, "Get the frame width of this window.");
        cls.def("window_state", &aw::gui::Window::windowState, "Get the current presentation state of this window.");
    }
} // namespace aw::bind::gui
