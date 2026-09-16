# Pick the program CMake drives its generated build files with.
#
# CMAKE_MAKE_PROGRAM is the program itself, whatever the generator: under Ninja it
# must be ninja -- CMake rejects anything else with "The detected version of Ninja
# () is less than the version of Ninja required by CMake (1.3)" -- so this file
# only touches the make generators.
#
# On FreeBSD bmake *is* the system make (/usr/bin/bmake is the same binary as
# /usr/bin/make), and on macOS Homebrew installs the same command name.  Neither
# platform then needs GNU make (gmake) just to build dui.  Linux and Windows keep
# their defaults: Linux ships GNU make, Windows uses the Visual Studio generator,
# which has no make program at all.
#
# Wired in through CMAKE_PROJECT_TOP_LEVEL_INCLUDES in CMakePresets.json, so it
# runs before project() and can look at CMAKE_GENERATOR.
if(CMAKE_GENERATOR MATCHES "Makefiles")
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "FreeBSD" OR CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        find_program(DUI_BMAKE_PROGRAM NAMES bmake)
        if(DUI_BMAKE_PROGRAM)
            set(CMAKE_MAKE_PROGRAM "${DUI_BMAKE_PROGRAM}" CACHE FILEPATH
                "Build program for the make generators" FORCE)
        else()
            # Not fatal: the system make still builds dui, it is just slower to
            # depend on and, on FreeBSD, not installed by default.
            message(WARNING
                "bmake was not found, so CMake keeps \"${CMAKE_MAKE_PROGRAM}\". "
                "On macOS: brew install bmake")
        endif()
    endif()
endif()
