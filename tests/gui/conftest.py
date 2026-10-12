#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

"""Conftest for the bindings tests."""

import os
import sys

for env_var in (
    "AWCORE_MODULE_DIR",
    "AWCORE_CORE_LIB_DIR",
    "AWGUI_MODULE_DIR",
    "AWGUI_CORE_LIB_DIR",
):
    os.add_dll_directory(os.environ[env_var])

for module_dir in ("AWCORE_MODULE_DIR", "AWGUI_MODULE_DIR"):
    sys.path.insert(0, os.environ[module_dir])
