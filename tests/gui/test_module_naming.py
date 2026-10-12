#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test that the awgui module's public API follows the project's naming conventions."""

import inspect
import re

import awgui

import pytest

_CAMEL_CASE = re.compile(r"^[A-Z][A-Za-z0-9]*$")
_SNAKE_CASE = re.compile(r"^[a-z][a-z0-9]*(_[a-z0-9]+)*$")


def _public_classes(module):
    """Get the public classes exposed directly by a module, keyed by name."""
    return {name: obj for name, obj in vars(module).items() if not name.startswith("_") and inspect.isclass(obj)}


def _public_callables(obj):
    """Get the public functions/methods/properties exposed directly by a module or class, keyed by name."""
    return {
        name: member
        for name, member in vars(obj).items()
        if not name.startswith("_") and (inspect.isroutine(member) or isinstance(member, property))
    }


def _method_cases():
    """Build (class name, method name) pairs for every public method of every public class in awgui."""
    return [
        (class_name, method_name)
        for class_name, cls in _public_classes(awgui).items()
        for method_name in _public_callables(cls)
    ]


@pytest.mark.parametrize("class_name", _public_classes(awgui))
def test_class_names_are_camel_case(class_name):
    """Test that every public class in awgui is named in CamelCase."""
    assert _CAMEL_CASE.match(class_name), f"{class_name!r} is not CamelCase"


@pytest.mark.parametrize("function_name", _public_callables(awgui))
def test_module_function_names_are_snake_case(function_name):
    """Test that every public top-level function in awgui is named in snake_case."""
    assert _SNAKE_CASE.match(function_name), f"{function_name!r} is not snake_case"


@pytest.mark.parametrize(("class_name", "method_name"), _method_cases(), ids=lambda value: value if isinstance(value, str) else None)
def test_method_names_are_snake_case(class_name, method_name):
    """Test that every public method of every public class in awgui is named in snake_case."""
    assert _SNAKE_CASE.match(method_name), f"{class_name}.{method_name} is not snake_case"
