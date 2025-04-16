#!/usr/bin/env bash

set -euo pipefail

RELEASE_VERSION="$1"

URL="https://github.com/jmattaa/laser/archive/${RELEASE_VERSION}.tar.gz"

echo "Updating flake.nix for $RELEASE_VERSION..."

NIX_SHA=$(nix-prefetch-url --unpack "https://github.com/jmattaa/laser/archive/${RELEASE_VERSION}.tar.gz")
sed -i.bak "s|rev = \".*\";|rev = \"${RELEASE_VERSION}\";|" flake.nix
sed -i.bak "s|sha256 = \".*\";|sha256 = \"${NIX_SHA}\";|" flake.nix
sed -i.bak "s|version = \".*\";|version = \"${RELEASE_VERSION#v}\";|" flake.nix

git config user.name "github-actions[bot]"
git config user.email "github-actions[bot]@users.noreply.github.com"

git add flake.nix
git commit -m "chore: update flake.nix for ${RELEASE_VERSION}"
git push https://x-access-token:${GITHUB_TOKEN}@github.com/jmattaa/laser.git main
