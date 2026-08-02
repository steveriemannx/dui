cmake_minimum_required(VERSION 3.18)

set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(embedxml CXX)

get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")

# Embed basic.xml into the binary at build time (C++ xml_to_header tool, no Python)
set(EMBED_XML_FILES basic.xml)
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_embed_xml.cmake")

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
