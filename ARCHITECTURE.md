# Dense Language Interpreter Architecture & Plan

## 1. Overview
Dense is a token-quantized ("IQ1-XXS"), zero-whitespace, zero-punctuation interpreted language.
The interpreter executable `dense.exe` is written in standard C99/C11 for Windows native execution with ultra-fast startup (<1ms) and minimal dependencies (MSVCRT / Windows standard libraries).

## 2. File Ecosystem & Rules
1. `project.denseproj`:
   - Line 1: `//denseproj`
   - Line 2: `encoding=base8` (or similar configs)
   - `scratchpad / ... \` block: ignored/skipped entirely by the interpreter.
   - `global | imports / ... \` block: imports with `-` mapping, e.g. `pillow - PID`.
2. `table.denselook`:
   - Must be present on disk in project directory.
   - `table / style | normal \` activates built-in master dictionary + additive project mappings.
   - `table / style | custom \` activates strict custom mappings only.
   - Mappings format: `<CODE> | <ACTION/NAME>` or `&<CODE> | <WORD/CONSTANT>`
3. `*.dense` source files:
   - Evaluated line by line / statement by statement.
   - Comments start with `//`.
   - Operators: `=` (assignment), `|` (multiplexer / pipe operator / call / branch / cascade).
   - Comparison operators inside conditions: `==`, `!=`, `>=`, `<=`, `>`, `<`.
   - Math / expressions if needed: `+`, `-`, `*`, `/`.
   - Prefix `&` for vocabulary / states / types / constants.
   - Direct execution of built-in/custom commands: `K` (echo/print), `J` (read_file), `9` (write_file), `Q` (mkdir), `7` (del), `v` (range loop), `s` (cls), `8` (sleep), `X` (exit), etc.
   - Multiplexers:
     - Branching / Ternary: `cond|true_expr|false_expr` or `cond|cmd1|arg1|cmd2|arg2`
     - Assignment with multiplexer: `val=cond|true_val|false_val`
     - Command call: `CMD|arg1|arg2...`
     - Assignment from command: `var=CMD|arg1|arg2...`
     - Loops: `v|var|count|action...`

## 3. Directory Layout
- `src/`
  - `dense.h`: Common definitions, AST, environment, symbol table structures.
  - `dense_dict.h` / `dense_dict.c`: Master Base64 hardcoded dictionary table + loader.
  - `dense_proj.h` / `dense_proj.c`: `.denseproj` parser (scratchpad skipper, imports loader).
  - `dense_table.h` / `dense_table.c`: `table.denselook` parser (style normal vs custom).
  - `dense_eval.h` / `dense_eval.c`: Tokenizer, expression evaluator, multiplexer engine, variables/state.
  - `dense_py.h` / `dense_py.c`: C-embedded runtime integration for direct library execution.
  - `dense_cb.h` / `dense_cb.c`: First-class C callback trampolines.
  - `dense_de.h` / `dense_de.c`: Wayland & Labwc Desktop Environment Subsystem & Building Blocks.
  - `dense_main.c`: CLI entrypoint, argument parsing, execution runner.

## 4. Wayland & Labwc Desktop Environment Building Blocks Subsystem
Dense provides first-class native building blocks for constructing, styling, scripting, and deploying complete Wayland and Labwc desktop environments:

1. **Declarative Block Structures**:
   - `panel/` ... `\`: Wayland layer-shell taskbar panel surfaces with anchors, exclusivity, and modular widgets (`launcher`, `pager`, `tasks`, `tray`, `clock`, `sysstat`, `quicksettings`).
   - `dock/` ... `\`: Floating application docks with autohide and pinned applications.
   - `wallpaper/` ... `\`: Wallpaper manager supporting `&fill`, `&fit`, `&tile`, `&stretch`, `&span`.
   - `labwc/` ... `\`: Labwc wlroots compositor configuration (themes, workspaces, snapping, keybinding chords, window rules).
   - `controlcenter/` ... `\`: QuickSettings control center surface with audio, Wi-Fi, and battery.
   - `lockscreen/` ... `\`: Lock screen session controller via `ext-session-lock-v1`.

2. **QML Replacement Blocks**:
   - `style/` or `theme/` ... `\`: Desktop theme variables (`bg`, `fg`, `accent`, `border`, `radius`, `font`, `size`, `shadow`, `blur`). Exports directly to `style.css`.
   - `animation/` ... `\`: Declarative animations and transitions with `target`, `prop`, `duration`, `easing`, `from`, `to`.
   - `state/` ... `\`: Dynamic component states (`hovered`, `focused`, `pressed`, `disabled`) with property overrides.
   - `box/` ... `\`: Layout containers (`row`, `col`, `grid`, `stack`) with automatic spacing, padding, and child hierarchy.
   - `widget/` ... `\`: Native controls (`button`, `slider`, `toggle`, `progress`, `entry`, `icon`).
   - `repeater/` or `listview/` ... `\`: Dynamic item lists with model-to-delegate binding.
   - `menu/` ... `\`: Desktop context menus with item actions.

3. **Python Replacement Blocks & Controls**:
   - `service/` or `daemon/` ... `\`: System service lifecycle (`service_start` / `EV`, `service_stop` / `ri`, `service_restart` / `bN`, `service_status` / `Vr`).
   - `timer/` ... `\`: Interval and event timers replacing Python's `threading.Timer` / `asyncio`.
   - Hardware Sensors: Native queries for `cpu_temp` (`iX`), `mem_used` (`4Q`), and `disk_free` (`eK`).
   - Audio & PipeWire: Volume and mute control (`volume` / `fK`, `audio_mute`).
   - NetworkManager: Wi-Fi status and connect (`net_status`, `net_connect`).
   - Battery & Power: Telemetry and CPU governor (`battery_info`, `power_governor`).
   - MPRIS2 Media: Playback controls (`media_play`, `media_pause`, `media_next`, `media_prev`, `media_info`).
   - Bluetooth: Device pairing and status (`bluetooth_pair` / `Bc`, `bluetooth_unpair` / `6g`, `bluetooth_status`).
   - D-Bus IPC: Session and system bus invocations (`dbus_call`, `dbus_get`).

4. **C / Compositor Replacement Blocks & Controls**:
   - `compositor/` ... `\`: Labwc / wlroots hardware parameters (`corner_radius`, `drop_shadows`, `animations`, `decorations`, `vsync`, `tearing`).
   - `gestures/` ... `\`: Touchpad multi-finger gesture bindings (`swipe3_up`, `swipe3_down`, `pinch_in`, `pinch_out`).
   - `layershell/` ... `\`: Native `zwlr_layer_shell_v1` surfaces with namespaces, layer positions, anchor bitmasks, and exclusive zones.
   - `keybinds/` ... `\`: Hardware scancode keychords (`Super+Return`, `Super+Q`, `Super+F`, etc.).
   - `windowrule/` ... `\`: Application window rules (decorations, workspaces, floating, dimensions).
   - `monitor/` ... `\`: Multi-monitor display management (resolution, refresh rate, scaling).
   - `de_export` / `0j`: Full export of `rc.xml`, `menu.xml`, `autostart`, `environment`, `style.css`, and `layershell.dense`.

5. **Ubuntu Integration, GNOME Wiping & Deployment Subsystem**:
   - `session/` ... `\`: Wayland session registration (`name`, `comment`, `compositor`, `exec`, `desktop_names`). Generates `/usr/share/wayland-sessions/dense.desktop` and `/usr/local/bin/dense-session`.
   - `wipe/` ... `\`: Complete desktop environment purging engine. Targets `all`, `gnome`, `kde`, `xfce`, `cinnamon`, `mate`, `lxde`. Stops and disables display managers (`gdm3`, `sddm`, `lightdm`), wipes obsolete session files from `/usr/share/xsessions/` and `/usr/share/wayland-sessions/`, purges GNOME packages (`ubuntu-desktop`, `gnome-shell`, `gdm3`, etc.), and autoremoves orphaned packages while preserving essential network, audio, and init services.
   - `deploy/` ... `\`: End-to-end Ubuntu deployment producing `setup_ubuntu.sh`, `wipe_ubuntu_desktops.sh`, `dense-session`, `dense-bar`, and configurations.
   - Direct built-in commands:
     - `wipe_desktop` / `wipe_gnome` / `desktop_wipe` (`23`, `Te`, `Py`): Dispatches desktop purging.
     - `de_deploy` (`QI`): Exports full deployment suite.
     - `session_register` (`Xy`): Registers session desktop entry.
     - `pkg_install` (`tm`) / `pkg_purge` (`kE`) / `pkg_autoremove` (`Sm`): Package manager operations.

- `tests/`
  - Test suites verifying:
    - Normal & custom style lookup tables
    - Scratchpad skipping and import parsing
    - Zero punctuation syntax (variables, assignment, pipe branching, file I/O, loops, system calls)
    - Wayland desktop environment blocks, GNOME wiping, and deployment
- `Makefile` / `build.sh` (POSIX Linux)
- `dense.exe` (compiled target)
