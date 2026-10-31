#!/bin/bash
set -e

echo "================================================================="
echo " Dense Desktop Environment: Ubuntu Plug & Installation Suite    "
echo "================================================================="

if [ "$EUID" -ne 0 ]; then
  echo "Error: Please run as root (sudo ./setup_ubuntu.sh)"
  exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[1/6] Installing Wayland stack (labwc, wbg, alacritty, fuzzel, audio)..."
apt-get update
apt-get install -y --no-install-recommends \
  gcc make build-essential labwc wbg alacritty fuzzel \
  pipewire wireplumber pipewire-pulse pamixer brightnessctl \
  network-manager seatd mako-notifier thunar

echo "[2/6] Executing wipe of GNOME and all other desktop environments..."
chmod +x "$SCRIPT_DIR/wipe_ubuntu_desktops.sh"
"$SCRIPT_DIR/wipe_ubuntu_desktops.sh"

echo "[3/6] Installing Dense language compiler/runtime to /usr/local/bin/dense..."
if [ -f "$SCRIPT_DIR/dense" ]; then
    cp "$SCRIPT_DIR/dense" /usr/local/bin/dense
elif [ -d "$SCRIPT_DIR/src" ]; then
    gcc -std=c11 -Wall -O2 "$SCRIPT_DIR/src/"*.c -o /usr/local/bin/dense
fi
chmod 755 /usr/local/bin/dense 2>/dev/null || true

echo "[4/6] Registering Dense Wayland session entry & utilities..."
mkdir -p /usr/share/wayland-sessions
cp "$SCRIPT_DIR/dense.desktop" /usr/share/wayland-sessions/dense.desktop
chmod 644 /usr/share/wayland-sessions/dense.desktop

cp "$SCRIPT_DIR/dense-session" /usr/local/bin/dense-session
chmod 755 /usr/local/bin/dense-session

if [ -f "$SCRIPT_DIR/dense-bar" ]; then
    cp "$SCRIPT_DIR/dense-bar" /usr/local/bin/dense-bar
    chmod 755 /usr/local/bin/dense-bar
fi

if [ -f "$SCRIPT_DIR/dense-taskmanager" ]; then
    cp "$SCRIPT_DIR/dense-taskmanager" /usr/local/bin/dense-taskmanager
    chmod 755 /usr/local/bin/dense-taskmanager
fi
if [ -f "$SCRIPT_DIR/dense-taskmanager.desktop" ]; then
    mkdir -p /usr/share/applications
    cp "$SCRIPT_DIR/dense-taskmanager.desktop" /usr/share/applications/dense-taskmanager.desktop
    chmod 644 /usr/share/applications/dense-taskmanager.desktop
fi

echo "[5/6] Deploying Dense desktop environment configurations..."
mkdir -p /etc/dense
cp "$SCRIPT_DIR/rc.xml" /etc/dense/
cp "$SCRIPT_DIR/menu.xml" /etc/dense/
cp "$SCRIPT_DIR/autostart" /etc/dense/
chmod 755 /etc/dense/autostart
cp "$SCRIPT_DIR/environment" /etc/dense/
cp "$SCRIPT_DIR/style.css" /etc/dense/
cp "$SCRIPT_DIR/"*.dense /etc/dense/ 2>/dev/null || true

ACTUAL_USER="${SUDO_USER:-$USER}"
if [ -n "$ACTUAL_USER" ] && [ "$ACTUAL_USER" != "root" ]; then
    USER_HOME=$(getent passwd "$ACTUAL_USER" | cut -d: -f6)
    USER_CONF="$USER_HOME/.config/dense"
    mkdir -p "$USER_CONF"
    cp -r /etc/dense/* "$USER_CONF/"
    chown -R "$ACTUAL_USER:$ACTUAL_USER" "$USER_CONF"
fi

echo "[6/6] Activating system services (seatd, audio, networks)..."
systemctl enable seatd 2>/dev/null || true
systemctl start seatd 2>/dev/null || true

echo "================================================================="
echo " Dense Desktop Environment has been successfully installed!     "
echo " GNOME and all other desktop environments have been wiped.       "
echo " Log in or start Wayland: select 'Dense Desktop' session!       "
echo "================================================================="
