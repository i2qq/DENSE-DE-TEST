#!/bin/bash
# =========================================================================
# Dense Desktop Environment (DENSE-DE-TEST) - One-Command Ubuntu Installer
# Wipes GNOME & all other desktop environments and installs pure Dense DE
# =========================================================================
set -e

echo "================================================================="
echo " Dense Desktop Environment: Ubuntu Plug & Installation Suite    "
echo " https://github.com/i2qq/DENSE-DE-TEST                          "
echo "================================================================="

if [ "$EUID" -ne 0 ]; then
  echo "Error: Please run as root (e.g. sudo bash or run via sudo)"
  exit 1
fi

export DEBIAN_FRONTEND=noninteractive

# Detect repository directory or clone if piped via wget/curl
if [ -d "./src" ] && [ -f "./Makefile" ]; then
    WORKDIR="$(pwd)"
elif [ -d "/tmp/dense-de-install/src" ]; then
    WORKDIR="/tmp/dense-de-install"
else
    WORKDIR="/tmp/dense-de-install"
    echo "[*] Downloading Dense Desktop Environment repository..."
    rm -rf "$WORKDIR"
    apt-get update -y
    apt-get install -y --no-install-recommends git ca-certificates curl software-properties-common
    add-apt-repository -y universe 2>/dev/null || true
    git clone --depth 1 https://github.com/i2qq/DENSE-DE-TEST.git "$WORKDIR"
fi

echo "[1/6] Installing minimal Wayland core runtime packages..."
add-apt-repository -y universe 2>/dev/null || true
apt-get update -y
apt-get install -y --no-install-recommends \
  gcc \
  make \
  build-essential \
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
  seatd \
  mako-notifier \
  thunar \
  python3 \
  python3-pip \
  libglib2.0-0

echo "[2/6] Wiping GNOME and all other desktop environments..."
# Stop and disable active display managers
systemctl stop gdm3 2>/dev/null || true
systemctl stop gdm 2>/dev/null || true
systemctl stop lightdm 2>/dev/null || true
systemctl stop sddm 2>/dev/null || true
systemctl disable gdm3 2>/dev/null || true
systemctl disable gdm 2>/dev/null || true
systemctl disable lightdm 2>/dev/null || true
systemctl disable sddm 2>/dev/null || true

# Purge GNOME packages
echo "  -> Purging GNOME Shell, GDM3, and Ubuntu Desktop..."
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

# Purge KDE, XFCE, Cinnamon, MATE, LXQt/LXDE
echo "  -> Purging KDE, XFCE, Cinnamon, MATE, LXDE/LXQt packages..."
apt-get purge -y \
  kubuntu-desktop kde-plasma-desktop plasma-desktop sddm \
  xubuntu-desktop xfce4 xfce4-session lightdm \
  cinnamon cinnamon-desktop-environment \
  ubuntu-mate-desktop mate-desktop-environment \
  lubuntu-desktop lxde lxqt 2>/dev/null || true

# Wipe legacy desktop session entries from X11 and Wayland
echo "  -> Wiping obsolete desktop session files..."
mkdir -p /usr/share/wayland-sessions /usr/share/xsessions
find /usr/share/wayland-sessions/ -type f ! -name "dense.desktop" -delete 2>/dev/null || true
rm -rf /usr/share/xsessions/* 2>/dev/null || true

# Autoremove orphaned packages and clean cache
echo "  -> Autoremoving orphaned dependencies..."
apt-get autoremove --purge -y
apt-get clean

echo "[3/6] Compiling and installing Dense language interpreter to /usr/local/bin/dense..."
if [ -d "$WORKDIR/src" ]; then
    gcc -std=c11 -Wall -O2 "$WORKDIR/src/"*.c -o /usr/local/bin/dense
    chmod 755 /usr/local/bin/dense
fi

echo "[4/6] Registering Dense Wayland session entry..."
mkdir -p /usr/share/wayland-sessions
if [ -f "$WORKDIR/dist_ubuntu_de/dense.desktop" ]; then
    cp "$WORKDIR/dist_ubuntu_de/dense.desktop" /usr/share/wayland-sessions/dense.desktop
else
    cat << 'EOF' > /usr/share/wayland-sessions/dense.desktop
[Desktop Entry]
Name=Dense Desktop
Comment=Simple and ultra-fast Wayland desktop environment coded in Dense
Exec=/usr/local/bin/dense-session
TryExec=/usr/local/bin/dense-session
Type=Application
DesktopNames=Dense;labwc
EOF
fi
chmod 644 /usr/share/wayland-sessions/dense.desktop

# Install session runner
if [ -f "$WORKDIR/dist_ubuntu_de/dense-session" ]; then
    cp "$WORKDIR/dist_ubuntu_de/dense-session" /usr/local/bin/dense-session
else
    cat << 'EOF' > /usr/local/bin/dense-session
#!/bin/bash
export XDG_CURRENT_DESKTOP="Dense:labwc"
export XDG_SESSION_DESKTOP="Dense"
export XDG_SESSION_TYPE="wayland"
export MOZ_ENABLE_WAYLAND=1
export GDK_BACKEND="wayland,x11"
export QT_QPA_PLATFORM="wayland;xcb"
export SDL_VIDEODRIVER="wayland"
export CLUTTER_BACKEND="wayland"

if [ -z "$XDG_RUNTIME_DIR" ]; then
    export XDG_RUNTIME_DIR="/run/user/$(id -u)"
fi

DENSE_CONF_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/dense"
mkdir -p "$DENSE_CONF_DIR"

if [ ! -f "$DENSE_CONF_DIR/rc.xml" ] && [ -d "/etc/dense" ]; then
    cp -r /etc/dense/* "$DENSE_CONF_DIR/" 2>/dev/null || true
fi

exec labwc -C "$DENSE_CONF_DIR"
EOF
fi
chmod 755 /usr/local/bin/dense-session

# Install native status bar
if [ -f "$WORKDIR/dist_ubuntu_de/dense-bar" ]; then
    cp "$WORKDIR/dist_ubuntu_de/dense-bar" /usr/local/bin/dense-bar
    chmod 755 /usr/local/bin/dense-bar
fi

# Install Dense Task Manager
if [ -f "$WORKDIR/dist_ubuntu_de/dense-taskmanager" ]; then
    cp "$WORKDIR/dist_ubuntu_de/dense-taskmanager" /usr/local/bin/dense-taskmanager
    chmod 755 /usr/local/bin/dense-taskmanager
fi
if [ -f "$WORKDIR/dist_ubuntu_de/dense-taskmanager.desktop" ]; then
    mkdir -p /usr/share/applications
    cp "$WORKDIR/dist_ubuntu_de/dense-taskmanager.desktop" /usr/share/applications/dense-taskmanager.desktop
    chmod 644 /usr/share/applications/dense-taskmanager.desktop
fi

echo "[5/6] Deploying Dense desktop environment configurations..."
mkdir -p /etc/dense
if [ -d "$WORKDIR/dist_ubuntu_de" ]; then
    cp -r "$WORKDIR/dist_ubuntu_de/"* /etc/dense/
fi
chmod 755 /etc/dense/autostart 2>/dev/null || true

# Seed target user's home configuration directory
ACTUAL_USER="${SUDO_USER:-$USER}"
if [ -n "$ACTUAL_USER" ] && [ "$ACTUAL_USER" != "root" ]; then
    USER_HOME=$(getent passwd "$ACTUAL_USER" | cut -d: -f6)
    USER_CONF="$USER_HOME/.config/dense"
    mkdir -p "$USER_CONF"
    cp -r /etc/dense/* "$USER_CONF/"
    chown -R "$ACTUAL_USER:$ACTUAL_USER" "$USER_CONF"
fi

echo "[6/6] Activating system services (seatd, network, audio)..."
systemctl enable seatd 2>/dev/null || true
systemctl start seatd 2>/dev/null || true

echo "================================================================="
echo " Dense Desktop Environment has been successfully installed!     "
echo " GNOME and all other desktop environments have been wiped.       "
echo " Log in or start Wayland: select 'Dense Desktop' session!       "
echo "================================================================="
