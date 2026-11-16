// Fetches homelab health from Gatus and stats from Pi-hole v6, straight from
// their own APIs. Responses are stream-parsed with ArduinoJson filters, so
// memory use stays small however many Gatus endpoints you have.
#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <ArduinoJson.h>
#if defined(USE_ESP8266)
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#elif defined(USE_ESP32)
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#endif
// On the host platform (preview/) there is no network code: the preview
// fills the screen with demo data instead.
#include "esphome/core/log.h"
#include "esphome/components/display/display.h"
#include "esphome/components/time/posix_tz.h"
#ifdef USE_ESP8266
#include <lwip/apps/sntp.h>
#include <spi_flash.h>
#include <Esp.h>
#endif

namespace homelab {

static const char *const TAG = "homelab";

struct Gatus {
  bool ok = false;               // last fetch worked
  uint32_t updated_ms = 0;       // millis() of last good fetch
  int up = 0, total = 0;
  std::vector<std::string> down; // names of failing endpoints
};

struct PiholeStats {
  bool ok = false;
  uint32_t updated_ms = 0;
  long queries = 0, blocked = 0;
};

static std::string trim_url(std::string u) {
  while (!u.empty() && (u.back() == '/' || u.back() == ' ')) u.pop_back();
  return u;
}

#ifndef USE_HOST
// Opens url (http or https; https certificates are not checked) on a fresh client.
class Request {
 public:
  bool begin(const std::string &url) {
    if (url.rfind("https://", 0) == 0) {
#ifdef USE_ESP8266
      auto *c = new BearSSL::WiFiClientSecure();
      c->setInsecure();
      c->setBufferSizes(4096, 512);
#else
      auto *c = new WiFiClientSecure();
      c->setInsecure();
#endif
      client_ = c;
    } else {
      client_ = new WiFiClient();
    }
    http_.setTimeout(5000);
    http_.useHTTP10(true);  // no chunked encoding, so the body can be streamed
    return http_.begin(*client_, url.c_str());
  }
  HTTPClient &http() { return http_; }
  ~Request() {
    http_.end();
    delete client_;
  }

 private:
  WiFiClient *client_ = nullptr;
  HTTPClient http_;
};

// GET <base>/api/v1/endpoints/statuses?pageSize=1 and count the latest results.
inline void fetch_gatus(const std::string &base_url, Gatus &g, size_t max_names = 4) {
  std::string base = trim_url(base_url);
  if (base.empty()) return;
  Request req;
  if (!req.begin(base + "/api/v1/endpoints/statuses?page=1&pageSize=1")) {
    g.ok = false;
    return;
  }
  int code = req.http().GET();
  if (code != 200) {
    ESP_LOGW(TAG, "Gatus HTTP %d", code);
    g.ok = false;
    return;
  }
  JsonDocument filter;
  filter[0]["name"] = true;
  filter[0]["results"][0]["success"] = true;
  JsonDocument doc;
  auto err = deserializeJson(doc, req.http().getStream(), DeserializationOption::Filter(filter));
  if (err) {
    ESP_LOGW(TAG, "Gatus JSON: %s", err.c_str());
    g.ok = false;
    return;
  }
  int up = 0, total = 0;
  std::vector<std::string> down;
  for (JsonObject ep : doc.as<JsonArray>()) {
    JsonArray results = ep["results"];
    if (results.size() == 0) continue;
    total++;
    if (results[results.size() - 1]["success"] | false) {
      up++;
    } else if (down.size() < max_names) {
      down.emplace_back(ep["name"] | "?");
    }
  }
  g.up = up;
  g.total = total;
  g.down = down;
  g.ok = true;
  g.updated_ms = millis();
  ESP_LOGD(TAG, "Gatus: %d/%d up", up, total);
}

// One Pi-hole v6 instance. Logs in with an app password (if set) and reuses
// the session until Pi-hole rejects it.
class Pihole {
 public:
  void fetch(const std::string &base_url, const std::string &password, PiholeStats &out) {
    std::string base = trim_url(base_url);
    if (base.empty()) {
      out.ok = false;
      return;
    }
    if (base != base_) {
      base_ = base;
      sid_.clear();
    }
    int code = summary_(out);
    if (code == 401) {
      if (password.empty()) {
        ESP_LOGW(TAG, "Pi-hole %s needs a password: set it on the web page", base_.c_str());
        out.ok = false;
        return;
      }
      if (login_(password)) code = summary_(out);
    }
    if (code != 200) {
      ESP_LOGW(TAG, "Pi-hole %s: HTTP %d", base_.c_str(), code);
      out.ok = false;
    }
  }

 private:
  std::string base_, sid_;

  bool login_(const std::string &password) {
    Request req;
    if (!req.begin(base_ + "/api/auth")) return false;
    req.http().addHeader("Content-Type", "application/json");
    JsonDocument body;
    body["password"] = password;
    std::string payload;
    serializeJson(body, payload);
    int code = req.http().POST(payload.c_str());
    if (code != 200) {
      ESP_LOGW(TAG, "Pi-hole login failed: HTTP %d", code);
      return false;
    }
    JsonDocument filter;
    filter["session"]["sid"] = true;
    filter["session"]["valid"] = true;
    JsonDocument doc;
    if (deserializeJson(doc, req.http().getStream(), DeserializationOption::Filter(filter))) return false;
    if (!(doc["session"]["valid"] | false)) return false;
    sid_ = doc["session"]["sid"] | "";
    return true;
  }

  int summary_(PiholeStats &out) {
    Request req;
    if (!req.begin(base_ + "/api/stats/summary")) return -1;
    if (!sid_.empty()) req.http().addHeader("X-FTL-SID", sid_.c_str());
    int code = req.http().GET();
    if (code != 200) return code;
    JsonDocument filter;
    filter["queries"]["total"] = true;
    filter["queries"]["blocked"] = true;
    JsonDocument doc;
    if (deserializeJson(doc, req.http().getStream(), DeserializationOption::Filter(filter))) return -2;
    out.queries = doc["queries"]["total"] | 0L;
    out.blocked = doc["queries"]["blocked"] | 0L;
    out.ok = true;
    out.updated_ms = millis();
    ESP_LOGD(TAG, "Pi-hole %s: %ld queries, %ld blocked", base_.c_str(), out.queries, out.blocked);
    return 200;
  }
};

#else
// Host build (preview): no networking.
inline void fetch_gatus(const std::string &, Gatus &, size_t = 4) {}
class Pihole {
 public:
  void fetch(const std::string &, const std::string &, PiholeStats &) {}
};
#endif  // USE_HOST

// ---- Time zone and NTP, changeable at runtime ----------------------------

namespace tzparse {
// POSIX TZ strings, e.g. "CST6CDT,M3.2.0,M11.1.0" or "<+0530>-5:30".
inline bool name(const char *&p) {
  if (*p == '<') {
    while (*p && *p != '>') p++;
    if (*p != '>') return false;
    p++;
    return true;
  }
  const char *s = p;
  while ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z')) p++;
  return p - s >= 3;
}
inline bool num(const char *&p, int &v) {
  if (*p < '0' || *p > '9') return false;
  v = 0;
  while (*p >= '0' && *p <= '9') v = v * 10 + (*p++ - '0');
  return true;
}
// [+-]hh[:mm[:ss]] -> seconds
inline bool offset(const char *&p, int32_t &out) {
  int sign = 1;
  if (*p == '+' || *p == '-') sign = (*p++ == '-') ? -1 : 1;
  int h, m = 0, sec = 0;
  if (!num(p, h)) return false;
  if (*p == ':') { p++; if (!num(p, m)) return false; }
  if (*p == ':') { p++; if (!num(p, sec)) return false; }
  out = sign * (h * 3600 + m * 60 + sec);
  return true;
}
inline bool rule(const char *&p, esphome::time::DSTRule &r) {
  using esphome::time::DSTRuleType;
  int a, b, c;
  if (*p == 'M') {
    p++;
    if (!num(p, a) || *p++ != '.' || !num(p, b) || *p++ != '.' || !num(p, c)) return false;
    r.type = DSTRuleType::MONTH_WEEK_DAY;
    r.month = a; r.week = b; r.day_of_week = c; r.day = 0;
  } else if (*p == 'J') {
    p++;
    if (!num(p, a)) return false;
    r.type = DSTRuleType::JULIAN_NO_LEAP;
    r.day = a;
  } else {
    if (!num(p, a)) return false;
    r.type = DSTRuleType::DAY_OF_YEAR;
    r.day = a;
  }
  r.time_seconds = 7200;
  if (*p == '/') { p++; if (!offset(p, r.time_seconds)) return false; }
  return true;
}
}  // namespace tzparse

// Parse a POSIX TZ rule and make it the device time zone. Returns false (and
// changes nothing) if the rule is not understood.
inline bool apply_timezone(const std::string &posix) {
  using namespace esphome::time;
  if (posix.empty()) return false;
  ParsedTimezone tz{};
  const char *p = posix.c_str();
  if (!tzparse::name(p) || !tzparse::offset(p, tz.std_offset_seconds)) return false;
  tz.dst_offset_seconds = tz.std_offset_seconds;
  tz.dst_start.type = tz.dst_end.type = DSTRuleType::NONE;
  if (*p && *p != ',') {
    if (!tzparse::name(p)) return false;
    tz.dst_offset_seconds = tz.std_offset_seconds - 3600;
    if (*p && *p != ',' && !tzparse::offset(p, tz.dst_offset_seconds)) return false;
    // US rules are the POSIX default when a DST name has no rules.
    if (*p != ',') {
      tz.dst_start = {7200, 0, DSTRuleType::MONTH_WEEK_DAY, 3, 2, 0};
      tz.dst_end = {7200, 0, DSTRuleType::MONTH_WEEK_DAY, 11, 1, 0};
    } else {
      p++;
      if (!tzparse::rule(p, tz.dst_start) || *p++ != ',' || !tzparse::rule(p, tz.dst_end)) return false;
    }
  }
  if (*p) return false;
  set_global_tz(tz);
  ESP_LOGI(TAG, "Time zone set to %s", posix.c_str());
  return true;
}

// Point SNTP at up to three servers (comma separated). Empty means the
// public pool.
inline void apply_ntp_servers(const std::string &list_in) {
  const std::string list = list_in.empty() ? "0.pool.ntp.org,1.pool.ntp.org,2.pool.ntp.org" : list_in;
#ifdef USE_ESP8266
  static char names[3][64];
  int n = 0;
  size_t pos = 0;
  while (n < 3 && pos <= list.size()) {
    size_t comma = list.find(',', pos);
    if (comma == std::string::npos) comma = list.size();
    std::string s = list.substr(pos, comma - pos);
    while (!s.empty() && s.front() == ' ') s.erase(0, 1);
    while (!s.empty() && s.back() == ' ') s.pop_back();
    if (!s.empty()) {
      snprintf(names[n], sizeof(names[n]), "%s", s.c_str());
      n++;
    }
    pos = comma + 1;
  }
  if (n == 0) return;
  sntp_stop();
  for (int i = 0; i < 3; i++) sntp_setservername(i, i < n ? names[i] : nullptr);
  sntp_init();
  ESP_LOGI(TAG, "NTP servers set (%d)", n);
#endif
}

// ---- Settings store ------------------------------------------------------
// ESPHome's own preference area on the ESP8266 is only 256-512 bytes, too
// small for several URLs and passwords. Settings live in one 4 KB flash
// sector instead: at 2 MB, well clear of the sketch (first 1 MB) and of where
// OTA updates are staged (just below 0x5FB000). Needs a 4 MB flash chip.
namespace store {
static const uint32_t SECTOR = 0x200000 / 4096;  // 4 KB flash sectors
static const uint32_t MAGIC = 0x53545631;  // "STV1"
static const size_t MAX_JSON = 2048;

inline uint32_t crc32(const uint8_t *d, size_t n) {
  uint32_t c = 0xFFFFFFFF;
  while (n--) {
    c ^= *d++;
    for (int k = 0; k < 8; k++) c = (c >> 1) ^ (0xEDB88320 & -(c & 1));
  }
  return ~c;
}

inline bool available() {
#ifdef USE_ESP8266
  return ESP.getFlashChipRealSize() >= 4 * 1024 * 1024;
#else
  return false;
#endif
}

using Settings = std::vector<std::pair<std::string, std::string>>;

inline bool write(const Settings &kv) {
#ifdef USE_ESP8266
  if (!available()) return false;
  JsonDocument doc;
  for (auto &p : kv) doc[p.first] = p.second;
  std::string json;
  serializeJson(doc, json);
  if (json.size() > MAX_JSON) return false;
  size_t len = (12 + json.size() + 3) & ~3u;
  std::vector<uint32_t> buf(len / 4, 0xFFFFFFFF);
  auto *b = reinterpret_cast<uint8_t *>(buf.data());
  memcpy(b + 12, json.data(), json.size());
  buf[0] = MAGIC;
  buf[1] = json.size();
  buf[2] = crc32(b + 12, json.size());
  bool ok = spi_flash_erase_sector(SECTOR) == SPI_FLASH_RESULT_OK &&
            spi_flash_write(SECTOR * SPI_FLASH_SEC_SIZE, buf.data(), len) == SPI_FLASH_RESULT_OK;
  if (ok) ESP_LOGI(TAG, "Settings written (%u bytes)", (unsigned) json.size());
  else ESP_LOGE(TAG, "Settings write failed");
  return ok;
#else
  return false;
#endif
}

inline bool read(Settings &kv) {
#ifdef USE_ESP8266
  if (!available()) return false;
  uint32_t hdr[3];
  if (spi_flash_read(SECTOR * SPI_FLASH_SEC_SIZE, hdr, sizeof(hdr)) != SPI_FLASH_RESULT_OK) return false;
  if (hdr[0] != MAGIC || hdr[1] == 0 || hdr[1] > MAX_JSON) return false;
  size_t len = (12 + hdr[1] + 3) & ~3u;
  std::vector<uint32_t> buf(len / 4);
  if (spi_flash_read(SECTOR * SPI_FLASH_SEC_SIZE, buf.data(), len) != SPI_FLASH_RESULT_OK) return false;
  auto *b = reinterpret_cast<uint8_t *>(buf.data());
  if (crc32(b + 12, hdr[1]) != hdr[2]) {
    ESP_LOGW(TAG, "Stored settings are corrupt; ignoring them");
    return false;
  }
  JsonDocument doc;
  if (deserializeJson(doc, (const char *) (b + 12), hdr[1])) return false;
  kv.clear();
  for (JsonPair p : doc.as<JsonObject>()) kv.emplace_back(p.key().c_str(), p.value().as<const char *>());
  return true;
#else
  return false;
#endif
}
}  // namespace store

// Shared state, kept here because ESPHome globals are declared before includes.
inline Gatus &gatus() { static Gatus g; return g; }
inline Pihole &pihole(int i) { static Pihole p[2]; return p[i]; }
inline PiholeStats &pihole_stats(int i) { static PiholeStats s[2]; return s[i]; }

// Filled rectangle with rounded corners (the display API has none).
inline void rounded_box(esphome::display::Display &it, int x, int y, int w, int h, int r,
                        esphome::Color c) {
  it.filled_rectangle(x + r, y, w - 2 * r, h, c);
  it.filled_rectangle(x, y + r, r, h - 2 * r, c);
  it.filled_rectangle(x + w - r, y + r, r, h - 2 * r, c);
  it.filled_circle(x + r, y + r, r, c);
  it.filled_circle(x + w - r - 1, y + r, r, c);
  it.filled_circle(x + r, y + h - r - 1, r, c);
  it.filled_circle(x + w - r - 1, y + h - r - 1, r, c);
}

// Rounded card with a 1px border.
inline void card(esphome::display::Display &it, int x, int y, int w, int h, int r, esphome::Color fill,
                 esphome::Color border) {
  rounded_box(it, x, y, w, h, r, border);
  rounded_box(it, x + 1, y + 1, w - 2, h - 2, r - 1, fill);
}

// Pseudo-angle of (dx, dy) in [0, 4096): monotonic with the real angle
// (clockwise from +x, screen coordinates) but needs only integer maths.
inline int pseudo_angle(int dx, int dy) {
  int s = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
  if (s == 0) return 0;
  if (dy >= 0) return dx >= 0 ? (dy << 10) / s : 1024 + ((-dx) << 10) / s;
  return dx < 0 ? 2048 + ((-dy) << 10) / s : 3072 + (dx << 10) / s;
}

// 270-degree ring gauge with round caps, open at the bottom. pct is 0..100.
inline void gauge(esphome::display::Display &it, int cx, int cy, int r_out, int r_in, float pct,
                  esphome::Color track, esphome::Color fill) {
  const float start = 135.0f, sweep = 270.0f;
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  const float end = sweep * pct / 100.0f;
  auto pa = [](float deg) {
    float rad = deg / 57.29578f;
    return pseudo_angle((int) lroundf(1000 * cosf(rad)), (int) lroundf(1000 * sinf(rad)));
  };
  const int p_start = pa(start), p_sweep_end = pa(start + sweep), p_fill_end = pa(start + end);
  auto rel = [&](int p) { return (p - p_start + 8192) & 4095; };
  const int r_sweep = rel(p_sweep_end), r_fill = pct >= 100 ? r_sweep : rel(p_fill_end);
  const int ro2 = r_out * r_out, ri2 = r_in * r_in;
  for (int dy = -r_out; dy <= r_out; dy++) {
    for (int dx = -r_out; dx <= r_out; dx++) {
      int d2 = dx * dx + dy * dy;
      if (d2 > ro2 || d2 < ri2) continue;
      int r = rel(pseudo_angle(dx, dy));
      if (r > r_sweep) continue;
      it.draw_pixel_at(cx + dx, cy + dy, (pct > 0 && r <= r_fill) ? fill : track);
    }
  }
  // Round caps at both ends of the coloured part.
  float rm = (r_out + r_in) / 2.0f, rc = (r_out - r_in) / 2.0f;
  auto cap = [&](float deg, esphome::Color c) {
    float rad = deg / 57.29578f;
    it.filled_circle(cx + (int) lroundf(rm * cosf(rad)), cy + (int) lroundf(rm * sinf(rad)), (int) rc, c);
  };
  cap(start + sweep, pct >= 100 ? fill : track);
  cap(start, pct > 0 ? fill : track);
  if (pct > 0) cap(start + end, fill);
}

// Short count for a small screen: 950, 12.8k, 1.2M.
inline std::string short_count(long n) {
  char buf[16];
  if (n >= 1000000) snprintf(buf, sizeof(buf), "%.1fM", n / 1e6);
  else if (n >= 10000) snprintf(buf, sizeof(buf), "%.0fk", n / 1e3);
  else if (n >= 1000) snprintf(buf, sizeof(buf), "%.1fk", n / 1e3);
  else snprintf(buf, sizeof(buf), "%ld", n);
  return buf;
}

}  // namespace homelab
