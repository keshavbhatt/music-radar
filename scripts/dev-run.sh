#!/usr/bin/env bash
# Runs the dev build against the kf6-core24 runtime content snap — the very
# libraries the shipped music-radar snap uses (dev/prod parity). Host glibc
# stays authoritative (the content snap carries no libc).
set -euo pipefail

DIR="$(cd "$(dirname "$0")/.." && pwd)"
RT=/snap/kf6-core24/current

[ -d "$RT/usr/lib/x86_64-linux-gnu" ] || {
    echo "kf6-core24 runtime snap missing: sudo snap install kf6-core24" >&2; exit 1; }

# Some runtime-snap libs depend on Ubuntu base libraries the host may not
# have (libselinux, liblerc, ...). Farm them from the core24 base snap,
# EXCLUDING the glibc family — the host's newer glibc must stay
# authoritative or the loader chokes on GLIBC_PRIVATE symbols.
BASE=/snap/core24/current/usr/lib/x86_64-linux-gnu
FARM="$DIR/build/dev/core24-libs"
mkdir -p "$FARM"
for f in "$BASE"/*.so*; do
    b="$(basename "$f")"
    case "$b" in
        libc.so*|libm.so*|libmvec.so*|libpthread.so*|libdl.so*|librt.so*|\
        ld-linux*|libresolv.so*|libnsl.so*|libnss_*|libanl.so*|libutil.so*|\
        libBrokenLocale.so*|libthread_db.so*) continue ;;
    esac
    ln -sf "$f" "$FARM/$b"
done

export LD_LIBRARY_PATH="$RT/usr/lib/x86_64-linux-gnu:$RT/usr/lib/x86_64-linux-gnu/libproxy:$RT/usr/lib/x86_64-linux-gnu/pulseaudio:$FARM${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="$RT/usr/lib/x86_64-linux-gnu/qt6/plugins"
# Multimedia support paths. Inside the shipped snap the kde-neon-6
# extension's desktop-launch6 exports all of these; replicate them for
# host runs. Without them: pipewire aborts on "support.system handle",
# and https previews fail ("Resource cannot be discovered") because
# libsoup finds no GIO TLS module.
export SPA_PLUGIN_DIR="$RT/usr/lib/x86_64-linux-gnu/spa-0.2"
export PIPEWIRE_MODULE_DIR="$RT/usr/lib/x86_64-linux-gnu/pipewire-0.3"
export GST_PLUGIN_SYSTEM_PATH="$RT/usr/lib/x86_64-linux-gnu/gstreamer-1.0"
export GST_REGISTRY="$DIR/build/dev/gst-registry.bin"
export GIO_MODULE_DIR="$RT/usr/lib/x86_64-linux-gnu/gio/modules"
# This Qt build logs to journald when stderr is not a TTY; for dev runs we
# want plain stderr (file-capturable).
export QT_FORCE_STDERR_LOGGING=1

cd "$DIR"
exec ./build/dev/music-radar "$@"
