# Embed image files into C++ header at build time (base64 + memfd)
#
# Usage:
#   set(EMBED_IMG_FILES icon.png window-close.svg)
#   include(duilib_embed_img.cmake)
#
# Generates "embedded_images.h" with GetEmbeddedXxxPath() for each image.

if(NOT DEFINED EMBED_IMG_FILES)
    message(FATAL_ERROR "EMBED_IMG_FILES must be set")
endif()

set(TOOL_SRC "${DUILIB_SRC_ROOT_DIR}/cmake/img_to_header.cpp")
set(TOOL_BIN "${CMAKE_CURRENT_BINARY_DIR}/img_to_header")
set(GENERATED_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/embedded_images.h")
set(RESOURCES_DIR "${DUILIB_ROOT}/bin/resources")

add_custom_command(
    OUTPUT "${TOOL_BIN}"
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -O2 "${TOOL_SRC}" -o "${TOOL_BIN}"
    DEPENDS "${TOOL_SRC}"
    COMMENT "Building img_to_header tool"
)

set(IMG_INPUT_FILES)
foreach(img ${EMBED_IMG_FILES})
    if(IS_ABSOLUTE "${img}")
        list(APPEND IMG_INPUT_FILES "${img}")
    else()
        file(GLOB_RECURSE found "${RESOURCES_DIR}/themes/*/${img}")
        if(NOT found)
            file(GLOB_RECURSE found "${RESOURCES_DIR}/*/${img}")
        endif()
        if(NOT found)
            file(GLOB found "${DUILIB_ROOT}/bin/${img}")
        endif()
        if(found)
            list(APPEND IMG_INPUT_FILES ${found})
        else()
            message(WARNING "Image not found: ${img}")
        endif()
    endif()
endforeach()

add_custom_command(
    OUTPUT "${GENERATED_HEADER}"
    COMMAND "${TOOL_BIN}" "${GENERATED_HEADER}" ${IMG_INPUT_FILES}
    DEPENDS "${TOOL_BIN}" ${IMG_INPUT_FILES}
    COMMENT "Embedding images into: ${GENERATED_HEADER}"
)

add_custom_target(embed_img DEPENDS "${GENERATED_HEADER}")
set(DUILIB_EMBED_IMG_HEADER "${GENERATED_HEADER}" CACHE INTERNAL "")
