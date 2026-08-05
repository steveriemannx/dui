# Embed XML resource files into C++ header at build time
#
# Usage in example CMakeLists.txt:
#   set(EMBED_XML_FILES basic.xml)
#   include(dui_embed_xml.cmake)
#
# This generates a header "embedded_resources.h" containing DString constants
# for each XML file, using the xml_to_header tool.
#
# In your code:
#   #include "embedded_resources.h"
#   DString GetSkinFile() override { return ui_resources::k_basic_xml; }
#
# Tool:
#   - Windows : compiled at configure time into cmake/xml_to_header.exe when
#               missing or the sources changed (run inside the vcvarsall environment)
#   - Others  : compiled from cmake/xml_to_header.cpp at build time (Linux / macOS / FreeBSD).

if(NOT DEFINED EMBED_XML_FILES)
    message(FATAL_ERROR "EMBED_XML_FILES must be set before including dui_embed_xml.cmake")
endif()

set(TOOL_SRC "${DUI_SRC_ROOT_DIR}/cmake/xml_to_header.cpp")
set(TOOL_BIN "${CMAKE_CURRENT_BINARY_DIR}/xml_to_header")
set(GENERATED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/embedded_resources.h")
set(RESOURCES_DIR "${DUI_ROOT}/resources")

if(DUI_OS_WINDOWS)
    # Windows: compiled at configure time when missing or the sources changed
    set(TOOL_EXE "${DUI_SRC_ROOT_DIR}/cmake/xml_to_header.exe")

    if("${TOOL_SRC}" IS_NEWER_THAN "${TOOL_EXE}")
        message(STATUS "Rebuilding xml_to_header.exe (source changed)")
        dui_build_msvc_tool(_tool_ok "${TOOL_SRC}" "${TOOL_EXE}" "")
        if(NOT _tool_ok)
            message(WARNING "Failed to rebuild xml_to_header.exe automatically. "
                            "Please compile it manually in a VS Developer Command Prompt: "
                            "cl /std:c++17 /O2 /EHsc cmake/xml_to_header.cpp /Fe:cmake/xml_to_header.exe")
        endif()
    endif()

    if(NOT EXISTS "${TOOL_EXE}")
        message(FATAL_ERROR "xml_to_header.exe not found at ${TOOL_EXE}. "
                            "Compile it in a VS Developer Command Prompt: "
                            "cl /std:c++17 /O2 /EHsc cmake/xml_to_header.cpp /Fe:cmake/xml_to_header.exe")
    endif()
else()
    # macOS / Linux / FreeBSD: compile from source at build time
    set(TOOL_EXE "${TOOL_BIN}")
    add_custom_command(
        OUTPUT "${TOOL_EXE}"
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -O2 "${TOOL_SRC}" -o "${TOOL_EXE}"
        DEPENDS "${TOOL_SRC}"
        COMMENT "Building xml_to_header tool"
    )
endif()

# Find XML files
set(XML_INPUT_FILES)
foreach(xml_file ${EMBED_XML_FILES})
    if(IS_ABSOLUTE "${xml_file}")
        list(APPEND XML_INPUT_FILES "${xml_file}")
    else()
        file(GLOB_RECURSE found_files "${RESOURCES_DIR}/themes/*/${xml_file}")
        if(found_files)
            list(APPEND XML_INPUT_FILES ${found_files})
        else()
            message(WARNING "XML file not found: ${xml_file} under ${RESOURCES_DIR}")
        endif()
    endif()
endforeach()

if(NOT XML_INPUT_FILES)
    message(FATAL_ERROR "No XML files found for embedding")
endif()

# Generate header using the C++ tool
add_custom_command(
    OUTPUT "${GENERATED_HEADER}"
    COMMAND "${TOOL_EXE}" "${GENERATED_HEADER}" ${XML_INPUT_FILES}
    DEPENDS "${TOOL_EXE}" ${XML_INPUT_FILES}
    COMMENT "Embedding XML into: ${GENERATED_HEADER}"
)

add_custom_target(embed_xml DEPENDS "${GENERATED_HEADER}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# (stash for dui_bin.cmake to pick up via add_dependencies)
set(DUI_EMBED_XML_HEADER "${GENERATED_HEADER}" CACHE INTERNAL "")
