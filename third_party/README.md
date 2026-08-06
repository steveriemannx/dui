## Notes on Third-Party Source Code
1. The third_party directory contains the source code of third-party libraries;
2. Third-party source code is generally not modified in this project, to avoid difficulties when upgrading later;
3. Third-party libraries are usually upgraded to the latest version on a regular basis, to provide the best features and reduce defects.

## Current Usage of Each Third-Party Library
| Name        | <div style="width: 90px">Updated</div>   | Version | <div style="width: 40px">Modified</div> | Source URL | Remarks |
| :---        | :---       | :---  |:---    | :---    |:---  |
| apng        | 2025-09-12 |v1.6.50|No |https://sourceforge.net/projects/libpng-apng/ |Supported through libpng + libpng-1.6.50-apng.patch |
| libpng      | 2025-09-12 |v1.6.50|Yes |https://sourceforge.net/projects/libpng/|See the notes below; the following files are modified:<br>CMakeLists.txt<br>projects\vstudio\libpng\libpng.vcxproj <br>pngstruct.h|
| cximage     | 2024-05-30 |v7.02  |No |https://sourceforge.net/projects/cximage/|No longer maintained since 2011-02-11|
| giflib      | 2026-04-14 |v6.1.3 |No |https://sourceforge.net/projects/giflib/||
| libwebp     | 2025-10-21 |v1.6.0 |No |https://github.com/webmproject/libwebp|tag/v1.6.0; uses the subdirectories under src; after overlaying, delete unnecessary files (basic method: diff the old and new versions, check file additions/deletions/modifications, then sync; the project only uses the image decoding feature, not encoding; the config.h file is currently not used)|
| stb_image   | 2025-10-21 |stb_image v2.30 <br> stb_image_resize2 v2.15|No |https://github.com/nothings/stb|No modification; taken directly from master|
| libjpeg-turbo| 2026-04-16 |v3.1.4.1|No |https://github.com/libjpeg-turbo/libjpeg-turbo|No modification; taken directly from its release code and lib|
| svg         | 2025-10-21 |None (2025-09-28)     |Yes |https://github.com/memononen/nanosvg| Code updated to 2025-09-28; modifications are in the commit history|
| zlib        | 2025-03-11 |v1.3.1（2024-01-22） |Yes |https://github.com/madler/zlib | Modified zlib.h and CMakeLists.txt, and renamed zconf.h to zconf_msvc.h<br>If the file names or count change, the VC project must be updated|
| xml         | 2025-03-11 |v1.15（2025-01-11）  |No |https://github.com/zeux/pugixml| pugixml|
| convert_utf | 2025-03-11 |v20.1.0|No |https://releases.llvm.org      | Download the latest source package, extract it, and find these two files |
| udis86      | 2025-04-11 |v1.7.2 |Yes |https://sourceforge.net/projects/udis86/      |  udint.h is modified|
| libcef/libcef_win_109|2025-03-10|<div style="width: 160px">cef_binary 109.1.18+gf1c41e4<br>chromium-109.0.5414.120</div>|Yes |<div style="width: 360px">x64: https://cef-builds.spotifycdn.com/index.html#windows64:109 <br> Win32: https://cef-builds.spotifycdn.com/index.html#windows32:109</div>| <div style="width: 360px">x64: https://cef-builds.spotifycdn.com/cef_binary_109.1.18%2Bgf1c41e4%2Bchromium-109.0.5414.120_windows64.tar.bz2 <br> Win32: https://cef-builds.spotifycdn.com/cef_binary_109.1.18%2Bgf1c41e4%2Bchromium-109.0.5414.120_windows32.tar.bz2</div>|
| libcef (cef_binary)|2026-08-06|<div style="width: 160px">cef_binary_142.0.10+g29548e2+chromium-142.0.7444.135</div>|No |https://cef-builds.spotifycdn.com/index.html| Downloaded and extracted to third_party/libcef/cef_binary automatically at configure time (cmake/dui_deps.cmake); the wrapper is built from it (third_party/CMakeLists.txt). CEF 109 (Win7): see the libcef_win_109 row above |
| WebView2      |2025-06-28|1.0.3296.44 |No | Installed via the NuGet package WebView2 SDK: <br>Microsoft.Web.WebView2| |
| libpag        |2025-11-13|main branch |No | https://github.com/Tencent/libpag| No modification |
| skia          |2026-08-02|skia-dui-0.1.0 |No |https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.0.zip| Tagged from the dui branch of the fork; includes the expat patch. Downloaded and extracted to third_party/skia automatically by CMake at configure time (cmake/dui_deps.cmake), built with gn + ninja at make time |
| SDL3          |2026-08-04|v3.4.14 |No |https://github.com/libsdl-org/SDL/releases/download/release-3.4.14/SDL3-3.4.14.zip| Official source package. Downloaded and extracted to third_party/SDL3 automatically by CMake at configure time (cmake/dui_deps.cmake), built at make time |

## libpng Update Notes
1. Download the latest release package: https://github.com/pnggroup/libpng/ or https://sourceforge.net/projects/libpng/
2. Extract the downloaded package, enter the extracted directory, and following the INSTALL instructions, run from the VS command prompt: `nmake -f scripts\makefile.vcwin32`, which generates the `pnglibconf.h` file
3. Then overlay the whole directory (excluding .lib and .obj files) onto `dui\third_party\libpng`, keeping it identical to the downloaded package; add missing files, delete extra ones, and add newly added files to git
4. Check the `pnglibconf.h` file and keep it in sync as needed
5. Do not update the `libpng\projects\vstudio\libpng\libpng.vcxproj` project file lightly, unless it is required by file changes (you can run `nmake -f scripts\makefile.vcwin32` in both the old and new official packages, compare the generated vcxproj, and then decide whether it needs modification)
6. Sync the CMakeLists.txt changes: compare the old and new CMakeLists.txt, and port the modifications made on the old version to the new one
7. Apply the APNG support patch:
 - Patch download: https://sourceforge.net/projects/libpng-apng/ ; if the matching patch cannot be found, search for `libpng PNG_APNG_SUPPORTED` and try to locate it
 - If no matching patch version is available, consider postponing the libpng update
 - Update step 1: extract the `libpng-1.6.50-apng.patch.gz` file to get `libpng-1.6.50-apng.patch`, and put it in the `dui\third_party\libpng` directory
 - Update step 2: from the command prompt, enter the `dui\third_party\libpng` directory and run: `git apply .\libpng-1.6.50-apng.patch`
 - Update step 3: after applying the patch, check, test and commit.
8. Modify `dui\third_party\libpng\pngstruct.h`, syncing the changes;

## libcef Update Notes
1. The CMakeLists.txt file and VC project files of libcef are modified, as follows:
 - libcef/libcef_win/include/cef_config.h (modification: the CEF_V8_ENABLE_SANDBOX macro is only defined on 64-bit systems; it is not needed on 32-bit systems)
 - libcef/libcef_win/CMakeLists.txt
 - libcef/libcef_win/libcef_dll_wrapper.vcxproj
 - libcef/libcef_win/libcef_dll_wrapper.vcxproj.filters
 - libcef/libcef_win_109/CMakeLists.txt
 - libcef/libcef_win_109/libcef_dll_wrapper_109.vcxproj
 - libcef/libcef_win_109/libcef_dll_wrapper_109.vcxproj.filters
 - libcef/libcef_linux/CMakeLists.txt
 - libcef/libcef_macos/CMakeLists.txt

The files above are modified; keep the differences in mind when updating.

2. Other content that needs modification in the project
 - `README.md` — the CEF version number must be updated
 - `docs\CEF.md`
 - `build\build_dui_all_in_one.sh`
