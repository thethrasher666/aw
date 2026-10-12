#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_test(test_gui
    CXX_SOURCES
        "test-gui-application.cxx"

    LIBRARIES_PRIVATE
        aw::gui
)

# Define the target.
add_test(
    NAME
        python.bindings.gui
    COMMAND
        Python::Interpreter -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Tell conftest.py where to find the built module.
set_tests_properties(python.bindings.gui PROPERTIES
    ENVIRONMENT
        "AWCORE_MODULE_DIR=$<TARGET_FILE_DIR:awcore>;AWCORE_CORE_LIB_DIR=$<TARGET_FILE_DIR:aw::core>;AWGUI_MODULE_DIR=$<TARGET_FILE_DIR:awgui>;AWGUI_CORE_LIB_DIR=$<TARGET_FILE_DIR:aw::gui>"
)
