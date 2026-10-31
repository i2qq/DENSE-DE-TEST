#!/bin/bash
set -e

echo "=========================================================="
echo " Dense OS: Complete Ubuntu GNOME & Legacy DE Purge Tool   "
echo "=========================================================="

if [ "$EUID" -ne 0 ]; then
  echo "Error: Please run as root (sudo ./wipe_ubuntu_desktops.sh)"
  exit 1
fi

echo "[1/6] Stopping and disabling active display managers..."
systemctl stop gdm3 2>/dev/null || true
systemctl stop gdm 2>/dev/null || true
systemctl stop lightdm 2>/dev/null || true
systemctl stop sddm 2>/dev/null || true
systemctl disable gdm3 2>/dev/null || true
systemctl disable gdm 2>/dev/null || true
systemctl disable lightdm 2>/dev/null || true
systemctl disable sddm 2>/dev/null || true

echo "[2/6] Purging GNOME desktop environment and Ubuntu shell..."
apt-get purge -y \
  ubuntu-desktop \
  ubuntu-desktop-minimal \
  gnome-shell \
  gnome-session \
  gnome-session-bin \
  gnome-session-common \
  gdm3 \
  gnome-control-center \
  gnome-terminal \
  mutter \
  yaru-theme-* \
  gnome-shell-extension-* 2>/dev/null || true

echo "[3/6] Purging KDE, XFCE, Cinnamon, MATE, LXQt/LXDE environments..."
apt-get purge -y \
  kubuntu-desktop kde-plasma-desktop plasma-desktop sddm \
  xubuntu-desktop xfce4 xfce4-session lightdm \
  cinnamon cinnamon-desktop-environment \
  ubuntu-mate-desktop mate-desktop-environment \
  lubuntu-desktop lxde lxqt 2>/dev/null || true

echo "[4/6] Wiping legacy desktop sessions from X11 and Wayland session trees..."
mkdir -p /usr/share/wayland-sessions /usr/share/xsessions
find /usr/share/wayland-sessions/ -type f ! -name "dense.desktop" -delete 2>/dev/null || true
rm -rf /usr/share/xsessions/* 2>/dev/null || true

echo "[5/6] Autoremoving orphaned dependencies and cleaning package cache..."
apt-get autoremove --purge -y
apt-get clean

echo "[6/6] Ensuring core Wayland & system daemons remain functional..."
apt-get install -y --no-install-recommends \
  labwc \
  wbg \
  alacritty \
  fuzzel \
  pipewire \
  wireplumber \
  pipewire-pulse \
  pamixer \
  brightnessctl \
  network-manager \
  dbus-user-session \
  seatd

echo "=========================================================="
echo " Success: GNOME and all other desktop environments wiped! "
echo " System is clean and ready for Dense Desktop Environment! "
echo "=========================================================="
