// Weather from Open-Meteo (no API key) and a small animated weather icon.
// The icon is drawn into its own 48x48 buffer and pushed straight to the
// panel, so animating it does not redraw the whole screen.
#pragma once

#include <cmath>
#include <cstring>
#include <string>
#include "homelab_status.h"

namespace weather {

static const char *const TAG = "weather";

struct Weather {
  bool ok = false;
  uint32_t updated_ms = 0;
  float temp = 0, tmax = 0, tmin = 0;
  int code = -1;
  bool is_day = true;
};
inline Weather &state() { static Weather w; return w; }

#ifndef USE_HOST
// location: "lat,lon"; unit: "C" or "F".
inline void fetch(const std::string &location, const std::string &unit) {
  auto &w = state();
  size_t comma = location.find(',');
  if (comma == std::string::npos) return;
  std::string lat = location.substr(0, comma), lon = location.substr(comma + 1);
  while (!lon.empty() && lon.front() == ' ') lon.erase(0, 1);
  std::string url = "http://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon +
                    "&current=temperature_2m,weather_code,is_day&daily=temperature_2m_max,temperature_2m_min"
                    "&forecast_days=1&timezone=auto&temperature_unit=" +
                    std::string(unit == "F" ? "fahrenheit" : "celsius");
  homelab::Request req;
  if (!req.begin(url)) return;
  int code = req.http().GET();
  if (code != 200) {
    ESP_LOGW(TAG, "Open-Meteo HTTP %d", code);
    w.ok = false;
    return;
  }
  JsonDocument filter;
  filter["current"]["temperature_2m"] = true;
  filter["current"]["weather_code"] = true;
  filter["current"]["is_day"] = true;
  filter["daily"]["temperature_2m_max"] = true;
  filter["daily"]["temperature_2m_min"] = true;
  JsonDocument doc;
  if (deserializeJson(doc, req.http().getStream(), DeserializationOption::Filter(filter))) {
    w.ok = false;
    return;
  }
  w.temp = doc["current"]["temperature_2m"] | 0.0f;
  w.code = doc["current"]["weather_code"] | -1;
  w.is_day = (doc["current"]["is_day"] | 1) == 1;
  w.tmax = doc["daily"]["temperature_2m_max"][0] | 0.0f;
  w.tmin = doc["daily"]["temperature_2m_min"][0] | 0.0f;
  w.ok = true;
  w.updated_ms = millis();
  ESP_LOGD(TAG, "%.1f %s, code %d", w.temp, unit.c_str(), w.code);
}

#endif  // USE_HOST

enum Kind { CLEAR, PARTLY, CLOUDY, FOG, DRIZZLE, RAIN, SNOW, THUNDER, UNKNOWN };

inline Kind kind(int code) {
  if (code < 0) return UNKNOWN;
  if (code <= 1) return CLEAR;
  if (code == 2) return PARTLY;
  if (code == 3) return CLOUDY;
  if (code == 45 || code == 48) return FOG;
  if (code >= 51 && code <= 57) return DRIZZLE;
  if ((code >= 61 && code <= 67) || (code >= 80 && code <= 82)) return RAIN;
  if ((code >= 71 && code <= 77) || code == 85 || code == 86) return SNOW;
  if (code >= 95) return THUNDER;
  return CLOUDY;
}

inline const char *describe(int code) {
  switch (kind(code)) {
    case CLEAR: return code == 0 ? "Clear" : "Mostly clear";
    case PARTLY: return "Partly cloudy";
    case CLOUDY: return "Cloudy";
    case FOG: return "Fog";
    case DRIZZLE: return "Drizzle";
    case RAIN: return code >= 80 ? "Showers" : "Rain";
    case SNOW: return "Snow";
    case THUNDER: return "Thunderstorm";
    default: return "";
  }
}

// ---- Tiny 48x48 RGB565 canvas -------------------------------------------
static const int S = 48;

struct Canvas {
  uint8_t px[S * S * 2];  // big-endian RGB565, as the panel expects
  static uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }
  void set(int x, int y, uint16_t c) {
    if (x < 0 || y < 0 || x >= S || y >= S) return;
    px[(y * S + x) * 2] = c >> 8;
    px[(y * S + x) * 2 + 1] = c & 0xFF;
  }
  uint16_t get(int x, int y) const { return (px[(y * S + x) * 2] << 8) | px[(y * S + x) * 2 + 1]; }
  void clear() { memset(px, 0, sizeof(px)); }
  void disc(float cx, float cy, float r, uint16_t c) {
    for (int y = (int) (cy - r - 1); y <= (int) (cy + r + 1); y++)
      for (int x = (int) (cx - r - 1); x <= (int) (cx + r + 1); x++)
        if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= r * r) set(x, y, c);
  }
  void rect(int x, int y, int w, int h, uint16_t c) {
    for (int j = y; j < y + h; j++)
      for (int i = x; i < x + w; i++) set(i, j, c);
  }
  void line(float x0, float y0, float x1, float y1, float th, uint16_t c) {
    float len = sqrtf((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
    int steps = (int) (len * 2) + 1;
    for (int i = 0; i <= steps; i++) {
      float t = (float) i / steps;
      disc(x0 + (x1 - x0) * t, y0 + (y1 - y0) * t, th / 2, c);
    }
  }
  void tri(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t c) {
    int minx = std::min({x0, x1, x2}), maxx = std::max({x0, x1, x2});
    int miny = std::min({y0, y1, y2}), maxy = std::max({y0, y1, y2});
    auto edge = [](int ax, int ay, int bx, int by, int px, int py) {
      return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
    };
    for (int y = miny; y <= maxy; y++)
      for (int x = minx; x <= maxx; x++) {
        int a = edge(x0, y0, x1, y1, x, y), b = edge(x1, y1, x2, y2, x, y), d = edge(x2, y2, x0, y0, x, y);
        if ((a >= 0 && b >= 0 && d >= 0) || (a <= 0 && b <= 0 && d <= 0)) set(x, y, c);
      }
  }
  void cloud(float x, float y, float s, uint16_t c) {  // x,y = bottom-left
    disc(x + 10 * s, y - 9 * s, 8 * s, c);
    disc(x + 21 * s, y - 14 * s, 11 * s, c);
    disc(x + 31 * s, y - 9 * s, 8 * s, c);
    rect((int) (x + 10 * s), (int) (y - 9 * s), (int) (21 * s), (int) (9 * s) + 1, c);
  }
};

inline Canvas &canvas() { static Canvas c; return c; }

// Draw frame f (increments ~5 times a second) for the given conditions.
inline void render(int code, bool day, uint32_t f) {
  auto &cv = canvas();
  cv.clear();
  const uint16_t sun = Canvas::rgb(255, 196, 60), ray = Canvas::rgb(255, 160, 40);
  const uint16_t moon = Canvas::rgb(225, 225, 255), star = Canvas::rgb(255, 255, 200);
  const uint16_t cloud = Canvas::rgb(205, 212, 225), dark = Canvas::rgb(120, 128, 145);
  const uint16_t drop = Canvas::rgb(90, 170, 255), snow = Canvas::rgb(240, 245, 255);
  const uint16_t bolt = Canvas::rgb(255, 225, 70), fog = Canvas::rgb(160, 168, 180);
  const float t = f * 0.2f;

  auto draw_sun = [&](float cx, float cy, float r) {
    for (int i = 0; i < 8; i++) {
      float a = i * 0.785398f + t * 0.35f;
      float pulse = 1.0f + 0.12f * sinf(t * 2 + i);
      cv.line(cx + cosf(a) * (r + 3), cy + sinf(a) * (r + 3), cx + cosf(a) * (r + 3 + 5 * pulse),
              cy + sinf(a) * (r + 3 + 5 * pulse), 2.5f, ray);
    }
    cv.disc(cx, cy, r, sun);
  };
  auto draw_moon = [&](float cx, float cy, float r) {
    cv.disc(cx, cy, r, moon);
    cv.disc(cx + r * 0.55f, cy - r * 0.35f, r * 0.85f, 0);
  };
  auto draw_stars = [&]() {
    const int sx[3] = {36, 41, 30}, sy[3] = {9, 22, 4};
    for (int i = 0; i < 3; i++)
      if (sinf(t * 1.7f + i * 2.1f) > -0.2f) cv.disc(sx[i], sy[i], 1.2f, star);
  };
  auto bob = [&](float amp) { return amp * sinf(t * 0.9f); };

  switch (kind(code)) {
    case CLEAR:
      if (day) draw_sun(24, 24, 10);
      else { draw_moon(21, 25, 13); draw_stars(); }
      break;
    case PARTLY:
      if (day) draw_sun(17, 17, 8);
      else draw_moon(16, 17, 10);
      cv.cloud(6 + bob(2), 41, 1.0f, cloud);
      break;
    case CLOUDY:
      cv.cloud(12 + bob(2.5f), 30, 0.8f, dark);
      cv.cloud(4 - bob(2), 43, 1.0f, cloud);
      break;
    case FOG:
      cv.cloud(6, 30, 0.9f, dark);
      for (int i = 0; i < 3; i++) {
        float off = 5 * sinf(t * 0.8f + i * 1.3f);
        cv.line(8 + off, 34 + i * 5, 40 + off, 34 + i * 5, 2.5f, fog);
      }
      break;
    case DRIZZLE:
    case RAIN: {
      bool heavy = kind(code) == RAIN;
      int n = heavy ? 4 : 3;
      for (int i = 0; i < n; i++) {
        float y = fmodf(f * (heavy ? 3.0f : 2.0f) + i * 7.0f, 18.0f);
        float x = 12 + i * (24.0f / n) - y * 0.25f;
        cv.line(x, 28 + y, x - 1.5f, 28 + y + (heavy ? 5 : 3), 2.0f, drop);
      }
      cv.cloud(4, 30, 1.0f, cloud);
      break;
    }
    case SNOW:
      for (int i = 0; i < 4; i++) {
        float y = fmodf(f * 1.2f + i * 5.0f, 18.0f);
        float x = 11 + i * 7 + 2.0f * sinf(t + i);
        cv.disc(x, 29 + y, 1.8f, snow);
      }
      cv.cloud(4, 30, 1.0f, cloud);
      break;
    case THUNDER: {
      bool flash = (f % 15) < 2;
      cv.cloud(4, 30, 1.0f, flash ? snow : dark);
      if ((f % 15) < 5) {
        cv.tri(26, 26, 18, 38, 24, 38, bolt);
        cv.tri(24, 36, 20, 47, 30, 34, bolt);
        cv.tri(22, 36, 30, 34, 24, 38, bolt);
      }
      break;
    }
    default:
      cv.disc(24, 24, 3, dark);
      break;
  }
}

// Push the current frame to the panel at x, y.
inline void push(esphome::display::Display &it, int x, int y) {
  it.draw_pixels_at(x, y, S, S, canvas().px, esphome::display::COLOR_ORDER_RGB,
                    esphome::display::COLOR_BITNESS_565, true);
}

// Draw the current frame through the normal (buffered) drawing path.
inline void draw(esphome::display::Display &it, int x, int y) {
  auto &cv = canvas();
  for (int j = 0; j < S; j++)
    for (int i = 0; i < S; i++) {
      uint16_t c = cv.get(i, j);
      if (c) it.draw_pixel_at(x + i, y + j, esphome::Color((c >> 8) & 0xF8, (c >> 3) & 0xFC, (c << 3) & 0xF8));
    }
}

}  // namespace weather
