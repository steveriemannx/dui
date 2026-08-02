# Common Image Size Specifications for Desktop Development UI Assets

In desktop development (e.g. Windows, macOS, Linux applications), image sizing for UI assets must primarily take into account **screen resolution adaptation** (normal screens, Retina screens, 4K screens), **icon function scenarios** (toolbar, status bar, menus, etc.) and **system/framework specifications**. The core principle is "multi-resolution adaptation + crisp, blur-free pixels". Below are the common specifications and design recommendations organized by **asset type**:


## I. Core UI Icon Sizes (most commonly used)
Icons in desktop applications (e.g. feature buttons, toolbar icons, sidebar icons) must adapt to screens with different DPIs (dots per inch), and typically follow a "base size + multiple scaling" design pattern (e.g. 1x, 2x, 3x versions) to avoid stretched blurriness.

| Icon type          | Base size (1x normal screen) | 2x HD screen (Retina/macOS) | 3x UHD screen (4K/large devices) | Typical use cases                  |
|-------------------|-----------------------|---------------------------|-----------------------------|---------------------------|
| Toolbar/feature icons   | 16×16 px              | 32×32 px                  | 48×48 px                    | Buttons in the app's top toolbar (e.g. "Save", "Undo"), sidebar feature entries |
| Status bar icons        | 16×16 px              | 32×32 px                  | 48×48 px                    | Windows taskbar notification icons, macOS menu bar icons |
| Menu icons          | 24×24 px              | 48×48 px                  | 72×72 px                    | Auxiliary icons in dropdown menus (e.g. small icons next to "Cut", "Paste") |
| Panel/card icons     | 32×32 px              | 64×64 px                  | 96×96 px                    | Data panel titles, core feature card icons (e.g. "User Center", "Settings" icons) |
| Large action button icons    | 48×48 px              | 96×96 px                  | 144×144 px                  | Core action buttons on the main UI (e.g. "New Project", "Import File" large buttons) |


## II. App Icon Sizes (desktop shortcuts/launch icons)
The app icon is the core identifier users recognize an application by. It must conform to system-native specifications (e.g. Windows ICO format, macOS ICNS format) and cover all scenarios from desktop shortcuts to taskbar thumbnails.

### 1. Windows System App Icons
Windows has detailed size requirements for app icons; a multi-size set (usually packaged as an ICO file) must be provided to cover different display scenarios:
- 16×16 px: taskbar thumbnails, File Explorer icons
- 32×32 px: desktop shortcuts (normal screens), folder icon overlays
- 48×48 px: Start menu list icons
- 64×64 px: File Explorer preview (medium icon view)
- 128×128 px: Start menu tiles (small), desktop shortcuts (HD screens)
- 256×256 px: Start menu tiles (large), app installer icons
- 512×512 px: desktop shortcuts on 4K screens, app store display images

### 2. macOS System App Icons
macOS is centered on Retina screens; app icons must provide 1x and 2x versions (packaged as ICNS files):
- 16×16 px (1x) / 32×32 px (2x): Launchpad small icons, menu bar icons
- 32×32 px (1x) / 64×64 px (2x): Finder icons (list view)
- 128×128 px (1x) / 256×256 px (2x): Launchpad standard icons, desktop shortcuts
- 256×256 px (1x) / 512×512 px (2x): Finder icons (large icon view)
- 512×512 px (1x) / 1024×1024 px (2x): App Store display images, launch animation icons


## III. UI Element Image Sizes
Elements such as background images, avatars, and popup illustrations in desktop applications must be designed against the UI layout and display device dimensions to avoid distortion or stretching.

### 1. Background/Cover Images
- Window background images: design at "mainstream window size + 1.5x headroom", e.g. 1920×1080 px (fits 1080P windows), 2560×1440 px (fits 2K windows). Prefer PNG (transparent background) or JPG (opaque background).
- Feature module cover images: e.g. "welcome page" and "empty state" illustrations; common sizes are 800×450 px (16:9 ratio, fits most window widths) and 600×600 px (square, fits compact layouts).

### 2. Avatars/User Avatars
- Small avatars (in lists): 24×24 px, 32×32 px (1:1 ratio to avoid stretch distortion).
- Medium avatars (profile center): 64×64 px, 80×80 px (slight rounded corners supported, consistent with modern UI style).
- Large avatars (profile pages): 128×128 px, 256×256 px (must be high-resolution to avoid pixel blur).

### 3. Popup/Notification Images
- Popup icons (e.g. warning, success notifications): 48×48 px, 64×64 px (proportionally coordinated with the popup text area; usually located on the left side of the popup).
- Tutorial/guide images: designed to popup size, e.g. 400×225 px (16:9 ratio, fits medium popups), 300×300 px (square, fits small guide popups).


## IV. Special-Scenario Asset Sizes
Special interaction scenarios in desktop development (e.g. drag-and-drop, thumbnails, high-DPI adaptation) require specifically sized assets.

### 1. Thumbnails (file/image previews)
- File thumbnails: 64×64 px (list preview), 128×128 px (grid preview); keep the 1:1 or original file ratio to avoid forced stretching.
- Video/image thumbnails: scaled down from the original ratio, e.g. 160×90 px (16:9 video preview), 120×120 px (square image preview).

### 2. Drag-and-Drop Icons
- The temporary icon shown while dragging: usually 1.2x the original icon size, e.g. base icon 16×16 px → drag icon 20×20 px, 32×32 px → 38×38 px, ensuring visual clarity during the drag.

### 3. High-DPI Adaptation Notes
- When developing apps that support 4K screens (3840×2160 px), all assets must provide 3x versions (e.g. 16×16 px → 48×48 px), or use vector images (SVG format) — vector images scale infinitely without losing quality and are the optimal choice for high-DPI adaptation (suited to icons and simple illustrations; complex images still need bitmaps).


## V. Design and Export Recommendations
1. **Format selection**:
   - Icons/transparent-background elements: prefer **PNG-24** (alpha channel support, fine color detail).
   - Opaque background images/photos: use **JPG** (high compression ratio, small file size, suited to large images).
   - High-DPI adaptation/unlimited scaling: use **SVG** (vector format, suited to icons and line-art assets, avoids pixel blur).

2. **Size naming conventions**:
   - To distinguish resolution versions, add a multiplier marker to the file name, e.g. `icon_save_16x16.png` (1x), `icon_save_32x32.png` (2x), `icon_save_48x48.png` (3x), for easy lookup during development.

3. **System specification references**:
   - Windows development: refer to the icon size requirements in the [Microsoft UI design documentation](https://learn.microsoft.com/zh-cn/windows/apps/design/).
   - macOS development: refer to the app icon specification in [Apple Human Interface Guidelines](https://developer.apple.com/design/human-interface-guidelines/macos/).
   - Cross-platform development (e.g. Electron, Qt): prefer designing from "highest-resolution assets + dynamic scaling" to reduce the number of assets (e.g. provide a 512×512 px icon and let the framework scale it automatically to the required sizes).
