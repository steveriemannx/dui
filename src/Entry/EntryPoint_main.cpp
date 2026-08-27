// dui_entry: a Qt-like platform entry shim.
//
// On Windows a GUI executable needs WinMain/wWinMain.  This small library
// provides that entry point and forwards to the normal C++ main(), so app code
// can be written exactly like Qt:
//
//     int main(int argc, char** argv) { ... }
//     or
//     int main() { ... }
//
// On non-Windows platforms the normal main() is already the system entry, so
// this file only keeps the static library non-empty.

#if defined(_WIN32)

#include <windows.h>
#include <stdlib.h>

// The C++ main() that the application provides.
int main(int argc, char** argv);

#if defined(__MINGW32__)

int APIENTRY WinMain(_In_ HINSTANCE /*hInstance*/,
                     _In_opt_ HINSTANCE /*hPrevInstance*/,
                     _In_ LPSTR /*lpCmdLine*/,
                     _In_ int /*nCmdShow*/)
{
    // The CRT has already parsed the command line into __argc/__argv.
    return main(__argc, __argv);
}

#else

int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/,
                      _In_opt_ HINSTANCE /*hPrevInstance*/,
                      _In_ LPWSTR /*lpCmdLine*/,
                      _In_ int /*nCmdShow*/)
{
    // The CRT has already parsed the command line into __argc/__argv.
    return main(__argc, __argv);
}

#endif

#else

// Non-Windows placeholder so the archive is not empty when linked.
int dui_entry_placeholder() { return 0; }

#endif
