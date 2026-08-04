# dui AI toolchain - global register script
# Usage: cd dui && .claude\register.bat
#    or: pwsh .claude\register.ps1

$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$DuiRoot = (Resolve-Path (Join-Path $ScriptDir "..")).Path
$ClaudeHome = Join-Path $env:USERPROFILE ".claude"
$GlobalSkillsDir = Join-Path $ClaudeHome "skills"
$NimInitDir = Join-Path $GlobalSkillsDir "nim-init"

Write-Host "=== dui AI toolchain register ===" -ForegroundColor Cyan
Write-Host "dui path: $DuiRoot"
Write-Host ""

# Validate
if (-not (Test-Path (Join-Path (Join-Path $DuiRoot "dui") "dui.h"))) {
    Write-Host "Error: dui project not detected. Run this from dui root." -ForegroundColor Red
    exit 1
}

# Normalize path for use in markdown (forward slashes)
$DuiRootUnix = $DuiRoot -replace '\\', '/'

# ============================================================
# Step 1: Register all dui-* skills as global skills
# ============================================================
Write-Host "[1/2] Registering global skills..." -ForegroundColor Yellow

$SourceSkillsDir = Join-Path $ScriptDir "skills"
$SkillFiles = Get-ChildItem -Path $SourceSkillsDir -Filter "dui-*.md" -File

foreach ($file in $SkillFiles) {
    # Each skill needs its own subdirectory: ~/.claude/skills/<name>/SKILL.md
    $skillName = $file.BaseName  # e.g. "dui-create-window"
    $targetDir = Join-Path $GlobalSkillsDir $skillName
    if (-not (Test-Path $targetDir)) {
        New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
    }
    Copy-Item -Path $file.FullName -Destination (Join-Path $targetDir "SKILL.md") -Force
    Write-Host "  + $skillName"
}

# ============================================================
# Step 2: Register /nim-init command
# ============================================================
Write-Host "[2/2] Registering /nim-init command..." -ForegroundColor Yellow

if (-not (Test-Path $NimInitDir)) {
    New-Item -ItemType Directory -Path $NimInitDir -Force | Out-Null
}

$SkillContent = @"
---
name: nim-init
description: "Initialize dui AI dev toolkit for current project (copy LLM docs + update CLAUDE.md). Global skills are already available - this command sets up project-specific config."
user-invocable: true
disable-model-invocation: true
---

# dui AI Toolkit - Project Init

When the user invokes /nim-init, follow these steps:

## Config
- dui install path: ``$DuiRootUnix``
- Source docs: ``$DuiRootUnix/.claude/docs/``

## Step 1: Copy LLM reference doc
Run this bash command to copy the reference doc to the current project:
``````bash
mkdir -p .claude/docs
cp "$DuiRootUnix/.claude/docs/dui-llm-reference.md" .claude/docs/
``````

## Step 2: Update CLAUDE.md
Check if the current project's CLAUDE.md already contains "dui UI". If not, append the following block (create CLAUDE.md if it doesn't exist):

``````markdown

## dui UI

This project uses [dui](https://github.com/steveriemannx/dui) as the UI framework.
Library path: ``$DuiRootUnix``

- LLM reference: ``.claude/docs/dui-llm-reference.md``
- XML layouts: ``bin/resources/themes/default/<skin_folder>/``
- Global resources (fonts/colors/styles): ``bin/resources/themes/default/global.xml``
- dui docs: ``$DuiRootUnix/docs/``
- dui examples: ``$DuiRootUnix/examples/``

### Resource rules (IMPORTANT)
- MUST copy: ``global.xml`` + ``public/`` (shared icons) + your app's own skin directory
- NEVER copy demo directories (basic/, controls/, layout/, chat/, cef/, render/, etc.)
- NEVER copy bin/*.exe, bin/*.dll, bin/bin.zip
- Resource packaging: local files (dev) / ZIP file (release) / embedded EXE (Windows single-file)

### Key patterns
- Window class extends ``ui::WindowImplBase``, override ``GetSkinFolder()``/``GetSkinFile()``/``OnInitWindow()``
- Main thread extends ``ui::FrameworkThread``, init resources and create window in ``OnInit()``
- Find control: ``dynamic_cast<ui::Type*>(FindControl(_T("name")))``
- Bind event: ``control->AttachClick([](const ui::EventArgs&) { return true; });``
- String type: ``DString``, literals wrapped with ``_T("...")``
``````

## Step 3: Report
Tell the user initialization is complete. Note that dui skills are already globally available (no per-project copy needed):
- dui-create-window - Create new window
- dui-xml-layout - Design XML layout
- dui-add-control - Add controls
- dui-event-handler - Event handlers
- dui-theme - Theme customization
- dui-resource-pack - Resource packaging / single EXE
"@

$SkillPath = Join-Path $NimInitDir "SKILL.md"
[System.IO.File]::WriteAllText($SkillPath, $SkillContent, [System.Text.UTF8Encoding]::new($false))

# ============================================================
# Done
# ============================================================
Write-Host ""
Write-Host "=== Register complete ===" -ForegroundColor Green
Write-Host ""
Write-Host "Registered:" -ForegroundColor Yellow
Write-Host "  /nim-init               - project initialization command"
foreach ($file in $SkillFiles) {
    Write-Host "  /$($file.BaseName)  - global skill"
}
Write-Host ""
Write-Host "Skills are now globally available in ALL projects." -ForegroundColor Cyan
Write-Host "Run /nim-init in a project to set up CLAUDE.md and LLM docs."
Write-Host ""
Write-Host "To update after editing skills: just re-run this script." -ForegroundColor Gray
