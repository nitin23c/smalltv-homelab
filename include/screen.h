// Draws the whole 240x240 screen from a plain View. The device fills the View
// from its sensors; preview/preview.yaml fills it with demo data, so README
// screenshots come from the exact same drawing code.
#pragma once

#include <cmath>
#include <string>
#include <vector>
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
#include "esphome/components/image/image.h"
#include "esphome/core/time.h"
#include "homelab_status.h"
#include "weather_icon.h"

namespace face {

using esphome::Color;
using esphome::display::Display;
using esphome::display::TextAlign;

struct Res {
  esphome::font::Font *time, *temp, *value, *text, *label;
  esphome::image::Image *day, *night, *services, *pihole;
};

struct View {
  bool wifi = true;
  std::string ap_password;
  esphome::ESPTime now{};
  std::string label;
  // Weather
  bool wx_set = false, wx_ok = false;
  float temp = 0, tmax = 0, tmin = 0;
  int code = -1;
  std::string unit = "C";
  // Gatus
  bool g_set = false, g_fresh = false;
  int up = 0, total = 0;
  std::vector<std::string> down;
  // Pi-hole
  bool p_set = false, p_any = false;
  long queries = 0, blocked = 0;
};

inline void draw(Display &it, const Res &r, const View &v) {
  // This panel exaggerates very dark colours, so the background is true black
  // and cards are outlines.
  const Color bg(0, 0, 0), card_line(52, 60, 78), track(44, 50, 66);
  const Color text(242, 244, 248), dim(150, 158, 175), accent(94, 200, 255);
  const Color amber(255, 190, 60), lavender(170, 160, 255);
  const Color green(52, 211, 153), red(248, 98, 98), pihole(240, 70, 110);

  it.fill(bg);
  if (!v.wifi) {
    homelab::card(it, 10, 10, 220, 220, 16, bg, card_line);
    it.print(120, 34, r.label, accent, TextAlign::CENTER, "WI-FI SETUP");
    it.print(120, 72, r.text, dim, TextAlign::CENTER, "Join the hotspot");
    it.print(120, 96, r.text, text, TextAlign::CENTER, "SmallTV Setup");
    it.print(120, 134, r.text, dim, TextAlign::CENTER, "Password");
    it.print(120, 158, r.text, text, TextAlign::CENTER, v.ap_password.c_str());
    it.print(120, 198, r.text, dim, TextAlign::CENTER, "then open 192.168.4.1");
    return;
  }
  auto now = v.now;  // copy: ESPTime::strftime is not const
  const bool day = now.is_valid() ? (now.hour >= 6 && now.hour < 18) : true;

  // ---- Header: day/night icon, city, date ----------------------------------
  it.image(8, 7, day ? r.day : r.night, day ? amber : lavender);
  int date_w = 0;
  if (now.is_valid()) {
    char date[16];
    now.strftime(date, sizeof(date), "%a %d");
    int bx, by, bh;
    it.get_text_bounds(232, 19, date, r.text, TextAlign::CENTER_RIGHT, &bx, &by, &date_w, &bh);
    it.print(232, 19, r.text, dim, TextAlign::CENTER_RIGHT, date);
  }
  // The city name is shortened if needed, to keep a gap before the date.
  std::string label = v.label;
  const int max_w = 232 - date_w - 14 - 38;
  for (bool cut = false;; cut = true) {
    std::string shown = cut ? label + "." : label;
    int bx, by, bw, bh;
    it.get_text_bounds(38, 19, shown.c_str(), r.text, TextAlign::CENTER_LEFT, &bx, &by, &bw, &bh);
    if (bw <= max_w || label.size() <= 1) {
      it.print(38, 19, r.text, accent, TextAlign::CENTER_LEFT, shown.c_str());
      break;
    }
    label.pop_back();
    while (!label.empty() && label.back() == ' ') label.pop_back();
  }

  // ---- Time + AM/PM ----------------------------------------------------------
  if (now.is_valid()) {
    char hm[8];
    now.strftime(hm, sizeof(hm), "%I:%M");
    int x1, y1, w1, h1;
    it.get_text_bounds(8, 40, hm, r.time, TextAlign::TOP_LEFT, &x1, &y1, &w1, &h1);
    it.print(8, 40, r.time, text, TextAlign::TOP_LEFT, hm);
    char ampm[4];
    now.strftime(ampm, sizeof(ampm), "%p");
    it.print(8 + w1 + 5, 50, r.text, accent, TextAlign::TOP_LEFT, ampm);
  } else {
    it.print(8, 60, r.text, dim, TextAlign::TOP_LEFT, "Getting time...");
  }

  // ---- Weather: icon, temperature with unit, conditions --------------------
  if (v.wx_ok) {
    weather::draw(it, 186, 32);
    auto deg = [](float x) { int n = (int) lroundf(x); return n == 0 ? 0 : n; };  // no "-0"
    char t[12];
    snprintf(t, sizeof(t), "%d°%s", deg(v.temp), v.unit == "F" ? "F" : "C");
    int bx, by, bw, bh;
    it.get_text_bounds(0, 0, t, r.temp, TextAlign::TOP_LEFT, &bx, &by, &bw, &bh);
    int cx = std::min(210, 236 - bw / 2);  // centred under the icon, never off-screen
    it.print(cx, 80, r.temp, text, TextAlign::TOP_CENTER, t);
    it.print(8, 110, r.text, dim, TextAlign::TOP_LEFT, weather::describe(v.code));
    char hl[24];
    snprintf(hl, sizeof(hl), "H %d°  L %d°", deg(v.tmax), deg(v.tmin));
    it.print(232, 110, r.text, dim, TextAlign::TOP_RIGHT, hl);
  } else if (v.wx_set) {
    it.print(210, 62, r.text, dim, TextAlign::CENTER, "...");
  }

  // ---- Day progress bar --------------------------------------------------------
  if (now.is_valid()) {
    int w_bar = 224 * (now.hour * 60 + now.minute) / 1440;
    homelab::rounded_box(it, 8, 130, 224, 4, 2, track);
    if (w_bar > 4) homelab::rounded_box(it, 8, 130, w_bar, 4, 2, day ? amber : lavender);
  }

  // ---- Services card (Gatus) ---------------------------------------------------
  const int cy = 189;
  const bool g_bad = v.g_fresh && v.up < v.total;
  const Color g_col = !v.g_fresh ? dim : (g_bad ? red : green);
  homelab::card(it, 6, 140, 112, 98, 12, bg, g_bad ? red : card_line);
  it.image(14, 144, r.services, g_bad ? red : dim);
  it.print(32, 151, r.label, g_bad ? red : dim, TextAlign::CENTER_LEFT, "SERVICES");
  float g_pct = v.g_fresh && v.total > 0 ? 100.0f * v.up / v.total : 0;
  homelab::gauge(it, 62, cy, 30, 25, g_pct, track, g_col);
  if (!v.g_set || !v.g_fresh) {
    it.print(62, cy, r.value, dim, TextAlign::CENTER, "?");
    it.print(62, 227, r.text, dim, TextAlign::CENTER, v.g_set ? "No data" : "Not set");
  } else {
    it.printf(62, cy - 8, r.value, text, TextAlign::CENTER, "%d", v.up);
    it.printf(62, cy + 8, r.label, dim, TextAlign::CENTER, "OF %d", v.total);
    if (!g_bad) {
      it.print(62, 227, r.text, green, TextAlign::CENTER, "All up");
    } else if (!v.down.empty()) {
      // Rotate through what is down, one name per refresh.
      std::string n = v.down[(now.is_valid() ? now.timestamp / 15 : 0) % v.down.size()];
      if (n.size() > 13) n = n.substr(0, 12) + ".";
      it.print(62, 227, r.text, red, TextAlign::CENTER, n.c_str());
    }
  }

  // ---- Pi-hole card ------------------------------------------------------------
  homelab::card(it, 122, 140, 112, 98, 12, bg, card_line);
  it.image(130, 144, r.pihole, v.p_any ? pihole : dim);
  it.print(148, 151, r.label, dim, TextAlign::CENTER_LEFT, "PI-HOLE");
  float pct = v.p_any && v.queries > 0 ? 100.0f * v.blocked / v.queries : 0;
  homelab::gauge(it, 178, cy, 30, 25, pct, track, pihole);
  if (!v.p_set || !v.p_any) {
    it.print(178, cy, r.value, dim, TextAlign::CENTER, "?");
    it.print(178, 227, r.text, dim, TextAlign::CENTER, v.p_set ? "No data" : "Not set");
  } else {
    it.printf(178, cy - 2, r.value, text, TextAlign::CENTER, "%.0f%%", pct);
    it.printf(178, 227, r.text, dim, TextAlign::CENTER, "%s blocked", homelab::short_count(v.blocked).c_str());
  }
}

}  // namespace face
