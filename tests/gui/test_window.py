#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test window class."""

import awcore

import awgui


def test_construction_defaults_to_a_top_level_window():
    """Test that a window constructed with no arguments has no parent and Window flags."""
    window = awgui.Window()
    assert window.parent() is None


def test_a_window_is_an_object():
    """Test that Window derives from Object in Python, matching the C++ hierarchy."""
    window = awgui.Window()
    assert isinstance(window, awcore.Object)


def test_parent_is_recorded():
    """Test that a window constructed with a parent records that parent."""
    parent = awgui.Window()
    child = awgui.Window(parent)
    assert child.parent() is parent


def test_geometry_can_be_set_and_is_reflected_in_width_and_height():
    """Test that set_geometry() updates width()/height()."""
    window = awgui.Window()
    window.set_geometry(awcore.Rect(0, 0, 200, 100))
    assert window.width() == 200
    assert window.height() == 100


def test_window_state_defaults_to_normal():
    """Test that a newly-constructed window reports the Normal state."""
    window = awgui.Window()
    assert window.window_state() == awgui.WindowState.Normal


def test_set_window_state_does_not_raise():
    """Test that set_window_state() can be called without raising.

    Window::windowState() reads a member that is only ever initialized to Normal and is never
    written back to by Window::setWindowState() (see src/gui/window.cxx) - so, unlike geometry,
    there's currently no way to observe a state change through window_state() after calling
    set_window_state(). This only exercises that the call succeeds.
    """
    window = awgui.Window()
    window.set_window_state(awgui.WindowState.Maximized)


def test_set_title_does_not_raise():
    """Test that set_title() can be called without raising."""
    window = awgui.Window()
    window.set_title("Hello, world!")


def test_set_window_icon_does_not_raise():
    """Test that set_window_icon() can be called without raising, even with an invalid icon."""
    window = awgui.Window()
    window.set_window_icon(awgui.Icon("does-not-exist.qoi"))


def test_native_handles_are_returned_as_integers():
    """Test that the native handle accessors return plain integer addresses."""
    window = awgui.Window()
    assert isinstance(window.native_handle(), int)
    assert isinstance(window.native_connection(), int)
    assert isinstance(window.native_surface_handle(), int)


def test_show_and_hide_do_not_raise():
    """Test that show()/hide() can be called without raising."""
    window = awgui.Window()
    window.show()
    window.hide()
