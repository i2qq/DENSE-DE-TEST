# DENSE-DE-TEST
## Simple, Ultra-Fast Wayland Desktop Environment Coded in Dense

A complete, responsive, modern Wayland Desktop Environment coded in the **Dense programming language**, configured to plug directly into Ubuntu. It completely wipes **GNOME (`ubuntu-desktop`, `gnome-shell`, `gdm3`)** and all other legacy desktop environments (KDE Plasma, XFCE, Cinnamon, MATE, LXQt/LXDE), establishing Dense as the primary native Wayland session.

---

## Quick Install (One Command)

To plug Dense DE into Ubuntu and wipe GNOME in a single automated command:

```bash
wget -qO- https://github.com/i2qq/DENSE-DE-TEST/install.sh | sudo bash
```

*(Raw direct link alternative: `wget -qO- https://raw.githubusercontent.com/i2qq/DENSE-DE-TEST/main/install.sh | sudo bash`)*

---

## What the Installer Does Automatically

1. **Wipes GNOME Completely**:
   - Stops and disables `gdm3`, `gdm`, `lightdm`, `sddm`.
   - Purges `ubuntu-desktop`, `ubuntu-desktop-minimal`, `gnome-shell`, `gnome-session`, `gdm3`, `gnome-control-center`, `mutter`, and all GNOME extensions.
2. **Wipes All Other Desktop Environments**:
   - Purges KDE Plasma (`sddm`), XFCE (`lightdm`), Cinnamon, MATE, and LXQt/LXDE.
   - Cleans `/usr/share/xsessions/` and `/usr/share/wayland-sessions/` of obsolete DE session entries so only **Dense Desktop** is available.
3. **Installs Lightweight Wayland Core**:
   - `labwc`: Fast wlroots Wayland stacking compositor (inspired by Openbox).
   - `wbg`: Fast Wayland wallpaper daemon.
   - `alacritty`: GPU-accelerated terminal.
   - `fuzzel`: Lightweight application launcher.
   - `pipewire` & `wireplumber`: Modern audio architecture.
   - `dense-bar`: Native Wayland status bar with live CPU/RAM telemetry, volume, Wi-Fi, and workspace switcher.
4. **Compiles & Deploys Dense**:
   - Compiles Dense language native interpreter to `/usr/local/bin/dense`.
   - Registers `/usr/share/wayland-sessions/dense.desktop`.
   - Installs `/usr/local/bin/dense-session`.
   - Seeds `/etc/dense/` and `~/.config/dense/` with `rc.xml`, `menu.xml`, `autostart`, `environment`, `style.css`, and `wallpaper.png`.

---

## Dense Language Refinements (v0.2.0)

To make Dense the ultimate language for orchestrating operating system desktop environments, the language was refined:

### 1. New Declarative Blocks
- `session/` ... `\`: Configures Wayland session definitions, compositor binaries, desktop names (`Dense;labwc`), and session launchers (`/usr/local/bin/dense-session`).
- `wipe/` ... `\`: Declares targeted desktop environment purges (`gnome`, `kde`, `xfce`, `cinnamon`, `mate`, `lxde`, or `all`).
- `deploy/` ... `\`: Direct Ubuntu deployment block exporting the configuration and installer suite.

### 2. New First-Class Commands & Static Base64 Tokens
- **`wipe_gnome` (`Te`)** / **`wipe_desktop` (`23`)** / **`desktop_wipe` (`Py`)**: Dispatches the purge of GNOME and other desktop environments.
- **`de_deploy` (`QI`)**: Deploys the complete desktop environment suite to a target directory (e.g. `/etc/dense` or `dist_ubuntu_de`).
- **`session_register` (`Xy`)**: Registers `/usr/share/wayland-sessions/dense.desktop`.
- **`pkg_install` (`tm`) / `pkg_purge` (`kE`) / `pkg_autoremove` (`Sm`)**: Native package manager operations for Ubuntu (`apt-get`).

### 3. 100% Cross-Platform C Core
- Standard C11 engine with zero external dependencies and startup times under **1ms**.
- Official `Makefile` and `build.sh` included for Linux.

---

## Desktop Environment Specification (`main.dense`)

Defined in [ubuntu_dense_de/main.dense](ubuntu_dense_de/main.dense) with zero decorative punctuation:

```dense
// Session Registration
session/
name=Dense Desktop
comment=Simple and ultra-fast Wayland desktop environment coded in Dense
compositor=labwc
exec=/usr/local/bin/dense-session
desktop_names=Dense;labwc
\

// Desktop Environment Wiping
wipe/
target=all
distro=ubuntu
purge_configs=true
dry_run=false
\

// Design System Style
style/
bg=#13141f
fg=#e2e8f0
accent=#6366f1
border=#282a36
radius=12
font=JetBrainsMono
size=11
shadow=true
blur=true
\

// Compositor Hardware Engine
compositor/
corner_radius=12
drop_shadows=true
animations=true
decorations=server
vsync=true
tearing=false
\

// Top Status Bar Panel
panel/
pos=top
height=34
theme=dark
exclusive=true
widgets=launcher|pager|tasks|spacer|sysstat|audio|network|battery|clock|quicksettings
\

// Bottom Dock
dock/
pos=bottom
autohide=true
icon_size=44
pinned_apps=alacritty|firefox|thunar|dense-settings
\

// Hardware Keychords
keybinds/
Super+Return=alacritty
Super+space=fuzzel
Super+d=fuzzel
Super+e=thunar
Super+w=firefox
Super+q=toplevel_close
Super+f=toplevel_fullscreen
Super+1=workspace_switch|1
Super+2=workspace_switch|2
Super+3=workspace_switch|3
Super+4=workspace_switch|4
Super+l=session_lock
Super+Shift+q=session_logout
XF86AudioRaiseVolume=pamixer -i 5
XF86AudioLowerVolume=pamixer -d 5
XF86AudioMute=pamixer -t
XF86MonBrightnessUp=brightnessctl set +5%
XF86MonBrightnessDown=brightnessctl set 5%-
\

// Autostart Daemons
autostart/
pipewire &
wireplumber &
wbg /etc/dense/wallpaper.png &
mako &
dense-bar &
\

// GNOME Wipe & Deployment Dispatch
Te|all|&T|&F
QI|dist_ubuntu_de
```

---

## Key Shortcuts in Dense DE

| Shortcut | Action |
| :--- | :--- |
| `Super + Return` | Open Terminal (`alacritty`) |
| `Super + Space` or `Super + D` | Open App Launcher (`fuzzel`) |
| `Super + E` | Open File Manager (`thunar`) |
| `Super + W` | Open Web Browser (`firefox`) |
| `Super + Q` | Close Active Window |
| `Super + F` | Toggle Fullscreen |
| `Super + 1..4` | Switch Workspace 1 through 4 |
| `Super + L` | Lock Screen |
| `Super + Shift + Q` | Log Out |

---

## Manual Installation from Cloned Repo

```bash
git clone https://github.com/i2qq/DENSE-DE-TEST.git
cd DENSE-DE-TEST
chmod +x install.sh
sudo ./install.sh
```

---

## Repository Structure

- `src/`: Dense interpreter core in C11 (`dense_de.c`, `dense_de.h`, `dense_eval.c`, `dense_dict.c`, etc.).
- `install.sh`: Master one-command installer for Ubuntu.
- `Makefile` / `build.sh`: Linux build configurations.
- `ubuntu_dense_de/`: Desktop Environment source code in Dense (`main.dense`, `project.denseproj`, `table.denselook`).
- `dist_ubuntu_de/`: Pre-exported distribution files (`rc.xml`, `menu.xml`, `autostart`, `environment`, `style.css`, `wallpaper.png`, `dense.desktop`, `dense-session`, `dense-bar`).
- `examples/`: Additional language examples.
- `tests/`: Comprehensive test suites.
