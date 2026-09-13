cmake_minimum_required(VERSION 3.18)

option(DUI_RUN_GUI_TESTS "Build tests that create native GUI windows" OFF)

# Include this file after the repository's src/CMakeLists.txt so the core
# library target is available. This keeps test wiring independent from native backend.
if(NOT TARGET dui)
    message(FATAL_ERROR "tests requires the dui target; add_subdirectory(src) before add_subdirectory(tests)")
endif()

add_executable(dui_core_tests
    core_tests.cpp
)

target_compile_features(dui_core_tests PRIVATE cxx_std_20)
target_compile_definitions(dui_core_tests PRIVATE
    DUI_TEST_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/.."
)
# The binding module is optional, so tests covering it must not reference symbols
# that were never compiled in.
if(DUI_MVVM_AVAILABLE)
    target_compile_definitions(dui_core_tests PRIVATE DUI_MVVM=1)
endif()
if(WIN32)
    target_compile_definitions(dui_core_tests PRIVATE UNICODE _UNICODE)
    if(MSVC)
        if(DUI_MD)
            set_property(TARGET dui_core_tests PROPERTY MSVC_RUNTIME_LIBRARY
                "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
        else()
            set_property(TARGET dui_core_tests PROPERTY MSVC_RUNTIME_LIBRARY
                "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()
    endif()
endif()
target_include_directories(dui_core_tests PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../include"
)
# The text-encoding tests include dui/RenderSkia/SkUtils.h, and that public header
# pulls in Skia headers, so the test needs Skia's include root just as dui does.
target_include_directories(dui_core_tests PRIVATE "${DUI_SKIA_SRC_ROOT_DIR}")
target_link_libraries(dui_core_tests PRIVATE
    dui
    dui-cximage
    dui-webp
    png_static
    dui_skia_libs
)

if(WIN32)
    target_link_libraries(dui_core_tests PRIVATE
        Comctl32 Imm32 Opengl32 User32 shlwapi dwmapi
    )
endif()

add_test(NAME dui_core_tests COMMAND dui_core_tests)
add_test(NAME dui_global_startup COMMAND dui_core_tests --global-startup)
add_test(NAME dui_global_failure_rollback COMMAND dui_core_tests --global-failure)
if(APPLE)
    add_test(NAME dui_timer_message_loop COMMAND dui_core_tests --timer-loop)
endif()

if(DUI_RUN_GUI_TESTS)
    add_executable(dui_gui_tests
        gui_tests.cpp
    )
    target_compile_features(dui_gui_tests PRIVATE cxx_std_20)
    target_compile_definitions(dui_gui_tests PRIVATE
        DUI_TEST_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/.."
    )
    if(WIN32)
        target_compile_definitions(dui_gui_tests PRIVATE UNICODE _UNICODE)
        if(MSVC)
            if(DUI_MD)
                set_property(TARGET dui_gui_tests PROPERTY MSVC_RUNTIME_LIBRARY
                    "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
            else()
                set_property(TARGET dui_gui_tests PROPERTY MSVC_RUNTIME_LIBRARY
                    "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            endif()
        endif()
    endif()
    target_include_directories(dui_gui_tests PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../include"
    )
    target_link_libraries(dui_gui_tests PRIVATE
        dui
        dui-cximage
        dui-webp
        png_static
        dui_skia_libs
    )
    add_test(NAME dui_gui_tests COMMAND dui_gui_tests)
    set_tests_properties(dui_gui_tests PROPERTIES LABELS "gui")
endif()
