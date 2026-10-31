# Changelog 3: Fix Raw Download URL & Noninteractive Ubuntu Installation

## Summary
- Diagnosed issue where `wget -qO- https://github.com/i2qq/DENSE-DE-TEST/install.sh` returned HTTP 404 because GitHub web URLs require `/raw/main/` or `raw.githubusercontent.com`.
- Updated documentation with the working direct raw GitHub URL:
  `wget -qO- https://raw.githubusercontent.com/i2qq/DENSE-DE-TEST/main/install.sh | sudo bash`
  and curl equivalent:
  `curl -sSL https://raw.githubusercontent.com/i2qq/DENSE-DE-TEST/main/install.sh | sudo bash`
- Hardened `install.sh` and `setup_ubuntu.sh`:
  - Added `export DEBIAN_FRONTEND=noninteractive` to prevent any interactive prompts during package purging or installation.
  - Added automatic enabling of Ubuntu `universe` repository (`add-apt-repository -y universe`) for required Wayland dependencies.
- Re-exported all configuration artifacts in `dist_ubuntu_de/`.
- Incremented changelog counter in `CHANGELOG/cnt.txt` to `3`.
