## Logging

The library logs through `ui::LogUtil` ([include/dui/Utils/LogUtil.h](../include/dui/Utils/LogUtil.h)).
It exists to make failures visible: a resource that is missing, an XML node name that
is not recognized, a font or image that cannot be loaded. Before this, an XML file that
did not load produced an empty window and no message anywhere.

### Where the messages go

| Platform | Sink |
| :--- | :--- |
| Windows | `OutputDebugStringA` (the debugger's output window) |
| macOS | `stderr`, and the unified log (subsystem `com.dui.framework`, category `dui`) |
| Linux / FreeBSD (X11 and Wayland) | `stderr` |

On macOS the unified log matters for applications launched from Finder, which have no
terminal:

```sh
log stream --predicate 'subsystem == "com.dui.framework"'
```

### Levels

`LogLevel` has five levels, and the default is `kWarn`:

| Level | Meaning |
| :--- | :--- |
| `kTrace` | Per-item detail: every resource loaded, every decode started |
| `kDebug` | Diagnostics for one subsystem |
| `kInfo` | Lifecycle: startup, theme switched, window created |
| `kWarn` | Something was skipped or fell back; the UI still works |
| `kError` | An operation failed and its result is missing |

Warn is the default because a UI library that prints on every resource load is worse
than one that prints nothing: it buries the two lines that matter.

### Turning it up

Set `DUI_LOG_LEVEL` before starting the program; it is read once, on the first log
call, and accepts `trace`, `debug`, `info`, `warn`, `error`, `off` (case-insensitive) or
the matching number `0`..`5`:

```sh
DUI_LOG_LEVEL=debug ./MyApp
```

An application can also set it in code, which overrides the environment variable:

```cpp
ui::LogUtil::SetLevel(ui::LogLevel::kWarn);
```

A mistyped `DUI_LOG_LEVEL` is reported once, as a warning, rather than ignored.

### Writing a log call

```cpp
DUI_LOG_WARN(StringUtil::Printf("image \"%s\" is not in the embedded resources", name.c_str()));
DUI_LOG_ERROR(StringUtil::Printf("cannot load xml file \"%s\": %s", path.c_str(), result.description()));
```

`DUI_LOG_TRACE`, `DUI_LOG_DEBUG`, `DUI_LOG_INFO`, `DUI_LOG_WARN`, `DUI_LOG_ERROR` and
the generic `DUI_LOG(level, message)` are the entry points. The message expression is
evaluated only when the level passes, so a disabled call costs one atomic load and no
allocation. A message should name **what** failed and **where** — the file, the node
name, the font or image name. `"failed"` on its own is not worth the line.

`Write()` runs on any thread: the level is atomic and each message reaches the sink in
a single locked call, so lines from the worker and image-decode threads never
interleave.

### Build-time floor

`-DDUI_LOG_COMPILE_LEVEL=2` removes every call site below INFO from the object code
(see `LogUtil::IsLevelCompiled`). The default is `0`, which keeps all levels compiled
in and leaves `DUI_LOG_LEVEL` in control at runtime.
