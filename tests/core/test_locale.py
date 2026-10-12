#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test locale class."""

import awcore


def test_default_construction_uses_the_system_locale():
    """Test that the default constructor produces a usable locale name."""
    # The system locale is machine-dependent, so only assert that it's a non-empty string rather
    # than a specific value.
    locale = awcore.Locale()
    assert isinstance(locale.name(), str)
    assert locale.name() != ""


def test_explicit_construction_records_the_locale_string():
    """Test that the explicit constructor records the given locale string."""
    locale = awcore.Locale("fr-FR")
    assert locale.name() == "fr-FR"


def test_equality_between_two_locales():
    """Test equality and inequality between two Locale objects."""
    assert awcore.Locale("en-US") == awcore.Locale("en-US")
    assert awcore.Locale("en-US") != awcore.Locale("fr-FR")


def test_equality_between_a_locale_and_a_string():
    """Test equality and inequality between a Locale object and a plain string, in both operand orders."""
    locale = awcore.Locale("en-US")
    assert locale == "en-US"
    assert "en-US" == locale
    assert locale != "fr-FR"
    assert "fr-FR" != locale


def test_equal_locales_hash_the_same():
    """Test that equal locales hash to the same value, so they behave correctly in sets/dicts."""
    assert hash(awcore.Locale("en-US")) == hash(awcore.Locale("en-US"))


def test_locales_deduplicate_correctly_in_a_set():
    """Test that a set of locales deduplicates equal locales."""
    locales = {awcore.Locale("en-US"), awcore.Locale("en-US"), awcore.Locale("fr-FR")}
    assert len(locales) == 2
