#!/usr/bin/env fish

# VisionCore SDK launcher for Arch Linux + fish
# - creates/uses local venv
# - installs dependencies
# - runs the app

set -l ROOT (dirname (status -f))
set -l PROJ (realpath "$ROOT/..")

cd "$PROJ"

if not command -vq python
  echo "python not found. Install: sudo pacman -S python"
  exit 1
end

if test ! -d .venv
  echo "Creating venv…"
  python -m venv .venv
end

source .venv/bin/activate.fish

# Keep installs reproducible but simple
python -m pip install -U pip setuptools wheel
python -m pip install -r requirements.txt
python -m pip install -e .

# Helpful on minimal installs (avoid Qt warnings/crashes)
set -x QT_LOGGING_RULES "*.debug=false"

# Run
visioncore
