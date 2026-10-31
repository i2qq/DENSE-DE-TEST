# Example 4: Dense as Full Replacement for QML, Python, and C

This example demonstrates how the **Dense language** completely eliminates the need for **QML, Python, and C** when configuring, scripting, styling, and orchestrating Wayland Desktop Environments (specifically for Labwc / wlroots).

## Core Principles
1. **Zero Decorative Punctuation**: No quotation marks, semicolons, colons, braces, or angle brackets in code logic.
2. **Zero Whitespace**: Code logic is parsed without needing spaces between operators.
3. **Static Random Base64 IDs**: All actions and vocabulary terms map to hardcoded static Base64 identifiers.
4. **Zero Python Script Dependencies**: No `.py` files on disk required.

---

## 1. QML Replacement Capabilities
Dense replaces QML declarative trees, anchors, states, animations, and delegates:
- **`style/` Block**: Full desktop theming with custom properties (`bg`, `fg`, `accent`, `border`, `radius`, `font`, `shadow`, `blur`). Exports directly to `style.css`.
- **`animation/` Block**: Declarative transitions specifying `target`, `prop`, `duration`, `easing` (`linear`, `ease_in`, `ease_out`, `ease_in_out`, `spring`), `from`, and `to`.
- **`state/` Block**: Visual state overrides (e.g. `hovered`, `focused`, `pressed`, `disabled`) with property adjustments (`scale`, `bg`, `opacity`).
- **`box/` Layouts**: Directional layouts (`row`, `col`, `grid`, `stack`) with automatic spacing and padding.
- **`widget/` Controls**: Native controls (`button`, `slider`, `toggle`, `progress`, `entry`, `icon`).
- **`repeater/` / `listview/`**: Dynamic listviews binding models to delegates (taskbars, window switchers).
- **`menu/`**: Desktop context menus with label/command mappings.

---

## 2. Python Replacement Capabilities
Dense replaces Python system utilities, daemons, background polling, and D-Bus IPC:
- **`service/` Block**: Manage systemd / init daemons (`service_start` / `EV`, `service_stop` / `ri`, `service_restart` / `bN`, `service_status` / `Vr`).
- **`timer/` Block**: Non-blocking interval timers replacing Python's `threading.Timer` and `asyncio.sleep`.
- **Hardware Sensors**: Direct queries for `cpu_temp` (`iX`), `mem_used` (`4Q`), and `disk_free` (`eK`).
- **Audio & PipeWire**: Volume adjustments (`volume` / `fK`), muting (`audio_mute`).
- **NetworkManager**: Instant WiFi telemetry (`net_status`) and connection (`net_connect`).
- **UPower & Battery**: Battery percentages (`battery_info`) and CPU governors (`power_governor`).
- **MPRIS2 Media Player**: Playback controls (`media_play`, `media_pause`, `media_next`, `media_info`).
- **Bluetooth**: Device pairing (`bluetooth_pair` / `Bc`) and status (`bluetooth_status`).
- **D-Bus IPC**: Direct session / system bus method calls and property reads (`dbus_call`, `dbus_get`).

---

## 3. C / Wayland Replacement Capabilities
Dense replaces low-level C compositor configurations, protocols, and hardware input handlers:
- **`compositor/` Block**: Direct control over compositor hardware parameters (`corner_radius`, `drop_shadows`, `animations`, `decorations`, `vsync`, `tearing`).
- **`gestures/` Block**: Touchpad multi-finger gestures (`swipe3_up`, `swipe3_down`, `pinch_in`, `pinch_out`) mapped to actions.
- **`layershell/` Block**: Direct `zwlr_layer_shell_v1` surfaces with layer positioning (`top`, `overlay`, `bottom`, `bg`), anchor bitmasks, and exclusive zones.
- **`keybinds/` Block**: Low-level hardware scancode chords (`Super+Return`, `Super+Q`, `Super+F`, etc.).
- **`windowrule/` Block**: Window management rules (decorations, tiling, floating, workspace pinning).
- **`monitor/` Block**: Display server output configurations (resolution, refresh rates, scaling).
- **Desktop Environment Exporter (`de_export` / `0j`)**: Generates production-ready Labwc configurations (`rc.xml`, `menu.xml`, `autostart`, `environment`, `style.css`, `layershell.dense`).

---

## Running the Example
From this directory:
```powershell
.\run.ps1
```
Or with `dense.exe` directly:
```bash
..\..\dense.exe main.dense
```
