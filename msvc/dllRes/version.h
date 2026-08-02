#pragma once

// Duilib version macro definitions (all prefixed with DUILIB_; uniform convention; version numbers max out at 65535)
// The last two numbers currently use a 4-digit year and 2-digit month/day; you can define your own rules when actually using them
#define DUILIB_VER_MAJOR          1
#define DUILIB_VER_MINOR          0
#define DUILIB_VER_PATCH          2026
#define DUILIB_VER_BUILD          0416

// Helper macro: convert a number to a string
#define STRINGIFY(x)              #x
#define TOSTRING(x)               STRINGIFY(x)

// Assemble the version number (automatically generates a "1.0.0.1" style version string)
#define DUILIB_VER_FILEVERSION_STR  TOSTRING(DUILIB_VER_MAJOR) "." \
                                    TOSTRING(DUILIB_VER_MINOR) "." \
                                    TOSTRING(DUILIB_VER_PATCH) "." \
                                    TOSTRING(DUILIB_VER_BUILD)

#define DUILIB_VER_PRODVERSION_STR  DUILIB_VER_FILEVERSION_STR

// Numeric version (for use by the RC compiler)
#define DUILIB_VER_FILEVERSION    DUILIB_VER_MAJOR,DUILIB_VER_MINOR,DUILIB_VER_PATCH,DUILIB_VER_BUILD
#define DUILIB_VER_PRODVERSION    DUILIB_VER_MAJOR,DUILIB_VER_MINOR,DUILIB_VER_PATCH,DUILIB_VER_BUILD

// Company/product information
#define DUILIB_VER_COMPANY        "https://github.com/rhett-lee/nim_duilib"
#define DUILIB_VER_DESC           "nim_duilib DLL"
#define DUILIB_VER_COPYRIGHT      "Copyright (C) 2026"
#define DUILIB_VER_PRODUCTNAME    "nim_duilib"
