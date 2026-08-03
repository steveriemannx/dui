# Generate pure C++ UI code from XML layout files at build time
#
# Usage in example CMakeLists.txt:
#   set(GEN_XML_FILES basic.xml)
#   set(GEN_FUNC_NAME InitBasicUI)
#   include(duilib_gen_code.cmake)
#
# This generates "generated_ui.inc" containing a function
#   void InitBasicUI(ui::Window* pWindow)
# that creates the entire UI in pure C++ (no XML parsing at runtime).
#
# Tool:
#   - Windows : pre-compiled cmake/xml_to_code.exe (rebuilt at configure time
#               when the source changes; avoids Device Guard blocking an exe
#               compiled on-the-fly during the MSBuild step).
#   - Others  : compiled from cmake/xml_to_code.cpp at build time.

if(NOT DEFINED GEN_XML_FILES)
    message(FATAL_ERROR "GEN_XML_FILES must be set before including duilib_gen_code.cmake")
endif()
if(NOT DEFINED GEN_FUNC_NAME)
    message(FATAL_ERROR "GEN_FUNC_NAME must be set before including duilib_gen_code.cmake")
endif()

# Output to source dir as .inc (not .cpp) to avoid duplicate compilation
set(GENERATED_SRC "${CMAKE_CURRENT_SOURCE_DIR}/generated_ui.inc")
set(RESOURCES_DIR "${DUILIB_ROOT}/bin/resources")

# Find XML files
set(XML_INPUT_FILES)
foreach(xml_file ${GEN_XML_FILES})
    if(IS_ABSOLUTE "${xml_file}")
        list(APPEND XML_INPUT_FILES "${xml_file}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${xml_file}")
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

if(DUILIB_OS_WINDOWS)
    # Windows: pre-compiled exe in the source tree.
    # It is rebuilt at CONFIGURE time when its source files change so that
    # it stays up-to-date without triggering Device Guard at build time.

    set(TOOL_EXE "${DUILIB_SRC_ROOT_DIR}/cmake/xml_to_code.exe")
    set(TOOL_SRC "${DUILIB_SRC_ROOT_DIR}/cmake/xml_to_code.cpp")
    set(PUGIXML_SRC "${DUILIB_SRC_ROOT_DIR}/third_party/xml/pugixml.cpp")
    set(PUGIXML_DIR "${DUILIB_SRC_ROOT_DIR}/third_party/xml")

    # Rebuild at configure time if source is newer (idempotent across examples)
    if("${TOOL_SRC}" IS_NEWER_THAN "${TOOL_EXE}" OR "${PUGIXML_SRC}" IS_NEWER_THAN "${TOOL_EXE}")
        message(STATUS "Rebuilding xml_to_code.exe (source changed)")
        execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}" /nologo /std:c++17 /O2 /EHsc
                    -I"${PUGIXML_DIR}"
                    "${TOOL_SRC}" "${PUGIXML_SRC}" /Fe:"${TOOL_EXE}"
            WORKING_DIRECTORY "${DUILIB_SRC_ROOT_DIR}"
            RESULT_VARIABLE _build_result
        )
        if(NOT _build_result EQUAL 0)
            message(FATAL_ERROR "Failed to build xml_to_code.exe")
        endif()
    endif()

    if(NOT EXISTS "${TOOL_EXE}")
        message(FATAL_ERROR "xml_to_code.exe not found at ${TOOL_EXE}")
    endif()

    add_custom_command(
        OUTPUT "${GENERATED_SRC}"
        COMMAND "${TOOL_EXE}" "${GENERATED_SRC}" "${GEN_FUNC_NAME}" ${XML_INPUT_FILES}
        DEPENDS "${TOOL_EXE}" ${XML_INPUT_FILES}
        COMMENT "Generating C++ UI code from XML: ${GENERATED_SRC}"
    )
else()
    # macOS / Linux: compile from source at build time (no Device Guard)
    set(TOOL_SRC "${DUILIB_SRC_ROOT_DIR}/cmake/xml_to_code.cpp")
    set(PUGIXML_SRC "${DUILIB_SRC_ROOT_DIR}/third_party/xml/pugixml.cpp")
    set(PUGIXML_DIR "${DUILIB_SRC_ROOT_DIR}/third_party/xml")
    set(TOOL_EXE "${CMAKE_CURRENT_BINARY_DIR}/xml_to_code")

    add_custom_command(
        OUTPUT "${TOOL_EXE}"
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -O2 -I"${PUGIXML_DIR}"
                "${TOOL_SRC}" "${PUGIXML_SRC}" -o "${TOOL_EXE}"
        DEPENDS "${TOOL_SRC}" "${PUGIXML_SRC}"
        COMMENT "Building xml_to_code tool"
    )

    add_custom_command(
        OUTPUT "${GENERATED_SRC}"
        COMMAND "${TOOL_EXE}" "${GENERATED_SRC}" "${GEN_FUNC_NAME}" ${XML_INPUT_FILES}
        DEPENDS "${TOOL_EXE}" ${XML_INPUT_FILES}
        COMMENT "Generating C++ UI code from XML: ${GENERATED_SRC}"
    )
endif()

add_custom_target("${PROJECT_NAME}_gen_xml_code" DEPENDS "${GENERATED_SRC}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Stash for duilib_bin.cmake to add dependency
set(DUILIB_GEN_CODE_SRC "${GENERATED_SRC}")
