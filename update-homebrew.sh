#!/usr/bin/env bash
set -euo pipefail

RELEASE_VERSION="$1"
SHA256="$2"

echo "Updating Homebrew formula for $RELEASE_VERSION..."

git clone https://github.com/jmattaa/homebrew-laser.git
cd homebrew-laser

sed -i.bak "s|^  url .*|  url \"https://github.com/jmattaa/laser/archive/${RELEASE_VERSION}.tar.gz\"|" laser.rb
sed -i.bak "s|^  sha256 .*|  sha256 \"${SHA256}\"|" laser.rb
sed -i.bak "s|^  version .*|  version \"${RELEASE_VERSION}\"|" laser.rb

git config user.name "github-actions[bot]"
git config user.email "github-actions[bot]@users.noreply.github.com"
git remote set-url origin https://x-access-token:${GITHUB_TOKEN}@github.com/jmattaa/homebrew-laser.git

git add laser.rb
git commit -m "chore: update laser.rb for ${RELEASE_VERSION}"
git push origin main
