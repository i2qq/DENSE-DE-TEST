# Dense Language Desktop Environment for Ubuntu
## Complete GNOME & Legacy DE Replacement

This project provides a **simple, ultra-fast, and responsive Wayland Desktop Environment coded entirely in the Dense programming language**. It is designed to be plugged directly into Ubuntu, completely wiping **GNOME (`ubuntu-desktop`, `gnome-shell`, `gdm3`)** and all other desktop environments (KDE, XFCE, Cinnamon, MATE, LXQt/LXDE), installing Dense as the primary native Wayland session.

---

## 1. Language Refinements in Dense (v0.2.0)

To make Dense the ultimate language for orchestrating operating system desktop environments, the following language refinements were implemented:

### A. New Declarative Blocks
1. **`session/` ... `\`**:
   Declarative configuration of the Wayland desktop session, XDG session type, compositor (`labwc`), session desktop entry (`/usr/share/wayland-sessions/dense.desktop`), and session runner (`/usr/local/bin/dense-session`).
2. **`wipe/` ... `\`**:
   Declarative purge orchestrator targeting `gnome`, `kde`, `xfce`, `cinnamon`, `mate`, `lxde`, or `all`. Automatically purges display managers (`gdm3`, `sddm`, `lightdm`), wipes obsolete desktop entries from session directories, runs `autoremove --purge`, and preserves network, audio, and core system utilities.
3. **`deploy/` ... `\`**:
   One-step deployment pipeline producing all Wayland configuration files (`rc.xml`, `menu.xml`, `autostart`, `environment`, `style.css`, `dense-session`, `setup_ubuntu.sh`).

### B. New First-Class Commands & Static Base64 Tokens
- **`wipe_desktop` / `wipe_gnome` / `desktop_wipe` (`23`, `Te`, `Py`)**:
  Executes or exports the complete GNOME and legacy desktop environment purge.
- **`de_deploy` (`QI`)**:
  Deploys the desktop environment suite to a target directory (e.g. `/etc/dense` or `dist_ubuntu_de`).
- **`session_register` (`Xy`)**:
  Registers `/usr/share/wayland-sessions/dense.desktop`.
- **`pkg_install` (`tm`) / `pkg_purge` (`kE`) / `pkg_autoremove` (`Sm`)**:
  Direct package manager invocation for Debian/Ubuntu (`apt-get`).
- **`sys_reboot` / `sys_poweroff`**:
  Session power management.

### C. 100% Cross-Platform C Core
- The Dense runtime was refactored with clean cross-platform POSIX and Win32 abstraction layers (`dense_file_exists`, `dense_mkdir`, `dense_sleep_ms`, POSIX unlink/rmdir).
- Dense compiles natively on Ubuntu Linux with standard `gcc -std=c11 -Wall -O2` with **zero external dependencies** and startup times under **1ms**.
- An official `Makefile` and `build.sh` are provided.

---

## 2. Desktop Environment Architecture

The desktop environment is configured in `main.dense` using Dense's zero-whitespace, zero-decorative-punctuation syntax:

```dense
// Session Block
session/
name=Dense Desktop
comment=Simple and ultra-fast Wayland desktop environment coded in Dense
compositor=labwc
exec=/usr/local/bin/dense-session
desktop_names=Dense;labwc
\

// Wipe Block
wipe/
target=all
distro=ubuntu
purge_configs=true
dry_run=false
\

// Theme Block
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

// Compositor Block
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

// Keybindings
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
\
```

---

## 3. How to Plug into Ubuntu & Wipe GNOME

When you run `dense main.dense`, Dense exports the complete ready-to-deploy distribution folder in `dist_ubuntu_de/`:

### Automated Installation on Ubuntu:
1. Copy `dist_ubuntu_de` to your Ubuntu machine (or clone this repo):
   ```bash
   cd dist_ubuntu_de
   ```
2. Run the automated setup script with root privileges:
   ```bash
   sudo ./setup_ubuntu.sh
   ```

### What `setup_ubuntu.sh` Does Automatically:
1. **Installs Lightweight Wayland Core**: Installs `labwc` (wlroots compositor), `wbg` (wallpaper daemon), `alacritty` (GPU-accelerated terminal), `fuzzel` (app launcher), `pipewire`, `wireplumber`, `seatd`, and `mako-notifier`.
2. **Wipes GNOME Completely**: Purges `ubuntu-desktop`, `ubuntu-desktop-minimal`, `gnome-shell`, `gnome-session`, `gdm3`, `gnome-control-center`, `mutter`, and all GNOME extensions.
3. **Wipes All Other Desktop Environments**: Purges any remnants of KDE Plasma (`sddm`), XFCE (`lightdm`), Cinnamon, MATE, and LXQt.
4. **Purges Obsolete Session Trees**: Cleans `/usr/share/xsessions/` and `/usr/share/wayland-sessions/` so only `dense.desktop` is available.
5. **Installs Dense Runtime & Session**:
   - Copies `dense` to `/usr/local/bin/dense`
   - Copies `dense-session` to `/usr/local/bin/dense-session`
   - Copies `dense-bar` to `/usr/local/bin/dense-bar`
   - Registers `/usr/share/wayland-sessions/dense.desktop`
6. **Deploys Configurations**: Seeds `/etc/dense/` and `~/.config/dense/` with `rc.xml`, `menu.xml`, `autostart`, `environment`, `style.css`, and `wallpaper.png`.

When you restart or log in, select the **Dense Desktop** session. You will be greeted with an ultra-lightweight, blazing-fast, modern desktop environment!
