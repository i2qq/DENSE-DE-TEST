# Changelog 2: Dense Task Manager Integration

## Summary
- Added native graphical Task Manager (`dense-taskmanager`) to the Dense Desktop Environment:
  - **Processes Tab**: Live process table (PID, Name, User, RSS Memory, State) with search filtering and process termination actions (`End Process` via `SIGTERM`, `Kill Force` via `SIGKILL`).
  - **Performance Tab**: Graphical progress bars for CPU Usage (%), Temperature (°C), Memory & Swap Usage (MB/%), Root Storage (GB), System Load Averages, and Uptime.
  - Automated fallback to terminal monitors (`btop` / `htop` / `top`) if GUI components are absent.
- Integrated `dense-taskmanager` into the Desktop Environment:
  - Added keychords: `Ctrl+Shift+Escape` and `Super+Escape`.
  - Added desktop right-click menu item `TaskManager -> dense-taskmanager`.
  - Pinned to bottom dock (`pinned_apps=alacritty|firefox|thunar|dense-taskmanager|dense-settings`).
  - Wired top status bar (`dense-bar`) CPU/RAM telemetry widget to launch `dense-taskmanager` upon click.
- Created `dense-taskmanager.desktop` for application launcher discovery.
- Updated `install.sh` and `setup_ubuntu.sh` to install `dense-taskmanager` to `/usr/local/bin/` and desktop file to `/usr/share/applications/`.
- Incremented changelog counter in `CHANGELOG/cnt.txt` to `2`.
