# Drive the make generators with bmake.
#
# CMAKE_MAKE_PROGRAM is the program CMake runs, whatever the generator is: under
# Ninja it has to be ninja, and CMake rejects anything else ("The detected version
# of Ninja () is less than the version of Ninja required by CMake (1.3)"), so this
# only applies to the make generators.
#
# FreeBSD ships bmake as its system make (/usr/bin/bmake is the same binary as
# /usr/bin/make) and macOS gets the same command name from Homebrew, so neither
# platform needs GNU make just to build dui.  It is required rather than merely
# preferred: without bmake the configure stops and says how to install it.
#
# Included by the top-level CMakeLists.txt before project(), which is early enough
# that CMAKE_MAKE_PROGRAM is empty unless the user named one, so it works for a
# plain `cmake -S . -B build` as well as for a preset.  Two ways out:
#
#   cmake -S . -B build -DCMAKE_MAKE_PROGRAM=<program>   name the program to use
#   cmake -S . -B build -DDUI_USE_BMAKE=OFF              let the generator pick
#
option(DUI_USE_BMAKE "Drive the make generators with bmake (FreeBSD/macOS)" ON)

if(DUI_USE_BMAKE AND CMAKE_GENERATOR MATCHES "Makefiles"
        AND (CMAKE_HOST_SYSTEM_NAME STREQUAL "FreeBSD"
             OR CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        AND NOT CMAKE_MAKE_PROGRAM)
    find_program(DUI_BMAKE_PROGRAM NAMES bmake)
    if(DUI_BMAKE_PROGRAM)
        set(CMAKE_MAKE_PROGRAM "${DUI_BMAKE_PROGRAM}" CACHE FILEPATH
            "Build program for the make generators" FORCE)
    else()
        message(FATAL_ERROR
            "bmake was not found, and ${CMAKE_HOST_SYSTEM_NAME} builds dui with it.\n"
            "  macOS : brew install bmake\n"
            "  FreeBSD: it is part of the base system, so check PATH\n"
            "Or name another program with -DCMAKE_MAKE_PROGRAM=<program>, or let the\n"
            "generator pick one with -DDUI_USE_BMAKE=OFF.")
    endif()
endif()
