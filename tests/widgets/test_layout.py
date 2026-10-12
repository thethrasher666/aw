#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test layout and vertical layout classes."""

import awcore

import awwidgets


def test_a_layout_is_an_object():
    """Test that Layout derives from Object in Python, matching the C++ hierarchy."""
    layout = awwidgets.Layout()
    assert isinstance(layout, awcore.Object)


def test_a_vertical_layout_is_a_layout():
    """Test that VerticalLayout derives from Layout in Python, matching the C++ hierarchy."""
    layout = awwidgets.VerticalLayout()
    assert isinstance(layout, awwidgets.Layout)


def test_update_does_not_raise_with_no_parent_widget():
    """Test that update() can be called safely before the layout is installed on a widget."""
    layout = awwidgets.VerticalLayout()
    layout.update()


def test_add_widget_returns_the_same_widget_and_reparents_it():
    """Test that add_widget() returns the added widget and adopts it as an Object child."""
    layout = awwidgets.VerticalLayout()
    widget = awwidgets.Widget()
    result = layout.add_widget(widget)
    assert result is widget
    assert widget.parent() is layout


def test_add_widget_supersedes_the_constructor_parent():
    """Test that add_widget() reparents a widget even if it was constructed with a different parent.

    A Widget's constructor `parent` argument only affects the native window hierarchy - Object
    ownership (what parent() reports) is a separate concept, only established via add_widget()/
    add_child(), and adding a widget to a layout takes ownership regardless of what it was
    constructed with.
    """
    other = awwidgets.Widget()
    widget = awwidgets.Widget(other)
    assert widget.parent() is other

    layout = awwidgets.VerticalLayout()
    layout.add_widget(widget)
    assert widget.parent() is layout


def test_added_widgets_stay_usable_after_ownership_transfer():
    """Test that a widget remains fully usable through its original handle after being adopted."""
    layout = awwidgets.VerticalLayout()
    widget = awwidgets.Widget()
    layout.add_widget(widget)
    widget.set_geometry(awcore.Rect(0, 0, 100, 50))
    widget.show()
    widget.hide()


def test_vertical_layout_update_positions_widgets_after_installation():
    """Test that installing a VerticalLayout on a widget and adding children does not raise."""
    container = awwidgets.Widget()
    layout = awwidgets.VerticalLayout()
    container.set_layout(layout)
    layout.add_widget(awwidgets.Widget())
    layout.add_widget(awwidgets.Widget())
    layout.update()
