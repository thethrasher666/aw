#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test application class."""

import gc

import awcore

import pytest


@pytest.fixture
def app():
    """Construct a CoreApplication for a test and guarantee the singleton is cleared afterwards."""
    instance = awcore.CoreApplication(["prog", "--flag"])
    try:
        yield instance
    finally:
        del instance
        gc.collect()


def test_instance_is_none_before_construction():
    """Test that instance() returns None when no CoreApplication has been constructed."""
    assert awcore.CoreApplication.instance() is None


def test_instance_returns_the_constructed_application(app):
    """Test that instance() returns the same object that was constructed."""
    assert awcore.CoreApplication.instance() is app


def test_instance_is_none_again_after_the_application_is_destroyed():
    """Test that instance() reverts to None once the application is destroyed."""
    # Deliberately not using the `app` fixture here - its generator frame would keep its own
    # reference to the instance alive (via the still-suspended `yield`) until the fixture's
    # teardown runs, so deleting the local name in the test body wouldn't drop the refcount to
    # zero and this assertion would fail.
    instance = awcore.CoreApplication(["prog"])
    del instance
    gc.collect()
    assert awcore.CoreApplication.instance() is None


def test_constructing_a_second_application_raises(app):
    """Test that constructing a second CoreApplication while one exists raises an error."""
    with pytest.raises(RuntimeError):
        awcore.CoreApplication(["prog2"])


def test_arguments_are_recorded(app):
    """Test that the command line arguments passed to the constructor are recorded."""
    assert app.arguments() == ["prog", "--flag"]


def test_continuous_update_can_be_enabled_and_disabled(app):
    """Test that continuous updates can be enabled and disabled."""
    assert app.is_continuous_update_enabled() is False
    app.set_continuous_update_enabled(True)
    assert app.is_continuous_update_enabled() is True
    app.set_continuous_update_enabled(False)
    assert app.is_continuous_update_enabled() is False


def test_locale_returns_a_locale(app):
    """Test that locale() returns a Locale object."""
    assert isinstance(app.locale(), awcore.Locale)
