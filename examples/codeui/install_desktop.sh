#!/bin/bash
# Install codeui desktop launcher
BIN_DIR=$(dirname "$(readlink -f "$0")")
EXEC_PATH="$BIN_DIR/codeui"

if [ ! -f "$EXEC_PATH" ]; then
    EXEC_PATH="$(dirname "$BIN_DIR")/bin/codeui"
fi

DESKTOP_FILE="$HOME/.local/share/applications/codeui.desktop"
mkdir -p "$HOME/.local/share/applications"

cat > "$DESKTOP_FILE" << EOF
[Desktop Entry]
Type=Application
Name=Code UI Demo
Comment=Pure C++ UI - standalone, no external resources
Exec=$EXEC_PATH
Terminal=false
Categories=Development;
EOF

chmod +x "$EXEC_PATH" 2>/dev/null
update-desktop-database "$HOME/.local/share/applications/" 2>/dev/null

echo "Desktop launcher installed: $DESKTOP_FILE"
echo "You can now find 'Code UI Demo' in your application menu,"
echo "or create a desktop shortcut from it."
echo ""
echo "Note: Modern GNOME blocks double-click on raw executables."
echo "Use the .desktop file (application menu) to launch instead."
