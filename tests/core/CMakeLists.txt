#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_test(test_core
    CXX_SOURCES
        "test-core-application.cxx"
        "test-core-object.cxx"
        "test-core-signal.cxx"
        "test-core-translation.cxx"

    LIBRARIES_PRIVATE
        aw::core
)

# Add the resources to the target.
aw_add_resources(test_core
    RESOURCE_FILES
        "resources.toml"
)

# Define the target.
add_test(
    NAME
        python.bindings.core
    COMMAND
        Python::Interpreter -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Tell conftest.py where to find the built module.
set_tests_properties(python.bindings.core PROPERTIES
    ENVIRONMENT
        "AWCORE_MODULE_DIR=$<TARGET_FILE_DIR:awcore>;AWCORE_CORE_LIB_DIR=$<TARGET_FILE_DIR:aw::core>"
)
