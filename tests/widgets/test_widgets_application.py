#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test widgets application class."""

import gc

import awcore

import awgui

import awwidgets

import pytest


@pytest.fixture
def app():
    """Construct a WidgetsApplication for a test and guarantee the singleton is cleared afterwards."""
    instance = awwidgets.WidgetsApplication(["prog", "--flag"])
    try:
        yield instance
    finally:
        del instance
        gc.collect()


def test_instance_is_none_before_construction():
    """Test that GuiApplication.instance() returns None when no application has been constructed.

    WidgetsApplication does not declare its own instance() accessor - it inherits the singleton
    lifecycle from GuiApplication/CoreApplication, so those are what's checked here.
    """
    assert awgui.GuiApplication.instance() is None
    assert awcore.CoreApplication.instance() is None


def test_instance_returns_the_constructed_application(app):
    """Test that instance() (via GuiApplication and CoreApplication) returns the constructed application."""
    assert awgui.GuiApplication.instance() is app
    assert awcore.CoreApplication.instance() is app


def test_instance_is_none_again_after_the_application_is_destroyed():
    """Test that instance() reverts to None once the application is destroyed."""
    # Deliberately not using the `app` fixture here - its generator frame would keep its own
    # reference to the instance alive (via the still-suspended `yield`) until the fixture's
    # teardown runs, so deleting the local name in the test body wouldn't drop the refcount to
    # zero and this assertion would fail.
    instance = awwidgets.WidgetsApplication(["prog"])
    del instance
    gc.collect()
    assert awgui.GuiApplication.instance() is None


def test_a_widgets_application_is_a_gui_and_core_application(app):
    """Test that WidgetsApplication derives from GuiApplication and CoreApplication in Python."""
    assert isinstance(app, awgui.GuiApplication)
    assert isinstance(app, awcore.CoreApplication)


def test_arguments_are_recorded(app):
    """Test that the command line arguments passed to the constructor are recorded."""
    assert app.arguments() == ["prog", "--flag"]
