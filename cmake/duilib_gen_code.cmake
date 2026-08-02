# Generate pure C++ UI code from XML layout files at build time
#
# Usage in example CMakeLists.txt:
#   set(GEN_XML_FILES basic.xml)
#   set(GEN_FUNC_NAME InitBasicUI)
#   include(duilib_gen_code.cmake)
#
# This generates "generated_ui.cpp" containing a function
#   void InitBasicUI(ui::Window* pWindow)
# that creates the entire UI in pure C++ (no XML parsing at runtime).
#
# In your code:
#   #include "generated_ui.cpp"
#   void OnInitWindow() override { InitBasicUI(this); }

if(NOT DEFINED GEN_XML_FILES)
    message(FATAL_ERROR "GEN_XML_FILES must be set before including duilib_gen_code.cmake")
endif()
if(NOT DEFINED GEN_FUNC_NAME)
    message(FATAL_ERROR "GEN_FUNC_NAME must be set before including duilib_gen_code.cmake")
endif()

set(TOOL_SRC "${DUILIB_SRC_ROOT_DIR}/cmake/xml_to_code.cpp")
set(PUGIXML_SRC "${DUILIB_SRC_ROOT_DIR}/third_party/xml/pugixml.cpp")
set(PUGIXML_DIR "${DUILIB_SRC_ROOT_DIR}/third_party/xml")
set(TOOL_BIN "${CMAKE_CURRENT_BINARY_DIR}/xml_to_code")
# Output to source dir as .inc (not .cpp) to avoid duplicate compilation
set(GENERATED_SRC "${CMAKE_CURRENT_SOURCE_DIR}/generated_ui.inc")
set(RESOURCES_DIR "${DUILIB_ROOT}/bin/resources")

# Build xml_to_code tool (with pugixml linked)
add_custom_command(
    OUTPUT "${TOOL_BIN}"
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -O2 -I"${PUGIXML_DIR}"
            "${TOOL_SRC}" "${PUGIXML_SRC}" -o "${TOOL_BIN}"
    DEPENDS "${TOOL_SRC}" "${PUGIXML_SRC}"
    COMMENT "Building xml_to_code tool"
)

# Find XML files
set(XML_INPUT_FILES)
foreach(xml_file ${GEN_XML_FILES})
    if(IS_ABSOLUTE "${xml_file}")
        list(APPEND XML_INPUT_FILES "${xml_file}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${xml_file}")
        # A relative path resolved against this example's source directory:
        # supports per-example XML files (e.g. ../hello/hello.xml)
        list(APPEND XML_INPUT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${xml_file}")
    else()
        file(GLOB_RECURSE found_files "${RESOURCES_DIR}/themes/*/${xml_file}")
        if(found_files)
            list(APPEND XML_INPUT_FILES ${found_files})
        else()
            message(WARNING "XML file not found: ${xml_file}")
        endif()
    endif()
endforeach()

if(NOT XML_INPUT_FILES)
    message(FATAL_ERROR "No XML files found for code generation")
endif()

# Auto-embed images from global.xml?
set(GEN_AUTO_EMBED_DEFAULT OFF)
if(DEFINED GEN_AUTO_EMBED)
    set(GEN_AUTO_EMBED_DEFAULT ${GEN_AUTO_EMBED})
endif()
option(GEN_AUTO_EMBED "Auto-embed images referenced by classes from global.xml" ${GEN_AUTO_EMBED_DEFAULT})

# Generate C++ source from XML
set(GEN_CMD_ARGS "${TOOL_BIN}" "${GENERATED_SRC}" "${GEN_FUNC_NAME}")
if(GEN_AUTO_EMBED)
    file(GLOB_RECURSE global_xml "${RESOURCES_DIR}/themes/*/global.xml")
    if(global_xml)
        list(APPEND GEN_CMD_ARGS "-g" "${global_xml}")
        list(APPEND XML_INPUT_FILES "${global_xml}")
    endif()
endif()
list(APPEND GEN_CMD_ARGS ${XML_INPUT_FILES})

add_custom_command(
    OUTPUT "${GENERATED_SRC}"
    COMMAND ${GEN_CMD_ARGS}
    DEPENDS "${TOOL_BIN}" ${XML_INPUT_FILES}
    COMMENT "Generating C++ UI code from XML: ${GENERATED_SRC}"
)

add_custom_target("${PROJECT_NAME}_gen_xml_code" DEPENDS "${GENERATED_SRC}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Stash for duilib_bin.cmake to add dependency
set(DUILIB_GEN_CODE_SRC "${GENERATED_SRC}")
