#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

function(aw_add_resources targetName)
    set(optionKeywords)

    set(oneValueKeywords)

    set(multiValueKeywords
        RESOURCE_FILES          # List resource files to be added to the target.
        TRANSLATION_FILES       # List translation manifests to be added to the target.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    foreach(it IN LISTS args_RESOURCE_FILES)
        cmake_path(GET it STEM outfilename)
        cmake_path(ABSOLUTE_PATH it NORMALIZE OUTPUT_VARIABLE infile)
        cmake_path(SET outfile "${CMAKE_CURRENT_BINARY_DIR}/rcc_${outfilename}.cpp")
        cmake_path(SET shadowfile "${outfile}.depends")

        _aw_parse_resource_manifest("${infile}" "${shadowfile}" resourceDepends)

        add_custom_command(
            OUTPUT
                "${outfile}"
            COMMAND
                rcc --output "${outfile}" "${infile}"
            DEPENDS
                rcc
                "${infile}"
                "${shadowfile}"
                ${resourceDepends}
            COMMENT
                "Generating resources from ${it}"
            VERBATIM
        )

        # Ensure the file is marked as generated.
        set_source_files_properties("${outfile}" PROPERTIES GENERATED TRUE)

        # Add the generated file to the target's sources.
        target_sources(${targetName} PRIVATE "${outfile}")
    endforeach()

    foreach(it IN LISTS args_TRANSLATION_FILES)
        cmake_path(GET it STEM outfilename)
        cmake_path(ABSOLUTE_PATH it NORMALIZE OUTPUT_VARIABLE infile)
        cmake_path(SET outfile "${CMAKE_CURRENT_BINARY_DIR}/linguist_${outfilename}.cpp")

        add_custom_command(
            OUTPUT
                "${outfile}"
            COMMAND
                linguist --output "${outfile}" "${infile}"
            DEPENDS
                linguist
                "${infile}"
            COMMENT
                "Generating translations from ${it}"
            VERBATIM
        )

        # Ensure the file is marked as generated.
        set_source_files_properties("${outfile}" PROPERTIES GENERATED TRUE)

        # Add the generated file to the target's sources.
        target_sources(${targetName} PRIVATE "${outfile}")
    endforeach()
endfunction(aw_add_resources)

# Parses a resource manifest for its file list: writes a shadow copy of the manifest so CMake reconfigures whenever it changes.
function(_aw_parse_resource_manifest infile shadowFile outResourceFiles)
    cmake_path(GET infile PARENT_PATH manifestDir)

    set(resourceFiles)
    if(EXISTS "${infile}")
        file(READ "${infile}" manifestContents)
        string(REGEX MATCH "files[ \t]*=[ \t]*\\[([^]]*)\\]" _ "${manifestContents}")
        set(fileEntries "${CMAKE_MATCH_1}")
        string(REGEX MATCHALL "file[ \t]*=[ \t]*\"[^\"]*\"" fileAssignments "${fileEntries}")

        foreach(fileAssignment IN LISTS fileAssignments)
            string(REGEX REPLACE "^file[ \t]*=[ \t]*\"|\"$" "" resourceFile "${fileAssignment}")
            if(NOT IS_ABSOLUTE "${resourceFile}")
                cmake_path(ABSOLUTE_PATH resourceFile BASE_DIRECTORY "${manifestDir}" NORMALIZE)
            endif()
            list(APPEND resourceFiles "${resourceFile}")
        endforeach()

        # Remove structured entries before collecting legacy string entries, so aliases are not dependencies.
        string(REGEX REPLACE "\\{[^}]*\\}" "" legacyEntries "${fileEntries}")
        string(REGEX MATCHALL "\"[^\"]*\"" quotedFiles "${legacyEntries}")

        foreach(quotedFile IN LISTS quotedFiles)
            string(REGEX REPLACE "^\"|\"$" "" resourceFile "${quotedFile}")
            if(NOT IS_ABSOLUTE "${resourceFile}")
                cmake_path(ABSOLUTE_PATH resourceFile BASE_DIRECTORY "${manifestDir}" NORMALIZE)
            endif()
            list(APPEND resourceFiles "${resourceFile}")
        endforeach()

        configure_file("${infile}" "${shadowFile}" COPYONLY)
    endif()

    set(${outResourceFiles} "${resourceFiles}" PARENT_SCOPE)
endfunction(_aw_parse_resource_manifest)
