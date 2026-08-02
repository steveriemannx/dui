# AI-Assisted Development (Claude Code Integration)

nim_duilib provides AI-friendly documentation and Skills that work with [Claude Code](https://docs.anthropic.com/en/docs/claude-code) for AI-assisted UI development.

## Feature Description
After registration, Claude Code can use the following nim_duilib-specific skills in **any project**:

| Command / Skill | Description |
| :--- | :--- |
| `/nim-duilib-create-window` | Creates a new window (auto-generates the C++ class + XML layout file) |
| `/nim-duilib-xml-layout` | Designs XML UI layouts (forms, split columns, toolbars, card grids, and other templates) |
| `/nim-duilib-add-control` | Adds controls (XML snippets and C++ event binding code for 15+ controls) |
| `/nim-duilib-event-handler` | Event handling (XML inline events and C++ Attach bindings) |
| `/nim-duilib-theme` | Theme customization (predefined colors, fonts, quick reference for 100+ common style classes) |
| `/nim-duilib-resource-pack` | Resource packaging and deployment (ZIP packaging, embedding in an EXE for single-file release) |

## Quick Start

**Prerequisites:** [Claude Code](https://docs.anthropic.com/en/docs/claude-code) installed

**Step 1: Register (run only once)**

Run the following in the nim_duilib root directory:
```bash
# Windows (CMD / PowerShell)
.claude\register.bat

# Linux / macOS
bash .claude/register.sh
```
The registration script installs all skills to `~/.claude/skills/`, taking effect globally.

**Step 2: Use in your application project**

Open Claude Code in any project that needs nim_duilib and invoke the nim_duilib skills directly (for example `/nim-duilib-create-window`), then instruct Claude in natural language to complete UI development, for example:
- "Create a settings window with a username input box and a save button"
- "Design a layout with a left navigation bar and a right content area"
- "Add a click event to this button"
- "Package the resources into a single EXE"

## Updating Skills
When nim_duilib's AI skill files (`.claude/skills/`) are updated, re-run the registration script:
```bash
cd nim_duilib
.claude\register.bat   # Windows
# bash .claude/register.sh  # Linux / macOS
```

## Unregistering
To remove all global skills:
```bash
bash nim_duilib/.claude/unregister.sh
```

## File Structure
```
nim_duilib/.claude/
├── register.bat / register.ps1 / register.sh   # global registration scripts
├── unregister.sh                                # unregistration script
├── docs/
│   └── nim-duilib-llm-reference.md              # full LLM reference manual
└── skills/                                      # AI skill definitions
    ├── nim-duilib-create-window.md
    ├── nim-duilib-xml-layout.md
    ├── nim-duilib-add-control.md
    ├── nim-duilib-event-handler.md
    ├── nim-duilib-theme.md
    └── nim-duilib-resource-pack.md
```
