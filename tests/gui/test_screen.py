#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test screen class."""

import awcore

import awgui

# Screen.geometry() is intentionally not exercised here - it's declared in screen.hxx but has no
# out-of-line definition anywhere in the codebase yet, so it isn't bound (see wrapScreen() in
# src/bindings/gui/module.cxx).


def test_construction_records_the_name():
    """Test that the constructor records the given name."""
    screen = awgui.Screen(awcore.Rect(0, 0, 1920, 1080), "Primary")
    assert screen.name() == "Primary"
