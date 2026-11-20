#!/usr/bin/env bash
# Entrypoint forwarding directly to setup.sh
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec "$SCRIPT_DIR/setup.sh" "$@"
