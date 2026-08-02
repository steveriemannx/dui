# Third-Party Libraries Used

All third-party dependencies are vendored under `third_party/` (skia and SDL3 are built automatically at configure time; the CEF binary distribution is downloaded automatically when missing).

| Name     |Code subdirectory                 | Purpose                                 | License file        | License category          |
| :---     | :---                      |:---                                  |:---               |:---               |
|apng      |third_party/libpng  | APNG image format support                     |zlib/libpng License|zlib/libpng license, a permissive open-source license|
|libpng    |third_party/libpng  | PNG image format support                      |[libpng.LICENSE.txt](../licenses/libpng.LICENSE.txt)|Custom BSD-style permissive license|
|zlib      |third_party/zlib    | PNG/APNG image format support<br>Zip file decompression  |[zlib.LICENSE.txt](../licenses/zlib.LICENSE.txt)|zlib license, a permissive open-source license|
|cximage   |third_party/cximage | ICO image format support                      |[cximage.LICENSE.txt](../licenses/cximage.LICENSE.txt)|MIT-style license (non-standard MIT)|
|giflib    |third_party/giflib  | GIF image format support                      |[giflib.LICENSE.txt](../licenses/giflib.LICENSE.txt)|MIT license|
|libwebp   |third_party/libwebp | WebP image format support                     |[libWebP.LICENSE.txt](../licenses/libwebp.LICENSE.txt)|BSD 3-clause license|
|stb_image |third_party/stb_image| BMP image format support<br>Image resizing     |[stb_image.LICENSE.txt](../licenses/stb_image.LICENSE.txt)|MIT license/public domain|
|libjpeg-turbo|third_party/libjpeg-turbo| JPEG image format support             |[libjpeg-turbo.LICENSE.md](../licenses/libjpeg-turbo.LICENSE.md)|IJG license and modified BSD 3-clause license|
|nanosvg   |third_party/svg      | SVG image format support                     |[nanosvg.LICENSE.txt](../licenses/nanosvg.LICENSE.txt)|zlib license|
|pugixml   |third_party/xml      | Parsing resource-description XML               |[pugixml.LICENSE.txt](../licenses/pugixml.LICENSE.txt)|MIT license|
|ConvertUTF|third_party/convert_utf| UTF-8/UTF-16 encoding conversion         |[llvm.LICENSE.txt](../licenses/llvm.LICENSE.txt)|Apache License Version 2.0 primarily,<br>supplemented by the LLVM exception terms,<br>with legacy license compatibility for historical versions|
|skia      |third_party/skia (vendored, built automatically) | UI library rendering engine<br>SVG image format support<br>Lottie JSON animation support|[skia.LICENSE.txt](../licenses/skia.LICENSE.txt)|BSD 3-clause license|
|SDL       |third_party/SDL3 (vendored, built automatically) | Cross-platform window management                     |[SDL.LICENSE.txt](../licenses/SDL.LICENSE.txt)|zlib license|
|duilib    |                            | NIM_Duilib_Framework<br>is developed based on duilib  |[duilib.LICENSE.txt](../licenses/duilib.LICENSE.txt)|BSD 2-clause license|
|NIM_Duilib<br>Framework|               | This project is developed based on<br>NIM_Duilib_Framework   |[NIM_Duilib_Framework.LICENSE.txt](../licenses/NIM_Duilib_Framework.LICENSE.txt)|MIT license|
|libcef    |third_party/libcef   | Loads the CEF module|[libcef.LICENSE.txt](../licenses/libcef.LICENSE.txt)|BSD 3-clause license|
|udis86    |third_party/libudis86| Computes the shortest instruction length for complete disassembly         |[udis86.LICENSE.txt](../licenses/udis86.LICENSE.txt)|BSD 2-clause license|
|WebView2  |third_party/<br>Microsoft.Web.WebView2| WebView2 control support |[Microsoft.Web.WebView2.LICENSE.txt](../licenses/Microsoft.Web.WebView2.LICENSE.txt)|BSD 3-clause license|
|libpag    |third_party/libpag   | PAG animation file support<br>(disabled by default; see below) |[libpag.LICENSE.txt](../licenses/libpag.LICENSE.txt)|Apache License Version 2.0 (primary)<br>libpag depends on many third-party components with<br>various licenses; see the files in the<br>`third_party/libpag/licenses`<br>directory. If libpag's licensing<br>(including the primary license and third-party component licenses)<br>is a concern, libpag can be left disabled.|

## About the PAG Animation File Format
* PAG animation files are currently supported only on Windows; other platforms are not supported yet
* PAG animation file format support is disabled by default (because libpag.lib and libpag.dll must be compiled yourself and placed in the project for it to build and run normally)
* How to enable PAG animation format support:    
(1) Open the [`msvc/PropertySheets/LibPagSettings.props`](../msvc/PropertySheets/LibPagSettings.props) file with a text editor and change the `LibPagEnabled` variable's value to `1`    
(2) Compile the libpag library following the documentation: [`third_party/libpag/windows/libpag-build.md`](../third_party/libpag/windows/libpag-build.md)     
* The primary license of the libpag library is Apache License Version 2.0, and the third-party components it depends on have many licenses,<br>see the files in the `third_party/libpag/licenses` directory.<br>If libpag's licensing (including the primary license and third-party component licenses) is a concern, libpag can be left disabled.
