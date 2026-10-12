#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Minimize the install messages.
set(CMAKE_INSTALL_MESSAGE LAZY)

# Generate values from the current date that can be used in file configuration macros.
string(TIMESTAMP BUILD_YEAR  "%Y")
string(TIMESTAMP BUILD_MONTH "%m")
string(TIMESTAMP BUILD_DAY   "%d")
string(TIMESTAMP BUILD_TIME  "%H:%M:%S")
