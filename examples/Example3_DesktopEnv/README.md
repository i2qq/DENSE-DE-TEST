# Dense Example 3: Wayland & Labwc Desktop Environment Building Blocks

This example demonstrates how Dense serves as a specialized, ultra-fast, token-quantized ("IQ1-XXS") language for architecting and managing **Wayland & Labwc Desktop Environments and Operating Systems**.

Dense completely bans decorative punctuation (no quotes, commas, colons, brackets, or semicolons in code logic). Everything is mapped to static random Base64 IDs scaling from 1 character to 8 characters.

---

## 🧩 First-Class Desktop Environment Building Blocks

Dense provides declarative block structures tailored for AI agents and OS developers to assemble full desktop environments without token bloat:

### 1. The Taskbar Panel Block (`panel/`)
Defines a Wayland layer-shell surface with anchor, size, exclusivity, styling, and modular widget slots:
```dense
panel/
pos=&bot
height=38
theme=&dark
exclusive=&T
layer=&toplayer
widgets=launcher|pager|tasks|tray|clock|sysstat|quicksettings
\
```

- **Positions**: `&TN` (`top`), `&Iu` (`bot`), `&xR` (`left`), `&xS` (`right`)
- **Layers**: `&cj` (`toplayer`), `&1G` (`botlayer`), `&Xg` (`overlay`), `&ra` (`bglayer`)
- **Themes**: `&Os` (`dark`), `&Rx` (`light`), `&eJ` (`trans`), `&AD` (`blur`)
- **Modular Widgets**:
  - `&launcher` (`launcher`): Application start menu
  - `&KQ` (`pager`): Virtual desktop workspace switcher
  - `&s2` (`tasks`): Window list / foreign toplevel switcher
  - `&kw` (`tray`): System notification area / status indicators
  - `&SN` (`clock`): Date / time clock
  - `sysstat`: System metrics (CPU, RAM, Battery)
  - `&mk` (`quicksettings`): Control center sliders & power button

### 2. The Floating App Dock Block (`dock/`)
Creates an animated, floating application dock with autohide and pinned applications:
```dense
dock/
pos=&bot
size=48
autohide=&T
pinned=alacritty|firefox|thunar|vscode
\
```

### 3. The Wallpaper Manager Block (`wallpaper/`)
Controls Wayland background layers across displays:
```dense
wallpaper/
path=assets\wallpaper_neon.png
mode=&fill
\
```
- Modes: `&xa` (`fill`), `&ca` (`fit`), `&Qf` (`tile`), `&z7` (`stretch`), `&v6` (`span`).

### 4. The Labwc Compositor Block (`labwc/`)
Configures the Labwc wlroots-based Wayland window manager:
```dense
labwc/
theme=Openbox
workspaces=4
snap=&T
keybind=W-Return|alacritty
keybind=W-q|close
keybind=W-d|launcher
keybind=W-f|toggle_fullscreen
keybind=W-m|toggle_maximize
\
```

---

## ⚡ Runtime Desktop Multiplexer Commands

In addition to declarative blocks, all DE components can be dynamically queried, manipulated, or scripted using Dense's universal multiplexer (`|`) and static Base64 action IDs:

| Action ID | Command Name | Description | Example |
| :--- | :--- | :--- | :--- |
| `ti` | `toplevel_list` | List active Wayland windows | `win_count=ti` |
| `Va` | `toplevel_focus` | Focus target window | `Va\|firefox` |
| `gf` | `toplevel_close` | Close target window | `gf\|active` |
| `hg` | `toplevel_maximize` | Toggle window maximize | `hg\|active` |
| `cC` | `toplevel_minimize` | Minimize target window | `cC\|active` |
| `i7` | `toplevel_fullscreen`| Toggle fullscreen mode | `i7\|active` |
| `JF` | `workspace_switch` | Switch to workspace index | `JF\|2` |
| `V7` | `workspace_list` | Query workspace count | `ws=V7` |
| `If` | `notify_send` | Send desktop notification | `If\|Title\|Body\|&norm_urg` |
| `d9` | `osd_show` | Display on-screen display | `d9\|volume\|85` |
| `PQ` | `labwc_reload` | Signal Labwc to reload | `PQ` |
| `PL` | `session_lock` | Lock Wayland session | `PL` |
| `3r` | `session_logout` | Log out session | `3r` |
| `7D` | `session_reboot` | Reboot machine | `7D` |
| `sk` | `session_poweroff` | Power off machine | `sk` |
| `0j` | `de_export` | Export full Labwc config directory | `0j\|dist_labwc_de` |
| `Xs` | `de_status` | Query active DE summary | `status=Xs` |

---

## 🚀 Running the Example

### Option 1: PowerShell
```powershell
cd "examples\Example3_DesktopEnv"
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

### Option 2: Command Line
```cmd
cd "examples\Example3_DesktopEnv"
run.bat
```

### Option 3: Direct Interpreter Execution
```powershell
..\..\dense.exe main.dense
```
