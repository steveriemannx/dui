cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(stardesk_xml CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Reuse the StarDesk feature code (network / sessions / capture / codec /
# file transfer / discovery / config). Only the UI layer is XML here.
# NOTE: dui_bin.cmake joins each entry onto DUI_PROJECT_SRC_DIR, so relative
# paths are required (absolute paths would double-prefix).
set(STARDESK_SRC_REL "../../examples/stardesk/src")
set(DUI_SRC_SUB_DIRS src src/ui
    ${STARDESK_SRC_REL}/app ${STARDESK_SRC_REL}/net ${STARDESK_SRC_REL}/session
    ${STARDESK_SRC_REL}/codec ${STARDESK_SRC_REL}/platform ${STARDESK_SRC_REL}/filetx)
if(APPLE)
    list(APPEND DUI_SRC_SUB_DIRS ${STARDESK_SRC_REL}/platform/apple)
elseif(WIN32)
    list(APPEND DUI_SRC_SUB_DIRS ${STARDESK_SRC_REL}/platform/win)
else()
    list(APPEND DUI_SRC_SUB_DIRS ${STARDESK_SRC_REL}/platform/x11)
endif()

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# ---- StarDesk-specific additions ----
# libpng headers for screen-frame encoding/decoding
target_include_directories(${PROJECT_NAME} PRIVATE "${DUI_SRC_ROOT_DIR}/third_party/libpng")

if(APPLE)
    # ScreenCaptureKit (macOS 14+) + CoreMedia (CMTime) for screen capture.
    # Plain signature: dui_bin_macos.cmake already linked this target plain.
    target_link_libraries(${PROJECT_NAME}
        "-framework ScreenCaptureKit" "-framework CoreMedia")
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
else()
    find_package(X11 REQUIRED)
    find_library(XTST_LIB Xtst)
    if(X11_FOUND AND XTST_LIB)
        target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} ${XTST_LIB})
    endif()
endif()

# Sync the XML skins into the runtime resource tree (like the hello example)
set(STARDESK_XML_DIR "${DUI_ROOT}/bin/resources/themes/default/stardesk_xml")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${STARDESK_XML_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/main.xml"      "${STARDESK_XML_DIR}/main.xml"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/remote.xml"     "${STARDESK_XML_DIR}/remote.xml"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/ask.xml"        "${STARDESK_XML_DIR}/ask.xml"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/file_send.xml"  "${STARDESK_XML_DIR}/file_send.xml"
    COMMENT "Copying stardesk_xml skins into the runtime resources tree")
