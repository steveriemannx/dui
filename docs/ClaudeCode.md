# AI-Assisted Development (Claude Code Integration)

dui provides AI-friendly documentation and Skills that work with [Claude Code](https://docs.anthropic.com/en/docs/claude-code) for AI-assisted UI development.

## Feature Description
After registration, Claude Code can use the following dui-specific skills in **any project**:

| Command / Skill | Description |
| :--- | :--- |
| `/dui-create-window` | Creates a new window (auto-generates the C++ class + XML layout file) |
| `/dui-xml-layout` | Designs XML UI layouts (forms, split columns, toolbars, card grids, and other templates) |
| `/dui-add-control` | Adds controls (XML snippets and C++ event binding code for 15+ controls) |
| `/dui-event-handler` | Event handling (XML inline events and C++ Attach bindings) |
| `/dui-theme` | Theme customization (predefined colors, fonts, quick reference for 100+ common style classes) |
| `/dui-resource-pack` | Resource packaging and deployment (ZIP packaging, embedding in an EXE for single-file release) |

## Quick Start

**Prerequisites:** [Claude Code](https://docs.anthropic.com/en/docs/claude-code) installed

**Step 1: Register (run only once)**

Run the following in the dui root directory:
```bash
# Windows (CMD / PowerShell)
.claude\register.bat

# Linux / macOS
bash .claude/register.sh
```
The registration script installs all skills to `~/.claude/skills/`, taking effect globally.

**Step 2: Use in your application project**

Open Claude Code in any project that needs dui and invoke the dui skills directly (for example `/dui-create-window`), then instruct Claude in natural language to complete UI development, for example:
- "Create a settings window with a username input box and a save button"
- "Design a layout with a left navigation bar and a right content area"
- "Add a click event to this button"
- "Package the resources into a single EXE"

## Updating Skills
When dui's AI skill files (`.claude/skills/`) are updated, re-run the registration script:
```bash
cd dui
.claude\register.bat   # Windows
# bash .claude/register.sh  # Linux / macOS
```

## Unregistering
To remove all global skills:
```bash
bash dui/.claude/unregister.sh
```

## File Structure
```
dui/.claude/
├── register.bat / register.ps1 / register.sh   # global registration scripts
├── unregister.sh                                # unregistration script
├── docs/
│   └── dui-llm-reference.md              # full LLM reference manual
└── skills/                                      # AI skill definitions
    ├── dui-create-window.md
    ├── dui-xml-layout.md
    ├── dui-add-control.md
    ├── dui-event-handler.md
    ├── dui-theme.md
    └── dui-resource-pack.md
```
