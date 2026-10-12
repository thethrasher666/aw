#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test object class."""

import gc

import awcore


def test_construction_and_name():
    """Test that an object can be constructed and its name can be set and retrieved."""
    obj = awcore.Object()
    assert obj.name() == ""
    obj.set_object_name("root")
    assert obj.name() == "root"


def test_root_object_has_no_parent():
    """Test that a newly constructed object has no parent."""
    root = awcore.Object()
    assert root.parent() is None


def test_construction_with_parent_argument():
    """Test that an object can be constructed with a parent argument, and that the parent is recorded correctly."""
    parent = awcore.Object()
    child = awcore.Object(parent)
    # The constructor only records `parent` - it does not add `child` to parent's children.
    assert child.parent() is parent
    assert parent.children() == []


def test_add_child_appears_in_children_and_reports_correct_parent():
    """Test that when a child is added to a parent, it appears in the parent's children list and reports the correct parent."""
    root = awcore.Object()
    child = awcore.Object()
    child.set_object_name("kid")
    root.add_child(child)

    children = root.children()
    assert len(children) == 1
    assert children[0].name() == "kid"
    assert children[0].parent() is root


def test_original_child_handle_stays_usable_after_add_child():
    """Test that the original child handle remains usable after being added to a parent."""
    root = awcore.Object()
    child = awcore.Object()
    child.set_object_name("kid")
    root.add_child(child)

    # `child` transferred ownership to `root`, but the Python wrapper itself must stay live and
    # usable - this is what ObjectTrampoline's life-support tracking in wrap-object.cxx enables.
    assert child.name() == "kid"
    assert child.parent() is root


def test_remove_child_hands_ownership_back_to_the_same_python_object():
    """Test that removing a child hands ownership back to the same Python object."""
    root = awcore.Object()
    child = awcore.Object()
    child.set_object_name("kid")
    root.add_child(child)

    kid = root.children()[0]
    removed = root.remove_child(kid)

    assert removed is kid
    assert removed.parent() is None
    assert root.children() == []


def test_removed_child_outlives_its_former_parent():
    """Test that a removed child can outlive its former parent."""
    root = awcore.Object()
    child = awcore.Object()
    child.set_object_name("kid")
    root.add_child(child)

    removed = root.remove_child(root.children()[0])

    del root
    gc.collect()

    assert removed.name() == "kid"


def test_add_remove_cycle_can_repeat_on_the_same_parent():
    """Test that a parent can repeatedly add and remove children."""
    root = awcore.Object()

    for index in range(3):
        child = awcore.Object()
        child.set_object_name(f"child-{index}")
        root.add_child(child)
        removed = root.remove_child(root.children()[0])
        assert removed.name() == f"child-{index}"
        assert root.children() == []
