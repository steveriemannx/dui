# Embed XML resource files into C++ header at build time
#
# Usage in example CMakeLists.txt:
#   set(EMBED_XML_FILES basic.xml)
#   include(duilib_embed_xml.cmake)
#
# This generates a header "embedded_resources.h" containing DString constants
# for each XML file, using a compiled C++ xml_to_header tool.
#
# In your code:
#   #include "embedded_resources.h"
#   DString GetSkinFile() override { return ui_resources::k_basic_xml; }

if(NOT DEFINED EMBED_XML_FILES)
    message(FATAL_ERROR "EMBED_XML_FILES must be set before including duilib_embed_xml.cmake")
endif()

set(TOOL_SRC "${DUILIB_SRC_ROOT_DIR}/cmake/xml_to_header.cpp")
set(TOOL_BIN "${CMAKE_CURRENT_BINARY_DIR}/xml_to_header")
set(GENERATED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/embedded_resources.h")
set(RESOURCES_DIR "${DUILIB_ROOT}/bin/resources")

# Build xml_to_header C++ tool
add_custom_command(
    OUTPUT "${TOOL_BIN}"
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -O2 "${TOOL_SRC}" -o "${TOOL_BIN}"
    DEPENDS "${TOOL_SRC}"
    COMMENT "Building xml_to_header tool"
)

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
    COMMAND "${TOOL_BIN}" "${GENERATED_HEADER}" ${XML_INPUT_FILES}
    DEPENDS "${TOOL_BIN}" ${XML_INPUT_FILES}
    COMMENT "Embedding XML into: ${GENERATED_HEADER}"
)

add_custom_target(embed_xml DEPENDS "${GENERATED_HEADER}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Make the project depend on the generated header
# (stash for duilib_bin.cmake to pick up via add_dependencies)
set(DUILIB_EMBED_XML_HEADER "${GENERATED_HEADER}" CACHE INTERNAL "")
