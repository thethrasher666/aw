#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_test(test_widgets
    CXX_SOURCES
        "test-widgets-application.cxx"

    LIBRARIES_PRIVATE
        aw::widgets
)

# Define the target.
add_test(
    NAME
        python.bindings.widgets
    COMMAND
        Python::Interpreter -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Tell conftest.py where to find the built module.
set_tests_properties(python.bindings.widgets PROPERTIES
    ENVIRONMENT
        "AWCORE_MODULE_DIR=$<TARGET_FILE_DIR:awcore>;AWCORE_CORE_LIB_DIR=$<TARGET_FILE_DIR:aw::core>;AWGUI_MODULE_DIR=$<TARGET_FILE_DIR:awgui>;AWGUI_CORE_LIB_DIR=$<TARGET_FILE_DIR:aw::gui>;AWWIDGETS_MODULE_DIR=$<TARGET_FILE_DIR:awwidgets>;AWWIDGETS_CORE_LIB_DIR=$<TARGET_FILE_DIR:aw::widgets>"
)
