// Settings and status page for the SmallTV homelab display.
// Talks to the ESPHome REST API on the device itself.
/* ZONES-BEGIN */const ZONES = {"Africa/Abidjan":"GMT0","Africa/Algiers":"CET-1","Africa/Bissau":"GMT0","Africa/Cairo":"EET-2EEST,M4.5.5/0,M10.5.4/24","Africa/Casablanca":"<+00>0","Africa/Ceuta":"CET-1CEST,M3.5.0,M10.5.0/3","Africa/El_Aaiun":"<+00>0","Africa/Johannesburg":"SAST-2","Africa/Juba":"CAT-2","Africa/Khartoum":"CAT-2","Africa/Lagos":"WAT-1","Africa/Maputo":"CAT-2","Africa/Monrovia":"GMT0","Africa/Nairobi":"EAT-3","Africa/Ndjamena":"WAT-1","Africa/Sao_Tome":"GMT0","Africa/Tripoli":"EET-2","Africa/Tunis":"CET-1","Africa/Windhoek":"CAT-2","America/Adak":"HST10HDT,M3.2.0,M11.1.0","America/Anchorage":"AKST9AKDT,M3.2.0,M11.1.0","America/Araguaina":"<-03>3","America/Argentina/Buenos_Aires":"<-03>3","America/Argentina/Catamarca":"<-03>3","America/Argentina/Cordoba":"<-03>3","America/Argentina/Jujuy":"<-03>3","America/Argentina/La_Rioja":"<-03>3","America/Argentina/Mendoza":"<-03>3","America/Argentina/Rio_Gallegos":"<-03>3","America/Argentina/Salta":"<-03>3","America/Argentina/San_Juan":"<-03>3","America/Argentina/San_Luis":"<-03>3","America/Argentina/Tucuman":"<-03>3","America/Argentina/Ushuaia":"<-03>3","America/Asuncion":"<-03>3","America/Bahia":"<-03>3","America/Bahia_Banderas":"CST6","America/Barbados":"AST4","America/Belem":"<-03>3","America/Belize":"CST6","America/Boa_Vista":"<-04>4","America/Bogota":"<-05>5","America/Boise":"MST7MDT,M3.2.0,M11.1.0","America/Cambridge_Bay":"MST7MDT,M3.2.0,M11.1.0","America/Campo_Grande":"<-04>4","America/Cancun":"EST5","America/Caracas":"<-04>4","America/Cayenne":"<-03>3","America/Chicago":"CST6CDT,M3.2.0,M11.1.0","America/Chihuahua":"CST6","America/Ciudad_Juarez":"MST7MDT,M3.2.0,M11.1.0","America/Costa_Rica":"CST6","America/Coyhaique":"<-03>3","America/Cuiaba":"<-04>4","America/Danmarkshavn":"GMT0","America/Dawson":"MST7","America/Dawson_Creek":"MST7","America/Denver":"MST7MDT,M3.2.0,M11.1.0","America/Detroit":"EST5EDT,M3.2.0,M11.1.0","America/Edmonton":"CST6","America/Eirunepe":"<-05>5","America/El_Salvador":"CST6","America/Fort_Nelson":"MST7","America/Fortaleza":"<-03>3","America/Glace_Bay":"AST4ADT,M3.2.0,M11.1.0","America/Goose_Bay":"AST4ADT,M3.2.0,M11.1.0","America/Grand_Turk":"EST5EDT,M3.2.0,M11.1.0","America/Guatemala":"CST6","America/Guayaquil":"<-05>5","America/Guyana":"<-04>4","America/Halifax":"AST4ADT,M3.2.0,M11.1.0","America/Havana":"CST5CDT,M3.2.0/0,M11.1.0/1","America/Hermosillo":"MST7","America/Indiana/Indianapolis":"EST5EDT,M3.2.0,M11.1.0","America/Indiana/Knox":"CST6CDT,M3.2.0,M11.1.0","America/Indiana/Marengo":"EST5EDT,M3.2.0,M11.1.0","America/Indiana/Petersburg":"EST5EDT,M3.2.0,M11.1.0","America/Indiana/Tell_City":"CST6CDT,M3.2.0,M11.1.0","America/Indiana/Vevay":"EST5EDT,M3.2.0,M11.1.0","America/Indiana/Vincennes":"EST5EDT,M3.2.0,M11.1.0","America/Indiana/Winamac":"EST5EDT,M3.2.0,M11.1.0","America/Inuvik":"CST6","America/Iqaluit":"EST5EDT,M3.2.0,M11.1.0","America/Jamaica":"EST5","America/Juneau":"AKST9AKDT,M3.2.0,M11.1.0","America/Kentucky/Louisville":"EST5EDT,M3.2.0,M11.1.0","America/Kentucky/Monticello":"EST5EDT,M3.2.0,M11.1.0","America/La_Paz":"<-04>4","America/Lima":"<-05>5","America/Los_Angeles":"PST8PDT,M3.2.0,M11.1.0","America/Maceio":"<-03>3","America/Managua":"CST6","America/Manaus":"<-04>4","America/Martinique":"AST4","America/Matamoros":"CST6CDT,M3.2.0,M11.1.0","America/Mazatlan":"MST7","America/Menominee":"CST6CDT,M3.2.0,M11.1.0","America/Merida":"CST6","America/Metlakatla":"AKST9AKDT,M3.2.0,M11.1.0","America/Mexico_City":"CST6","America/Miquelon":"<-03>3<-02>,M3.2.0,M11.1.0","America/Moncton":"AST4ADT,M3.2.0,M11.1.0","America/Monterrey":"CST6","America/Montevideo":"<-03>3","America/New_York":"EST5EDT,M3.2.0,M11.1.0","America/Nome":"AKST9AKDT,M3.2.0,M11.1.0","America/Noronha":"<-02>2","America/North_Dakota/Beulah":"CST6CDT,M3.2.0,M11.1.0","America/North_Dakota/Center":"CST6CDT,M3.2.0,M11.1.0","America/North_Dakota/New_Salem":"CST6CDT,M3.2.0,M11.1.0","America/Nuuk":"<-02>2<-01>,M3.5.0/-1,M10.5.0/0","America/Ojinaga":"CST6CDT,M3.2.0,M11.1.0","America/Panama":"EST5","America/Paramaribo":"<-03>3","America/Phoenix":"MST7","America/Port-au-Prince":"EST5EDT,M3.2.0,M11.1.0","America/Porto_Velho":"<-04>4","America/Puerto_Rico":"AST4","America/Punta_Arenas":"<-03>3","America/Rankin_Inlet":"CST6CDT,M3.2.0,M11.1.0","America/Recife":"<-03>3","America/Regina":"CST6","America/Resolute":"CST6CDT,M3.2.0,M11.1.0","America/Rio_Branco":"<-05>5","America/Santarem":"<-03>3","America/Santiago":"<-04>4<-03>,M9.1.6/24,M4.1.6/24","America/Santo_Domingo":"AST4","America/Sao_Paulo":"<-03>3","America/Scoresbysund":"<-02>2<-01>,M3.5.0/-1,M10.5.0/0","America/Sitka":"AKST9AKDT,M3.2.0,M11.1.0","America/St_Johns":"NST3:30NDT,M3.2.0,M11.1.0","America/Swift_Current":"CST6","America/Tegucigalpa":"CST6","America/Thule":"AST4ADT,M3.2.0,M11.1.0","America/Tijuana":"PST8PDT,M3.2.0,M11.1.0","America/Toronto":"EST5EDT,M3.2.0,M11.1.0","America/Vancouver":"MST7","America/Whitehorse":"MST7","America/Winnipeg":"EST5","America/Yakutat":"AKST9AKDT,M3.2.0,M11.1.0","Antarctica/Casey":"<+08>-8","Antarctica/Davis":"<+07>-7","Antarctica/Macquarie":"AEST-10AEDT,M10.1.0,M4.1.0/3","Antarctica/Mawson":"<+05>-5","Antarctica/Palmer":"<-03>3","Antarctica/Rothera":"<-03>3","Antarctica/Troll":"<+00>0<+02>-2,M3.5.0/1,M10.5.0/3","Antarctica/Vostok":"<+05>-5","Asia/Almaty":"<+05>-5","Asia/Amman":"<+03>-3","Asia/Anadyr":"<+12>-12","Asia/Aqtau":"<+05>-5","Asia/Aqtobe":"<+05>-5","Asia/Ashgabat":"<+05>-5","Asia/Atyrau":"<+05>-5","Asia/Baghdad":"<+03>-3","Asia/Baku":"<+04>-4","Asia/Bangkok":"<+07>-7","Asia/Barnaul":"<+07>-7","Asia/Beirut":"EET-2EEST,M3.5.0/0,M10.5.0/0","Asia/Bishkek":"<+06>-6","Asia/Chita":"<+09>-9","Asia/Colombo":"<+0530>-5:30","Asia/Damascus":"<+03>-3","Asia/Dhaka":"<+06>-6","Asia/Dili":"<+09>-9","Asia/Dubai":"<+04>-4","Asia/Dushanbe":"<+05>-5","Asia/Famagusta":"EET-2EEST,M3.5.0/3,M10.5.0/4","Asia/Gaza":"EET-2EEST,M3.4.4/50,M10.4.4/50","Asia/Hebron":"EET-2EEST,M3.4.4/50,M10.4.4/50","Asia/Ho_Chi_Minh":"<+07>-7","Asia/Hong_Kong":"HKT-8","Asia/Hovd":"<+07>-7","Asia/Irkutsk":"<+08>-8","Asia/Jakarta":"WIB-7","Asia/Jayapura":"WIT-9","Asia/Jerusalem":"IST-2IDT,M3.4.4/26,M10.5.0","Asia/Kabul":"<+0430>-4:30","Asia/Kamchatka":"<+12>-12","Asia/Karachi":"PKT-5","Asia/Kathmandu":"<+0545>-5:45","Asia/Khandyga":"<+09>-9","Asia/Kolkata":"IST-5:30","Asia/Krasnoyarsk":"<+07>-7","Asia/Kuching":"<+08>-8","Asia/Macau":"CST-8","Asia/Magadan":"<+11>-11","Asia/Makassar":"WITA-8","Asia/Manila":"PST-8","Asia/Nicosia":"EET-2EEST,M3.5.0/3,M10.5.0/4","Asia/Novokuznetsk":"<+07>-7","Asia/Novosibirsk":"<+07>-7","Asia/Omsk":"<+06>-6","Asia/Oral":"<+05>-5","Asia/Pontianak":"WIB-7","Asia/Pyongyang":"KST-9","Asia/Qatar":"<+03>-3","Asia/Qostanay":"<+05>-5","Asia/Qyzylorda":"<+05>-5","Asia/Riyadh":"<+03>-3","Asia/Sakhalin":"<+11>-11","Asia/Samarkand":"<+05>-5","Asia/Seoul":"KST-9","Asia/Shanghai":"CST-8","Asia/Singapore":"<+08>-8","Asia/Srednekolymsk":"<+11>-11","Asia/Taipei":"CST-8","Asia/Tashkent":"<+05>-5","Asia/Tbilisi":"<+04>-4","Asia/Tehran":"<+0330>-3:30","Asia/Thimphu":"<+06>-6","Asia/Tokyo":"JST-9","Asia/Tomsk":"<+07>-7","Asia/Ulaanbaatar":"<+08>-8","Asia/Urumqi":"<+06>-6","Asia/Ust-Nera":"<+10>-10","Asia/Vladivostok":"<+10>-10","Asia/Yakutsk":"<+09>-9","Asia/Yangon":"<+0630>-6:30","Asia/Yekaterinburg":"<+05>-5","Asia/Yerevan":"<+04>-4","Atlantic/Azores":"<-01>1<+00>,M3.5.0/0,M10.5.0/1","Atlantic/Bermuda":"AST4ADT,M3.2.0,M11.1.0","Atlantic/Canary":"WET0WEST,M3.5.0/1,M10.5.0","Atlantic/Cape_Verde":"<-01>1","Atlantic/Faroe":"WET0WEST,M3.5.0/1,M10.5.0","Atlantic/Madeira":"WET0WEST,M3.5.0/1,M10.5.0","Atlantic/South_Georgia":"<-02>2","Atlantic/Stanley":"<-03>3","Australia/Adelaide":"ACST-9:30ACDT,M10.1.0,M4.1.0/3","Australia/Brisbane":"AEST-10","Australia/Broken_Hill":"ACST-9:30ACDT,M10.1.0,M4.1.0/3","Australia/Darwin":"ACST-9:30","Australia/Eucla":"<+0845>-8:45","Australia/Hobart":"AEST-10AEDT,M10.1.0,M4.1.0/3","Australia/Lindeman":"AEST-10","Australia/Lord_Howe":"<+1030>-10:30<+11>-11,M10.1.0,M4.1.0","Australia/Melbourne":"AEST-10AEDT,M10.1.0,M4.1.0/3","Australia/Perth":"AWST-8","Australia/Sydney":"AEST-10AEDT,M10.1.0,M4.1.0/3","Europe/Andorra":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Astrakhan":"<+04>-4","Europe/Athens":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Belgrade":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Berlin":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Brussels":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Bucharest":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Budapest":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Chisinau":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Dublin":"IST-1GMT0,M10.5.0,M3.5.0/1","Europe/Gibraltar":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Helsinki":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Istanbul":"<+03>-3","Europe/Kaliningrad":"EET-2","Europe/Kirov":"MSK-3","Europe/Kyiv":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Lisbon":"WET0WEST,M3.5.0/1,M10.5.0","Europe/London":"GMT0BST,M3.5.0/1,M10.5.0","Europe/Madrid":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Malta":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Minsk":"<+03>-3","Europe/Moscow":"MSK-3","Europe/Paris":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Prague":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Riga":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Rome":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Samara":"<+04>-4","Europe/Saratov":"<+04>-4","Europe/Simferopol":"MSK-3","Europe/Sofia":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Tallinn":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Tirane":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Ulyanovsk":"<+04>-4","Europe/Vienna":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Vilnius":"EET-2EEST,M3.5.0/3,M10.5.0/4","Europe/Volgograd":"MSK-3","Europe/Warsaw":"CET-1CEST,M3.5.0,M10.5.0/3","Europe/Zurich":"CET-1CEST,M3.5.0,M10.5.0/3","Indian/Chagos":"<+06>-6","Indian/Maldives":"<+05>-5","Indian/Mauritius":"<+04>-4","Pacific/Apia":"<+13>-13","Pacific/Auckland":"NZST-12NZDT,M9.5.0,M4.1.0/3","Pacific/Bougainville":"<+11>-11","Pacific/Chatham":"<+1245>-12:45<+1345>,M9.5.0/2:45,M4.1.0/3:45","Pacific/Easter":"<-06>6<-05>,M9.1.6/22,M4.1.6/22","Pacific/Efate":"<+11>-11","Pacific/Fakaofo":"<+13>-13","Pacific/Fiji":"<+12>-12","Pacific/Galapagos":"<-06>6","Pacific/Gambier":"<-09>9","Pacific/Guadalcanal":"<+11>-11","Pacific/Guam":"ChST-10","Pacific/Honolulu":"HST10","Pacific/Kanton":"<+13>-13","Pacific/Kiritimati":"<+14>-14","Pacific/Kosrae":"<+11>-11","Pacific/Kwajalein":"<+12>-12","Pacific/Marquesas":"<-0930>9:30","Pacific/Nauru":"<+12>-12","Pacific/Niue":"<-11>11","Pacific/Norfolk":"<+11>-11<+12>,M10.1.0,M4.1.0/3","Pacific/Noumea":"<+11>-11","Pacific/Pago_Pago":"SST11","Pacific/Palau":"<+09>-9","Pacific/Pitcairn":"<-08>8","Pacific/Port_Moresby":"<+10>-10","Pacific/Rarotonga":"<-10>10","Pacific/Tahiti":"<-10>10","Pacific/Tarawa":"<+12>-12","Pacific/Tongatapu":"<+13>-13","UTC":"UTC0"};/* ZONES-END */
// ESPHome's page has no mobile viewport tag; add it before building the page.
for (const [name, content] of [
  ["viewport", "width=device-width, initial-scale=1, viewport-fit=cover"],
  ["theme-color", "#060912"],
  ["color-scheme", "dark"],
]) {
  const m = document.createElement("meta");
  m.name = name;
  m.content = content;
  document.head.appendChild(m);
}
document.title = "SmallTV";

const $ = (s) => document.querySelector(s);
const enc = encodeURIComponent;
const api = (kind, name, action = "") =>
  `/${kind}/${enc(name)}${action ? "/" + action : ""}`;
async function get(kind, name) {
  try {
    const r = await fetch(api(kind, name));
    return r.ok ? await r.json() : null;
  } catch { return null; }
}
const post = (kind, name, action, q = "") =>
  fetch(api(kind, name, action) + q, { method: "POST" });

const FIELDS = [
  ["label", "Clock label", "text"],
  ["ntp", "NTP servers", "text"],
  ["gatus", "Gatus URL", "url"],
  ["ph1", "Pi-hole 1 URL", "url"],
  ["ph1pw", "Pi-hole 1 app password", "password"],
  ["ph2", "Pi-hole 2 URL (optional)", "url"],
  ["ph2pw", "Pi-hole 2 app password", "password"],
];

document.body.insertAdjacentHTML("afterbegin", `
<main>
  <header>
    <div><h1>SmallTV</h1><div class="sub" id="ip">…</div></div>
    <span class="pill" id="health">checking…</span>
  </header>

  <section class="card">
    <div class="clock"><span class="z" id="zone">CLOCK</span><span class="sub" id="date"></span></div>
    <div class="clock"><span class="t" id="time">--:--</span><span class="wx" id="wx"></span></div>
    <div class="tiles">
      <div class="tile" id="svc"><div class="l">Services</div><div class="v" id="svcv">–</div><div class="l" id="svcl"></div></div>
      <div class="tile"><div class="l">Pi-hole blocked</div><div class="v" id="phv">–</div><div class="l" id="phl"></div></div>
    </div>
  </section>

  <form class="card" id="form" autocomplete="off">
    <h2>Clock</h2>
    <div class="row">
      <div><label for="label">Location name</label><input id="label" type="text" maxlength="20" placeholder="New York"></div>
      <div><label for="tz">Time zone</label><input id="tz" type="text" autocapitalize="off" autocorrect="off" spellcheck="false" list="zones" placeholder="America/New_York"></div>
    </div>
    <datalist id="zones"></datalist>
    <div class="row">
      <div><label for="unit">Temperature</label>
        <select id="unit"><option value="C">°C</option><option value="F">°F</option></select></div>
      <div><label>Weather location</label><div class="sub" id="wxloc" style="padding:11px 0">–</div></div>
    </div>
    <label for="ntp">NTP servers</label>
    <input id="ntp" type="text" inputmode="url" autocapitalize="off" autocorrect="off" spellcheck="false" placeholder="pool.ntp.org (default)">
    <div class="hint">Changing the location name looks the city up (Open-Meteo) and sets the weather and time zone for you.
      You can still pick a time zone yourself, or paste a POSIX rule. Up to three NTP servers, comma separated; empty uses the built-in pool.</div>

    <h2 style="margin-top:22px">Data sources</h2>
    <label for="gatus">Gatus URL</label>
    <input id="gatus" type="url" inputmode="url" autocapitalize="off" autocorrect="off" spellcheck="false" placeholder="http://gatus.lan:8080">
    <label for="ph1">Pi-hole 1</label>
    <div class="row pair">
      <input id="ph1" type="url" inputmode="url" autocapitalize="off" autocorrect="off" spellcheck="false" placeholder="http://pi.hole">
      <input id="ph1pw" type="password" autocomplete="new-password" placeholder="password">
    </div>
    <label for="ph2">Pi-hole 2 (optional)</label>
    <div class="row pair">
      <input id="ph2" type="url" inputmode="url" autocapitalize="off" autocorrect="off" spellcheck="false" placeholder="http://pihole2.lan">
      <input id="ph2pw" type="password" autocomplete="new-password" placeholder="password">
    </div>
    <div class="hint">Passwords are never shown again. Leave a password box empty to keep the saved one.
      An app password (Pi-hole: Settings → Web interface / API) is safer than the admin password.</div>
    <div class="savebar"><button class="primary" id="save" type="submit">Save settings</button></div>
  </form>

  <section class="card">
    <h2>Display</h2>
    <label for="bri">Brightness</label>
    <input id="bri" type="range" min="5" max="255">
  </section>

  <section class="card">
    <h2>Device</h2>
    <div class="line"><span>Last check</span><span id="last">–</span></div>
    <div class="line"><span>Device time</span><span id="dt">–</span></div>
    <div class="line"><span>Uptime</span><span id="up">–</span></div>
    <div class="line"><span>ESPHome</span><span id="ver">–</span></div>
    <div style="margin-top:12px"><button class="ghost" id="restart" type="button">Restart</button></div>
  </section>
</main>
<div class="toast" id="toast"></div>`);

function toast(msg) {
  const t = $("#toast");
  t.textContent = msg;
  t.classList.add("show");
  clearTimeout(toast.h);
  toast.h = setTimeout(() => t.classList.remove("show"), 2500);
}

let zone = "UTC";
let unit = "C";
let savedLabel = "";
function tick() {
  const now = new Date();
  try {
    $("#time").textContent = now.toLocaleTimeString("en-US", { timeZone: zone, hour: "2-digit", minute: "2-digit" });
    $("#date").textContent = now.toLocaleDateString("en-GB", { timeZone: zone, weekday: "short", day: "numeric", month: "short" });
  } catch { /* unknown zone */ }
}

const fmt = (n) => n >= 1e6 ? (n / 1e6).toFixed(1) + "M" : n >= 1e4 ? Math.round(n / 1e3) + "k" : n >= 1e3 ? (n / 1e3).toFixed(1) + "k" : String(n);
function dur(s) {
  const d = Math.floor(s / 86400), h = Math.floor(s % 86400 / 3600), m = Math.floor(s % 3600 / 60);
  return d ? `${d}d ${h}h` : h ? `${h}h ${m}m` : `${m}m`;
}

// Live values come from ESPHome's event stream: one connection, all states.
const S = {};
function render() {
  const v = (id) => (S[id] ? S[id].value : undefined);
  const txt = (id) => (S[id] ? S[id].state ?? S[id].value : undefined);
  if (txt("text_sensor/IP address")) $("#ip").textContent = txt("text_sensor/IP address");
  const clock = txt("text_sensor/Clock");
  if (clock) {
    const [label, tz] = clock.split("|");
    $("#zone").textContent = label;
    zone = tz || "UTC";
    tick();
  }
  const u = Number(v("sensor/Services up")), t = Number(v("sensor/Services total"));
  if (Number.isFinite(u) && Number.isFinite(t) && t > 0) {
    const bad = u < t;
    $("#svcv").textContent = bad ? `${t - u} down` : `${u}/${t}`;
    $("#svcl").textContent = bad ? txt("text_sensor/Services down") || "" : "All up";
    $("#svc").classList.toggle("bad", bad);
    const h = $("#health");
    h.textContent = bad ? "attention" : "all good";
    h.className = "pill " + (bad ? "bad" : "ok");
  }
  const pct = Number(v("sensor/Pi-hole blocked percent"));
  if (Number.isFinite(pct)) $("#phv").textContent = pct.toFixed(0) + "%";
  const blk = Number(v("sensor/Pi-hole blocked today"));
  if (Number.isFinite(blk)) $("#phl").textContent = fmt(blk) + " today";
  if (txt("text_sensor/Last check")) $("#last").textContent = txt("text_sensor/Last check");
  const wt = Number(v("sensor/Weather temperature")), wd = txt("text_sensor/Weather");
  if (Number.isFinite(wt)) $("#wx").textContent = `${Math.round(wt)}°${unit} ${wd || ""}`;
  if (txt("text_sensor/Device time")) $("#dt").textContent = txt("text_sensor/Device time");
  const upt = Number(v("sensor/Uptime"));
  if (Number.isFinite(upt)) $("#up").textContent = dur(upt);
  if (txt("text_sensor/ESPHome version")) $("#ver").textContent = txt("text_sensor/ESPHome version");
  const light = S["light/Backlight"];
  if (light && light.brightness !== undefined && document.activeElement !== $("#bri"))
    $("#bri").value = light.brightness;
}

function connect() {
  const es = new EventSource("/events");
  es.addEventListener("state", (e) => {
    try {
      const d = JSON.parse(e.data);
      S[d.id] = d;
      render();
    } catch { /* ignore */ }
  });
  es.onerror = () => { es.close(); setTimeout(connect, 3000); };
}

async function loadFields() {
  $("#zones").innerHTML = Object.keys(ZONES).map((z) => `<option value="${z}">`).join("");
  const tz = await get("text", "Time zone");
  if (tz) $("#tz").value = tz.value ?? tz.state ?? "";
  const u = await get("text", "Temperature unit");
  if (u) { unit = (u.value ?? u.state) === "F" ? "F" : "C"; $("#unit").value = unit; }
  const wl = await get("text", "Weather location");
  if (wl) $("#wxloc").textContent = wl.value ?? wl.state ?? "–";
  for (const [id, name, type] of FIELDS) {
    if (type === "password") continue;
    const s = await get("text", name);
    if (s) $("#" + id).value = s.value ?? s.state ?? "";
  }
  savedLabel = $("#label").value.trim();
}

$("#form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const btn = $("#save");
  btn.disabled = true;
  btn.textContent = "Saving…";
  try {
    // A new location name: look the city up for weather and time zone.
    const label = $("#label").value.trim();
    if (label && label !== savedLabel) {
      const r = await fetch("https://geocoding-api.open-meteo.com/v1/search?count=1&name=" + enc(label));
      const g = (await r.json()).results?.[0];
      if (!g) throw new Error(`location: no city called "${label}" found`);
      const loc = `${g.latitude.toFixed(3)},${g.longitude.toFixed(3)}`;
      if (!(await post("text", "Weather location", "set", "?value=" + enc(loc))).ok) throw new Error("weather location");
      $("#wxloc").textContent = loc;
      if (g.timezone && ZONES[g.timezone]) $("#tz").value = g.timezone;
      savedLabel = label;
    }
    const newUnit = $("#unit").value;
    if (!(await post("text", "Temperature unit", "set", "?value=" + newUnit)).ok) throw new Error("unit");
    unit = newUnit;
    // Time zone: a known zone name, or a raw POSIX rule such as "IST-5:30".
    const tz = $("#tz").value.trim();
    let rule = ZONES[tz];
    if (!rule && /^(<[^>]+>|[A-Za-z]{3,})[+-]?\d/.test(tz)) rule = tz;
    if (tz && !rule) throw new Error("time zone: pick one from the list");
    if (tz) {
      if (!(await post("text", "Time zone", "set", "?value=" + enc(tz))).ok) throw new Error("time zone");
      if (!(await post("text", "Time zone rule", "set", "?value=" + enc(rule))).ok) throw new Error("time zone");
    }
    for (const [id, name, type] of FIELDS) {
      const v = $("#" + id).value.trim();
      if (type === "password" && !v) continue; // keep the saved password
      const r = await post("text", name, "set", "?value=" + enc(v));
      if (!r.ok) throw new Error(name);
    }
    await post("button", "Save settings", "press");
    $("#ph1pw").value = $("#ph2pw").value = "";
    toast("Saved. Checking sources…");
  } catch (err) {
    toast("Could not save " + err.message);
  } finally {
    btn.disabled = false;
    btn.textContent = "Save settings";
  }
});

$("#bri").addEventListener("change", (e) =>
  post("light", "Backlight", "turn_on", "?brightness=" + e.target.value));
$("#restart").addEventListener("click", async () => {
  if (!confirm("Restart the display?")) return;
  await post("button", "Restart", "press");
  toast("Restarting…");
});

loadFields().then(connect);
setInterval(tick, 1000);
