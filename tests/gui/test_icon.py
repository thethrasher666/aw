#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test icon class."""

import awgui

import pytest


def test_unresolvable_path_produces_an_invalid_icon():
    """Test that constructing an icon from a path with no registered resource is not valid."""
    # Icon paths are looked up in the compiled-in resource table (see core/resource.hxx), not the
    # filesystem, and this test binary has no resources registered - so any path is "not found".
    icon = awgui.Icon("does-not-exist.qoi")
    assert icon.is_valid() is False


@pytest.mark.parametrize("accessor", ["data", "width", "height", "length"])
def test_accessors_raise_on_an_invalid_icon(accessor):
    """Test that data()/width()/height()/length() raise rather than reading through a null image."""
    icon = awgui.Icon("does-not-exist.qoi")
    with pytest.raises(RuntimeError):
        getattr(icon, accessor)()
