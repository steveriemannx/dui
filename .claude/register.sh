#!/bin/bash
# dui AI toolchain - global register script
# Usage: cd dui && bash .claude/register.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DUI_ROOT="$(dirname "$SCRIPT_DIR")"
CLAUDE_HOME="${HOME}/.claude"
GLOBAL_SKILLS_DIR="${CLAUDE_HOME}/skills"

echo "=== dui AI toolchain register ==="
echo "dui path: ${DUI_ROOT}"
echo ""

# Validate
if [ ! -f "${DUI_ROOT}/include/dui/dui.h" ]; then
    echo "Error: dui project not detected. Run this from dui root."
    exit 1
fi

# ============================================================
# Step 1: Register all dui-* skills as global skills
# ============================================================
echo "[1/2] Registering global skills..."

for skill_file in "${SCRIPT_DIR}/skills"/dui-*.md; do
    [ -f "$skill_file" ] || continue
    skill_name="$(basename "$skill_file" .md)"
    target_dir="${GLOBAL_SKILLS_DIR}/${skill_name}"
    mkdir -p "$target_dir"
    cp "$skill_file" "${target_dir}/SKILL.md"
    echo "  + ${skill_name}"
done

# ============================================================
# Step 2: Register /nim-init command
# ============================================================
echo "[2/2] Registering /nim-init command..."

NIM_INIT_DIR="${GLOBAL_SKILLS_DIR}/nim-init"
mkdir -p "$NIM_INIT_DIR"

cat > "${NIM_INIT_DIR}/SKILL.md" << SKILLEOF
---
name: nim-init
description: "Initialize dui AI dev toolkit for current project (copy LLM docs + update CLAUDE.md). Global skills are already available - this command sets up project-specific config."
user-invocable: true
disable-model-invocation: true
---

# dui AI Toolkit - Project Init

When the user invokes /nim-init, follow these steps:

## Config
- dui install path: \`${DUI_ROOT}\`
- Source docs: \`${DUI_ROOT}/.claude/docs/\`

## Step 1: Copy LLM reference doc
Run this bash command to copy the reference doc to the current project:
\`\`\`bash
mkdir -p .claude/docs
cp "${DUI_ROOT}/.claude/docs/dui-llm-reference.md" .claude/docs/
\`\`\`

## Step 2: Update CLAUDE.md
Check if the current project's CLAUDE.md already contains "dui UI". If not, append the following block (create CLAUDE.md if it doesn't exist):

\`\`\`markdown

## dui UI

This project uses [dui](https://github.com/steveriemannx/dui) as the UI framework.
Library path: \`${DUI_ROOT}\`

- LLM reference: \`.claude/docs/dui-llm-reference.md\`
- XML layouts: \`bin/resources/themes/default/<skin_folder>/\`
- Global resources (fonts/colors/styles): \`bin/resources/themes/default/global.xml\`
- dui docs: \`${DUI_ROOT}/docs/\`
- dui examples: \`${DUI_ROOT}/examples/\`

### Resource rules (IMPORTANT)
- MUST copy: \`global.xml\` + \`public/\` (shared icons) + your app's own skin directory
- NEVER copy demo directories (basic/, controls/, layout/, chat/, cef/, render/, etc.)
- NEVER copy bin/*.exe, bin/*.dll, bin/bin.zip
- Resource packaging: local files (dev) / ZIP file (release) / embedded EXE (Windows single-file)

### Key patterns
- Window class extends \`ui::WindowImplBase\`, override \`GetSkinFolder()\`/\`GetSkinFile()\`/\`OnInitWindow()\`
- Main thread extends \`ui::FrameworkThread\`, init resources and create window in \`OnInit()\`
- Find control: \`dynamic_cast<ui::Type*>(FindControl(_T("name")))\`
- Bind event: \`control->AttachClick([](const ui::EventArgs&) { return true; });\`
- String type: \`DString\`, literals wrapped with \`_T("...")\`
\`\`\`

## Step 3: Report
Tell the user initialization is complete. Note that dui skills are already globally available (no per-project copy needed):
- dui-create-window - Create new window
- dui-xml-layout - Design XML layout
- dui-add-control - Add controls
- dui-event-handler - Event handlers
- dui-theme - Theme customization
- dui-resource-pack - Resource packaging / single EXE
SKILLEOF

# ============================================================
# Done
# ============================================================
echo ""
echo "=== Register complete ==="
echo ""
echo "Registered:"
echo "  /nim-init               - project initialization command"
for skill_file in "${SCRIPT_DIR}/skills"/dui-*.md; do
    [ -f "$skill_file" ] || continue
    echo "  /$(basename "$skill_file" .md)  - global skill"
done
echo ""
echo "Skills are now globally available in ALL projects."
echo "Run /nim-init in a project to set up CLAUDE.md and LLM docs."
echo ""
echo "To update after editing skills: just re-run this script."
