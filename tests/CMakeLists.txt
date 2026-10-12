#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Include the dependent modules.
include("../share/cmake/aw")

# Add the base library tests.
add_subdirectory("base")

# Add the core library tests.
add_subdirectory("core")

# Add the gui library tests.
add_subdirectory("gui")

# Add the tools tests.
add_subdirectory("tools")

# Add the widgets library tests.
add_subdirectory("widgets")
