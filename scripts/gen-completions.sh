#!/usr/bin/bash

LSR="lsr"
if [[ -x "build/lsr" ]]; then
  LSR="build/lsr"
fi

mkdir -p completions/bash
mkdir -p completions/zsh
mkdir -p completions/fish

$LSR --completions=fish > completions/fish/lsr.fish
$LSR --completions=zsh > completions/zsh/_lsr
$LSR --completions=bash > completions/bash/lsr

