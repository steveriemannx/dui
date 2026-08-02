cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(hello_code CXX)

get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Mode 3 (pure code): the UI is built entirely in C++ code (MainForm::OnInitWindow),
# no layout XML and no build-time generator.
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")

# Embed bin/resources into the executable as a custom binary archive (Qt qrc style, no zip)
set(EMBED_RES_DIR "${DUILIB_ROOT}/bin/resources")
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_embed_res.cmake")

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
