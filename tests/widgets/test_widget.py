#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test widget class."""

import awcore

import awgui

import awwidgets


def test_construction_defaults_to_no_parent():
    """Test that a widget constructed with no arguments has no parent."""
    widget = awwidgets.Widget()
    assert widget.parent() is None


def test_a_widget_is_an_object():
    """Test that Widget derives from Object in Python, matching the C++ hierarchy."""
    widget = awwidgets.Widget()
    assert isinstance(widget, awcore.Object)


def test_constructor_parent_is_recorded_as_the_object_parent():
    """Test that the parent passed to the constructor is recorded as the Object parent.

    This only holds until the widget is adopted elsewhere (e.g. added to a layout) - see
    test_layout.py for how add_widget() supersedes it, since Object ownership is a separate
    concept from the constructor's `parent` argument (used to find the native parent window).
    """
    parent = awwidgets.Widget()
    child = awwidgets.Widget(parent)
    assert child.parent() is parent


def test_window_returns_a_window():
    """Test that window() returns the backing Window for this widget."""
    widget = awwidgets.Widget()
    assert isinstance(widget.window(), awgui.Window)


def test_set_geometry_does_not_raise():
    """Test that set_geometry() can be called without raising.

    Unlike a raw gui.Window (see test_window.py), a Widget's width()/height() are not observed
    to update synchronously after set_geometry() in this environment - Widget::onGeometryChanged()
    only runs when the backing window's native platform layer calls back into it, and that isn't
    guaranteed to happen without pumping an event loop. This only exercises that the call succeeds.
    """
    widget = awwidgets.Widget()
    widget.set_geometry(awcore.Rect(0, 0, 300, 150))


def test_set_title_does_not_raise():
    """Test that set_title() can be called without raising."""
    widget = awwidgets.Widget()
    widget.set_title("Hello, world!")


def test_set_window_icon_does_not_raise():
    """Test that set_window_icon() can be called without raising, even with an invalid icon."""
    widget = awwidgets.Widget()
    widget.set_window_icon(awgui.Icon("does-not-exist.qoi"))


def test_set_window_state_does_not_raise():
    """Test that set_window_state() can be called without raising."""
    widget = awwidgets.Widget()
    widget.set_window_state(awgui.WindowState.Maximized)


def test_show_and_hide_do_not_raise():
    """Test that show()/hide() can be called without raising."""
    widget = awwidgets.Widget()
    widget.show()
    widget.hide()
