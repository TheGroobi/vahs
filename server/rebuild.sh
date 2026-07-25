#!/usr/bin/env bash
set -euo pipefail

FLAKE="github:TheGroobi/vahs?dir=server/nix#t630"

echo "Rebuilding from $FLAKE"

sudo nixos-rebuild switch --flake "$FLAKE"

echo "Rebuild complete"
