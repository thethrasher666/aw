#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test gui application class."""

import gc

import awcore

import awgui

import pytest


@pytest.fixture
def app():
    """Construct a GuiApplication for a test and guarantee the singleton is cleared afterwards."""
    instance = awgui.GuiApplication(["prog", "--flag"])
    try:
        yield instance
    finally:
        del instance
        gc.collect()


def test_instance_is_none_before_construction():
    """Test that instance() returns None when no GuiApplication has been constructed."""
    assert awgui.GuiApplication.instance() is None


def test_instance_returns_the_constructed_application(app):
    """Test that instance() returns the same object that was constructed."""
    assert awgui.GuiApplication.instance() is app


def test_instance_is_none_again_after_the_application_is_destroyed():
    """Test that instance() reverts to None once the application is destroyed."""
    # Deliberately not using the `app` fixture here - its generator frame would keep its own
    # reference to the instance alive (via the still-suspended `yield`) until the fixture's
    # teardown runs, so deleting the local name in the test body wouldn't drop the refcount to
    # zero and this assertion would fail.
    instance = awgui.GuiApplication(["prog"])
    del instance
    gc.collect()
    assert awgui.GuiApplication.instance() is None


def test_a_gui_application_is_a_core_application(app):
    """Test that GuiApplication derives from CoreApplication in Python, matching the C++ hierarchy."""
    assert isinstance(app, awcore.CoreApplication)
    assert awcore.CoreApplication.instance() is app


def test_screens_returns_a_list_of_screens(app):
    """Test that screens() returns a list of Screen objects."""
    screens = awgui.GuiApplication.screens()
    assert isinstance(screens, list)
    assert all(isinstance(screen, awgui.Screen) for screen in screens)


def test_register_and_notify_top_level_window_do_not_raise(app):
    """Test that a window can be registered as top-level and its closure notified without raising."""
    window = awgui.Window()
    app.register_top_level_window(window)
    app.notify_top_level_window_closed(window)
