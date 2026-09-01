#ifndef EXAMPLES_CONTROLS_NATIVE_MENU_HELPER_H_
#define EXAMPLES_CONTROLS_NATIVE_MENU_HELPER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*NativeMenuAboutCallback)(void* userData);

/** Shows the controls settings menu as a native macOS NSMenu.
 *  This avoids the custom Skia menu's focus/dismiss/submenu issues on macOS.
 */
void ShowNativeControlsMenu(void* nsWindow,
                            float x,
                            float y,
                            NativeMenuAboutCallback onAbout,
                            void* userData);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLES_CONTROLS_NATIVE_MENU_HELPER_H_
