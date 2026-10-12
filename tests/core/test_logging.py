#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Test logging functionality."""

import awcore


def test_message_type_has_expected_members():
    """Test that the MessageType enum has the expected members."""
    assert set(awcore.MessageType.__members__) == {
        "Debug",
        "Info",
        "Warning",
        "Critical",
        "Fatal",
    }


def test_log_message_accepts_every_message_type():
    """Test that the log_message function accepts every message type without raising an exception."""
    for name, message_type in awcore.MessageType.__members__.items():
        awcore.log_message(message_type, f"test message ({name})")
