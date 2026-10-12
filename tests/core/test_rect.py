#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test rect class."""

import awcore


def test_default_construction_is_zeroed():
    """Test that a default-constructed rect has zero coordinates and size."""
    rect = awcore.Rect()
    assert rect.x() == 0
    assert rect.y() == 0
    assert rect.width() == 0
    assert rect.height() == 0


def test_explicit_construction_records_coordinates_and_size():
    """Test that the explicit constructor records the given coordinates and size."""
    rect = awcore.Rect(10, 20, 30, 40)
    assert rect.x() == 10
    assert rect.y() == 20
    assert rect.width() == 30
    assert rect.height() == 40
