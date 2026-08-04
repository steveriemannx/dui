#pragma once

// Dui version macro definitions (all prefixed with DUI_; uniform convention; version numbers max out at 65535)
// The last two numbers currently use a 4-digit year and 2-digit month/day; you can define your own rules when actually using them
#define DUI_VER_MAJOR          1
#define DUI_VER_MINOR          0
#define DUI_VER_PATCH          2026
#define DUI_VER_BUILD          0416

// Helper macro: convert a number to a string
#define STRINGIFY(x)              #x
#define TOSTRING(x)               STRINGIFY(x)

// Assemble the version number (automatically generates a "1.0.0.1" style version string)
#define DUI_VER_FILEVERSION_STR  TOSTRING(DUI_VER_MAJOR) "." \
                                    TOSTRING(DUI_VER_MINOR) "." \
                                    TOSTRING(DUI_VER_PATCH) "." \
                                    TOSTRING(DUI_VER_BUILD)

#define DUI_VER_PRODVERSION_STR  DUI_VER_FILEVERSION_STR

// Numeric version (for use by the RC compiler)
#define DUI_VER_FILEVERSION    DUI_VER_MAJOR,DUI_VER_MINOR,DUI_VER_PATCH,DUI_VER_BUILD
#define DUI_VER_PRODVERSION    DUI_VER_MAJOR,DUI_VER_MINOR,DUI_VER_PATCH,DUI_VER_BUILD

// Company/product information
#define DUI_VER_COMPANY        "https://github.com/steveriemannx/dui"
#define DUI_VER_DESC           "dui DLL"
#define DUI_VER_COPYRIGHT      "Copyright (C) 2026"
#define DUI_VER_PRODUCTNAME    "dui"
