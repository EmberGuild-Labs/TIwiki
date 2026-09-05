#!/bin/sh
# Build the desktop simulator for one wiki.
#
#   ./build.sh mathwiki           -> ./sim-mathwiki
#   ./build.sh physicswiki --asan -> ./sim-physicswiki-asan
#
# Compiles the real engine/main.c against the stub graphx/keypadc headers in
# this directory, linked with the chosen wiki's generated content.c.
set -e

HERE=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$HERE/../../.." && pwd)

PROJECT=${1:?usage: build.sh <project> [--asan]}
SRC="$REPO/$PROJECT/src"
[ -d "$SRC" ] || { echo "no such wiki: $REPO/$PROJECT/src" >&2; exit 1; }

SAN=""
OUT="$HERE/sim-$PROJECT"
if [ "$2" = "--asan" ]; then
    SAN="-fsanitize=address,undefined"
    OUT="$OUT-asan"
fi

OBJ=$(mktemp -d)
trap 'rm -rf "$OBJ"' EXIT

# $SAN is one word or empty, so it is safe unquoted; paths may contain spaces.
CC="cc -std=c11 -Wall -Wextra -g"

$CC $SAN -I"$HERE" -I"$SRC" -Dmain=wiki_main -c "$REPO/engine/main.c" -o "$OBJ/main.o"
$CC $SAN -I"$HERE" -I"$SRC"                  -c "$SRC/content.c"      -o "$OBJ/content.o"
$CC $SAN -I"$HERE" -I"$SRC"                  -c "$HERE/sim.c"         -o "$OBJ/sim.o"
cc $SAN "$OBJ/main.o" "$OBJ/content.o" "$OBJ/sim.o" -o "$OUT"

echo "built $OUT"
