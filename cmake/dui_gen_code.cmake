# Generate pure C++ UI code from XML layout files at build time
#
# Usage in example CMakeLists.txt:
#   set(GEN_XML_FILES basic.xml)
#   set(GEN_FUNC_NAME InitBasicUI)
#   include(dui_gen_code.cmake)
#
# This generates "generated_ui.inc" containing a function
#   void InitBasicUI(ui::Window* pWindow)
# that creates the entire UI in pure C++ (no XML parsing at runtime).
#
# Tool:
#   - Windows : compiled at configure time into build/tools/xml_to_code.exe (only when
#               missing or the sources changed), run inside the vcvarsall
#               environment so cl.exe finds the standard library headers.
#   - Others  : compiled from tools/xml_to_code.cpp at configure time.

if(NOT DEFINED GEN_XML_FILES)
    message(FATAL_ERROR "GEN_XML_FILES must be set before including dui_gen_code.cmake")
endif()
if(NOT DEFINED GEN_FUNC_NAME)
    message(FATAL_ERROR "GEN_FUNC_NAME must be set before including dui_gen_code.cmake")
endif()

# Optional resource folders corresponding to GEN_XML_FILES by index. Each path
# is relative to the active theme root, for example "chat" or "../shared".
# Example:
#   set(GEN_RESOURCE_FOLDERS chat shared)
#   # applies "chat" to GEN_XML_FILES[0] and "shared" to GEN_XML_FILES[1]
set(GEN_RESOURCE_ARGS)
if(DEFINED GEN_RESOURCE_FOLDERS)
    foreach(_resource_folder ${GEN_RESOURCE_FOLDERS})
        list(APPEND GEN_RESOURCE_ARGS -r "${_resource_folder}")
    endforeach()
endif()

# Keep generated files in the build tree so source checkouts never retain stale UI code.
set(GENERATED_SRC "${CMAKE_CURRENT_BINARY_DIR}/generated_ui.inc")
set(RESOURCES_DIR "${DUI_ROOT}/resources")

# GEN_AUTO_EMBED ON: pass -g global.xml so the tool emits RegisterEmbeddedClasses()
# with the images referenced by classes in global.xml embedded into the binary
set(GEN_TOOL_GLOBAL_ARGS)
if(GEN_AUTO_EMBED)
    set(GEN_TOOL_GLOBAL_ARGS -g "${RESOURCES_DIR}/themes/${DUI_EXAMPLE_THEME}/global.xml")
endif()

# Find XML files
set(XML_INPUT_FILES)
foreach(xml_file ${GEN_XML_FILES})
    if(DUI_EXAMPLE_THEME AND "${xml_file}" MATCHES "/themes/default/")
        string(REPLACE "/themes/default/" "/themes/${DUI_EXAMPLE_THEME}/" xml_file "${xml_file}")
    endif()
    if(IS_ABSOLUTE "${xml_file}")
        list(APPEND XML_INPUT_FILES "${xml_file}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${xml_file}")
        list(APPEND XML_INPUT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${xml_file}")
    else()
        # Prefer the active platform theme for generated C++ UI code.  If we globbed
        # every theme (default, windows11, ...), the same XML basename would
        # produce duplicate InitXxx() functions in generated_ui.inc.
        file(GLOB_RECURSE found_files "${RESOURCES_DIR}/themes/${DUI_EXAMPLE_THEME}/${xml_file}")
        if(NOT found_files)
            file(GLOB_RECURSE found_files "${RESOURCES_DIR}/themes/*/${xml_file}")
        endif()
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

if(DUI_OS_WINDOWS)
    # Windows: compile the tool into the source tree at CONFIGURE time (only
    # when missing or the sources changed), so it stays up-to-date without
    # triggering Device Guard at build time.

    set(TOOL_EXE "${CMAKE_BINARY_DIR}/tools/xml_to_code.exe")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tools")
    set(TOOL_SRC "${DUI_SRC_ROOT_DIR}/tools/xml_to_code.cpp")
    set(PUGIXML_SRC "${DUI_SRC_ROOT_DIR}/third_party/xml/pugixml.cpp")
    set(PUGIXML_DIR "${DUI_SRC_ROOT_DIR}/third_party/xml")

    # Rebuild at configure time if source is newer (idempotent across examples)
    if("${TOOL_SRC}" IS_NEWER_THAN "${TOOL_EXE}" OR "${PUGIXML_SRC}" IS_NEWER_THAN "${TOOL_EXE}")
        message(STATUS "Rebuilding xml_to_code.exe (source changed)")
        dui_build_msvc_tool(_tool_ok "${TOOL_SRC};${PUGIXML_SRC}" "${TOOL_EXE}" "-I${PUGIXML_DIR}")
        if(NOT _tool_ok)
            message(WARNING "Failed to rebuild xml_to_code.exe automatically.\n"
                            "Compile it manually in a VS Developer Command Prompt:\n"
                            "  cl /nologo /std:c++17 /O2 /EHsc -I${PUGIXML_DIR} ${TOOL_SRC} ${PUGIXML_SRC} /Fe:${TOOL_EXE}")
        endif()
    endif()

    if(NOT EXISTS "${TOOL_EXE}")
        message(FATAL_ERROR "xml_to_code.exe not found at ${TOOL_EXE}.\n"
                "Compile it manually in a VS Developer Command Prompt:\n"
                "  cl /nologo /std:c++17 /O2 /EHsc -I${PUGIXML_DIR} ${TOOL_SRC} ${PUGIXML_SRC} /Fe:${TOOL_EXE}")
    endif()

    add_custom_command(
        OUTPUT "${GENERATED_SRC}"
        COMMAND "${TOOL_EXE}" ${GEN_TOOL_GLOBAL_ARGS} ${GEN_RESOURCE_ARGS} "${GENERATED_SRC}" "${GEN_FUNC_NAME}" ${XML_INPUT_FILES}
        DEPENDS "${TOOL_EXE}" ${XML_INPUT_FILES}
        COMMENT "Generating C++ UI code from XML: ${GENERATED_SRC}"
    )
else()
    # macOS / Linux / FreeBSD: build the tool once at CONFIGURE time into the
    # build tree and share it across all *_gen examples, avoiding rebuilding the
    # tool once per example at build time.
    set(TOOL_SRC "${DUI_SRC_ROOT_DIR}/tools/xml_to_code.cpp")
    set(PUGIXML_SRC "${DUI_SRC_ROOT_DIR}/third_party/xml/pugixml.cpp")
    set(PUGIXML_DIR "${DUI_SRC_ROOT_DIR}/third_party/xml")
    set(TOOL_EXE "${CMAKE_BINARY_DIR}/tools/xml_to_code")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tools")

    # Rebuild at configure time if source is newer (idempotent across examples)
    if("${TOOL_SRC}" IS_NEWER_THAN "${TOOL_EXE}" OR "${PUGIXML_SRC}" IS_NEWER_THAN "${TOOL_EXE}")
        message(STATUS "Rebuilding xml_to_code tool (source changed)")
        execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}" -std=c++17 -O2 "-I${PUGIXML_DIR}"
                    "${TOOL_SRC}" "${PUGIXML_SRC}" -o "${TOOL_EXE}"
            WORKING_DIRECTORY "${DUI_SRC_ROOT_DIR}"
            RESULT_VARIABLE _build_result
            ERROR_VARIABLE _tool_err
        )
        if(NOT _build_result EQUAL 0)
            message(FATAL_ERROR "Failed to build xml_to_code tool: ${_tool_err}")
        endif()
    endif()

    if(NOT EXISTS "${TOOL_EXE}")
        message(FATAL_ERROR "xml_to_code tool not found at ${TOOL_EXE}")
    endif()

    add_custom_command(
        OUTPUT "${GENERATED_SRC}"
        COMMAND "${TOOL_EXE}" ${GEN_TOOL_GLOBAL_ARGS} ${GEN_RESOURCE_ARGS} "${GENERATED_SRC}" "${GEN_FUNC_NAME}" ${XML_INPUT_FILES}
        DEPENDS "${TOOL_EXE}" ${XML_INPUT_FILES}
        COMMENT "Generating C++ UI code from XML: ${GENERATED_SRC}"
    )
endif()

add_custom_target("${PROJECT_NAME}_gen_xml_code" DEPENDS "${GENERATED_SRC}")
list(APPEND DUI_GENERATED_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}")

# Kept for callers that still read it; dui_finalize_app() wires the dependency onto the
# application target by name, so nothing has to consume this any more.
set(DUI_GEN_CODE_SRC "${GENERATED_SRC}")
