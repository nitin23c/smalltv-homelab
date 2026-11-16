#!/bin/bash
# Rebuild the README images in docs/images from preview/preview.yaml.
# Needs: esphome, libsdl2-dev, xvfb, ffmpeg, imagemagick, gifsicle.
#   tools/capture_screens.sh
set -eu
cd "$(dirname "$0")/.."
esphome compile preview/preview.yaml
BIN=preview/.esphome/build/smalltv-preview/.pioenvs/smalltv-preview/program
OUT=docs/images
mkdir -p "$OUT"
Xvfb :99 -screen 0 240x240x24 >/dev/null 2>&1 &
XP=$!
trap 'kill $XP 2>/dev/null' EXIT
sleep 2
export DISPLAY=:99
grab_png() { ffmpeg -loglevel error -y -f x11grab -draw_mouse 0 -video_size 240x240 -i :99 -frames:v 1 "$1"; }
grab_gif() {
  ffmpeg -loglevel error -y -f x11grab -draw_mouse 0 -framerate 10 -video_size 240x240 -t "$2" -i :99 \
    -vf "scale=480:480:flags=neighbor,split[a][b];[a]palettegen=max_colors=128[p];[b][p]paletteuse=dither=none" "$1"
}
for scene in ok down night setup weather; do
  SCENE=$scene "$BIN" >/dev/null 2>&1 &
  P=$!
  sleep 3
  [ "$scene" != weather ] && grab_png "$OUT/$scene.png"
  case $scene in
    ok) grab_gif "$OUT/ok.gif" 4 ;;
    night) grab_gif "$OUT/night.gif" 4 ;;
    weather) grab_gif "$OUT/weather.gif" 21 ;;
  esac
  kill $P; wait $P 2>/dev/null || true
done
for f in "$OUT"/{ok,down,night,setup}.png; do convert "$f" -filter point -resize 200% -strip "$f"; done
gifsicle -O3 --batch "$OUT"/*.gif
echo "Images written to $OUT"
