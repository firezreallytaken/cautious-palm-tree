#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-C3 Web Lab v5</title>
  <style>
    * { box-sizing: border-box; }

    body {
      margin: 0;
      font-family: system-ui, -apple-system, BlinkMacSystemFont, sans-serif;
      background:
        radial-gradient(circle at top left, rgba(124,255,155,0.12), transparent 30%),
        radial-gradient(circle at top right, rgba(120,160,255,0.12), transparent 28%),
        #0b0b0e;
      color: white;
      padding: 18px;
    }

    .wrap {
      max-width: 900px;
      margin: 0 auto;
    }

    .hero {
      background: linear-gradient(135deg, #1c1c22, #101014);
      border: 1px solid #333;
      border-radius: 28px;
      padding: 20px;
      margin-bottom: 14px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.35);
    }

    h1 {
      margin: 0;
      font-size: 34px;
      letter-spacing: -1px;
    }

    .sub {
      color: #aaa;
      margin-top: 6px;
      line-height: 1.4;
    }

    .grid {
      display: grid;
      gap: 12px;
    }

    .stats {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 10px;
      margin-top: 16px;
    }

    .stat {
      background: #111116;
      border: 1px solid #2e2e35;
      border-radius: 18px;
      padding: 12px;
    }

    .stat .key {
      color: #888;
      font-size: 13px;
    }

    .stat .value {
      font-weight: 900;
      overflow-wrap: anywhere;
      margin-top: 4px;
    }

    .card {
      background: #18181d;
      border: 1px solid #333;
      border-radius: 24px;
      padding: 16px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.25);
    }

    .title {
      font-size: 20px;
      font-weight: 950;
      margin-bottom: 10px;
    }

    .notice {
      color: #aaa;
      font-size: 14px;
      line-height: 1.45;
    }

    .buttons {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 10px;
    }

    button, input[type="submit"] {
      border: 0;
      border-radius: 16px;
      padding: 14px;
      font-size: 15px;
      font-weight: 900;
      background: white;
      color: black;
    }

    button.dark {
      background: #2c2c34;
      color: white;
      border: 1px solid #444;
    }

    button.danger {
      background: #ff4d4d;
      color: black;
    }

    button:active, .appcard:active {
      transform: scale(0.98);
    }

    input, textarea, select {
      width: 100%;
      border: 1px solid #444;
      background: #101014;
      color: white;
      border-radius: 14px;
      padding: 12px;
      font: inherit;
      margin: 6px 0 10px;
    }

    textarea {
      min-height: 190px;
      font-family: ui-monospace, SFMono-Regular, Menlo, monospace;
      font-size: 13px;
    }

    .apps {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(160px, 1fr));
      gap: 12px;
      margin-top: 12px;
    }

    .appcard {
      background: linear-gradient(180deg, #24242b, #151519);
      border: 1px solid #3a3a42;
      border-radius: 22px;
      padding: 14px;
      text-decoration: none;
      color: white;
      min-height: 145px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
    }

    .appicon {
      width: 46px;
      height: 46px;
      border-radius: 16px;
      background: white;
      color: black;
      display: grid;
      place-items: center;
      font-size: 24px;
      font-weight: 950;
      margin-bottom: 10px;
    }

    .appname {
      font-weight: 950;
      overflow-wrap: anywhere;
      line-height: 1.1;
    }

    .appmeta {
      color: #aaa;
      font-size: 12px;
      margin-top: 6px;
      overflow-wrap: anywhere;
    }

    .fileRow {
      display: grid;
      grid-template-columns: 1fr auto;
      gap: 8px;
      align-items: center;
      border-top: 1px solid #303038;
      padding: 10px 0;
    }

    .fileRow:first-child {
      border-top: 0;
    }

    .fileName {
      font-weight: 800;
      overflow-wrap: anywhere;
    }

    .fileMeta {
      color: #888;
      font-size: 12px;
      margin-top: 3px;
    }

    .miniBtn {
      padding: 10px 12px;
      font-size: 13px;
      border-radius: 12px;
    }

    .morseBox {
      background: #101014;
      border: 1px solid #333;
      border-radius: 14px;
      padding: 12px;
      font-family: ui-monospace, monospace;
      overflow-wrap: anywhere;
      min-height: 44px;
    }

    code {
      background: #25252b;
      padding: 2px 6px;
      border-radius: 8px;
    }

    @media (max-width: 520px) {
      .stats {
        grid-template-columns: 1fr;
      }

      h1 {
        font-size: 30px;
      }
    }
  </style>
</head>
<body>
  <div class="wrap">
    <div class="hero">
      <h1>ESP32-C3 Web Lab</h1>
      <div class="sub">Offline applet launcher, LED tools, Wi-Fi scan, and tiny storage.</div>

      <div class="stats">
        <div class="stat"><div class="key">Wi-Fi</div><div class="value" id="ssid">...</div></div>
        <div class="stat"><div class="key">IP</div><div class="value" id="ip">...</div></div>
        <div class="stat"><div class="key">Uptime</div><div class="value" id="uptime">...</div></div>
        <div class="stat"><div class="key">Storage</div><div class="value" id="storage">...</div></div>
        <div class="stat"><div class="key">Heap</div><div class="value" id="heap">...</div></div>
        <div class="stat"><div class="key">LED</div><div class="value" id="led">...</div></div>
      </div>
    </div>

    <div class="grid">
      <div class="card">
        <div class="title">Applets</div>
        <div class="notice">HTML applets saved on the ESP32. Tap one to open it.</div>
        <div class="apps" id="apps">Loading...</div>
      </div>

      <div class="card">
        <div class="title">Quick Controls</div>
        <div class="buttons">
          <button onclick="api('/api/led/on')">LED ON</button>
          <button onclick="api('/api/led/off')">LED OFF</button>
          <button onclick="api('/api/led/toggle')" class="dark">Toggle</button>
          <button onclick="api('/api/blink?times=5')" class="dark">Blink</button>
          <button onclick="scanWifi()" class="dark">Scan Wi-Fi</button>
          <button onclick="reboot()" class="danger">Reboot</button>
        </div>
      </div>

      <div class="card">
        <div class="title">Morse LED</div>
        <input id="morseText" value="SOS">
        <select id="morseSpeed">
          <option value="80">Fast</option>
          <option value="120" selected>Normal</option>
          <option value="200">Slow</option>
        </select>
        <div class="morseBox" id="morseOut">...</div>
        <br>
        <button onclick="playMorse()">Blink Morse on Blue LED</button>
      </div>

      <div class="card">
        <div class="title">Upload Applet</div>
        <div class="notice">Upload small single-file HTML applets. Limit is shown in status API.</div>
        <input type="file" id="fileInput" accept=".html,.htm,.css,.js,.json,.txt">
        <button onclick="uploadFile()">Upload File</button>
      </div>

      <div class="card">
        <div class="title">Create Applet</div>
        <input id="newName" value="hello.html">
        <textarea id="newContent"><!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Hello ESP32</title>
  <style>
    body { font-family: system-ui; background: #111; color: white; padding: 24px; }
    button { padding: 16px; border-radius: 14px; border: 0; font-weight: 900; }
  </style>
</head>
<body>
  <h1>Hello from ESP32-C3</h1>
  <p>This page was saved without rebuilding firmware.</p>
  <button onclick="fetch('/api/led/toggle')">Toggle LED</button>
</body>
</html></textarea>
        <button onclick="saveMiniApp()">Save Applet</button>
      </div>

      <div class="card">
        <div class="title">Stored Files</div>
        <div class="notice">Text and JSON files saved by applets also appear here.</div>
        <div id="files">Loading...</div>
      </div>

      <div class="card">
        <div class="title">Change ESP32 Wi-Fi</div>
        <div class="notice">Use a stronger password than the default. After saving, reboot and reconnect.</div>
        <input id="wifiName" placeholder="Wi-Fi name">
        <input id="wifiPass" placeholder="Password, 8+ chars or empty">
        <button onclick="saveSettings()">Save Wi-Fi Settings</button>
      </div>

      <div class="card">
        <div class="title">Wi-Fi Scan</div>
        <div id="scan">Tap <code>Scan Wi-Fi</code> to scan nearby networks.</div>
      </div>
    </div>
  </div>

  <script>
    function formatBytes(bytes) {
      if (bytes > 1024 * 1024) return (bytes / 1024 / 1024).toFixed(2) + ' MB';
      if (bytes > 1024) return (bytes / 1024).toFixed(1) + ' KB';
      return bytes + ' B';
    }

    function formatTime(sec) {
      const h = Math.floor(sec / 3600);
      const m = Math.floor((sec % 3600) / 60);
      const s = sec % 60;
      if (h > 0) return h + 'h ' + m + 'm ' + s + 's';
      if (m > 0) return m + 'm ' + s + 's';
      return s + 's';
    }

    function escapeHtml(s) {
      return String(s).replace(/[&<>"']/g, c => ({
        '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#039;'
      }[c]));
    }

    function appIcon(name) {
      name = name.toLowerCase();
      if (name.includes('calc')) return 'C';
      if (name.includes('note')) return 'N';
      if (name.includes('pixel')) return 'P';
      if (name.includes('dice')) return 'D';
      if (name.includes('timer')) return 'T';
      if (name.includes('morse')) return 'M';
      if (name.includes('clock')) return 'K';
      if (name.includes('terminal')) return '>';
      return 'A';
    }

    function niceName(name) {
      return name.replace(/\.(html|htm|txt|json|css|js)$/i, '').replace(/[-_]/g, ' ');
    }

    async function loadStatus() {
      try {
        const res = await fetch('/api/status?t=' + Date.now());
        const d = await res.json();

        document.getElementById('ssid').textContent = d.ssid;
        document.getElementById('ip').textContent = d.ip;
        document.getElementById('uptime').textContent = formatTime(d.uptime);
        document.getElementById('storage').textContent = formatBytes(d.fs_used) + ' / ' + formatBytes(d.fs_total);
        document.getElementById('heap').textContent = formatBytes(d.free_heap);
        document.getElementById('led').textContent = d.led;
        document.getElementById('wifiName').value = d.ssid;
      } catch (e) {
        document.getElementById('ssid').textContent = 'Disconnected';
      }
    }

    async function api(path) {
      await fetch(path + (path.includes('?') ? '&' : '?') + 't=' + Date.now());
      await loadStatus();
    }

    async function loadApps() {
      try {
        const res = await fetch('/api/apps?t=' + Date.now());
        const list = await res.json();

        const applets = list.filter(x => x.kind === 'applet');
        const files = list.filter(x => x.kind !== 'applet');

        const appsBox = document.getElementById('apps');
        if (!applets.length) {
          appsBox.innerHTML = '<div class="notice">No applets found.</div>';
        } else {
          appsBox.innerHTML = applets.map(app => `
            <a class="appcard" href="${escapeHtml(app.url)}&t=${Date.now()}" target="_blank">
              <div>
                <div class="appicon">${escapeHtml(appIcon(app.name))}</div>
                <div class="appname">${escapeHtml(niceName(app.name))}</div>
                <div class="appmeta">${formatBytes(app.size)} · ${escapeHtml(app.name)}</div>
              </div>
            </a>
          `).join('');
        }

        const filesBox = document.getElementById('files');
        if (!files.length) {
          filesBox.innerHTML = '<div class="notice">No extra stored files yet.</div>';
        } else {
          filesBox.innerHTML = files.map(file => `
            <div class="fileRow">
              <div>
                <div class="fileName">${escapeHtml(file.name)}</div>
                <div class="fileMeta">${escapeHtml(file.kind)} · ${formatBytes(file.size)}</div>
              </div>
              <div>
                <button class="miniBtn danger" onclick="deleteApp('${escapeHtml(file.name)}')">Delete</button>
              </div>
            </div>
          `).join('');
        }
      } catch (e) {
        document.getElementById('apps').textContent = 'Failed to load applets.';
      }
    }

    async function uploadFile() {
      const input = document.getElementById('fileInput');
      if (!input.files.length) {
        alert('Choose a file first.');
        return;
      }

      const data = new FormData();
      data.append('file', input.files[0]);

      const res = await fetch('/api/upload?t=' + Date.now(), { method: 'POST', body: data });
      const text = await res.text();

      input.value = '';
      console.log(text);

      await loadApps();
      await loadStatus();
    }

    async function saveMiniApp() {
      const data = new URLSearchParams();
      data.append('name', document.getElementById('newName').value);
      data.append('content', document.getElementById('newContent').value);

      const res = await fetch('/api/save?t=' + Date.now(), {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: data
      });

      alert(await res.text());
      await loadApps();
      await loadStatus();
    }

    async function deleteApp(name) {
      if (!confirm('Delete ' + name + '?')) return;

      const data = new URLSearchParams();
      data.append('name', name);

      await fetch('/api/delete?t=' + Date.now(), {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: data
      });

      await loadApps();
      await loadStatus();
    }

    async function updateMorsePreview() {
      const text = document.getElementById('morseText').value;
      const res = await fetch('/api/morse/encode?text=' + encodeURIComponent(text) + '&t=' + Date.now());
      const d = await res.json();
      document.getElementById('morseOut').textContent = d.morse || '(nothing)';
    }

    async function playMorse() {
      const text = document.getElementById('morseText').value;
      const unit = document.getElementById('morseSpeed').value;

      document.getElementById('morseOut').textContent = 'Playing...';

      const res = await fetch('/api/morse/play?text=' + encodeURIComponent(text) + '&unit=' + unit + '&t=' + Date.now());
      const d = await res.json();

      document.getElementById('morseOut').textContent = d.morse || '(nothing)';
      await loadStatus();
    }

    async function saveSettings() {
      const data = new URLSearchParams();
      data.append('ssid', document.getElementById('wifiName').value);
      data.append('pass', document.getElementById('wifiPass').value);

      const res = await fetch('/api/settings?t=' + Date.now(), {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: data
      });

      alert(await res.text());
    }

    async function scanWifi() {
      const box = document.getElementById('scan');
      box.textContent = 'Scanning... wait a few seconds';

      try {
        const res = await fetch('/api/scan?t=' + Date.now());
        const list = await res.json();

        if (!list.length) {
          box.textContent = 'No networks found.';
          return;
        }

        box.innerHTML = list.map(w => `
          <div class="fileRow">
            <div>
              <div class="fileName">${escapeHtml(w.ssid || '(hidden network)')}</div>
              <div class="fileMeta">RSSI ${w.rssi} dBm · Channel ${w.channel} · ${w.secure ? 'Secured' : 'Open'}</div>
            </div>
          </div>
        `).join('');
      } catch (e) {
        box.textContent = 'Scan failed.';
      }
    }

    function reboot() {
      fetch('/api/reboot?t=' + Date.now());
      document.body.innerHTML =
        '<div style="padding:24px;font-family:system-ui;color:white;background:#111;min-height:100vh">' +
        '<h1>Rebooting...</h1><p>Reconnect to the ESP32 Wi-Fi if it disappears.</p></div>';
    }

    document.getElementById('morseText').addEventListener('input', updateMorsePreview);

    loadStatus();
    loadApps();
    updateMorsePreview();

    setInterval(loadStatus, 3000);
    setInterval(loadApps, 6000);
  </script>
</body>
</html>
)rawliteral";

const char PRESET_CALCULATOR_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Calculator</title>
  <style>
    * { box-sizing: border-box; }
    body { margin: 0; min-height: 100vh; background: #0d0d0f; color: white; font-family: system-ui; display: grid; place-items: center; padding: 18px; }
    .calc { width: min(100%, 380px); background: #1a1a1f; border: 1px solid #333; border-radius: 26px; padding: 18px; }
    h1 { margin: 0 0 12px; font-size: 22px; }
    .screen { background: #09090b; border: 1px solid #333; border-radius: 18px; padding: 16px; margin-bottom: 14px; min-height: 92px; display: flex; flex-direction: column; justify-content: space-between; overflow: hidden; }
    #expr { color: #aaa; font-size: 18px; overflow-wrap: anywhere; min-height: 24px; }
    #result { font-size: 36px; font-weight: 900; text-align: right; overflow-wrap: anywhere; }
    .keys { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; }
    button { border: 0; border-radius: 16px; padding: 18px 8px; font-size: 20px; font-weight: 900; background: #2c2c34; color: white; }
    .op { background: white; color: black; }
    .danger { background: #ff4d4d; color: black; }
    .equal { background: #7cff9b; color: black; }
    .wide { grid-column: span 2; }
    .hint { color: #777; font-size: 12px; text-align: center; margin-top: 14px; }
  </style>
</head>
<body>
  <div class="calc">
    <h1>Calculator</h1>
    <div class="screen"><div id="expr"></div><div id="result">0</div></div>
    <div class="keys">
      <button class="danger" onclick="clearAll()">AC</button><button onclick="backspace()">Back</button><button onclick="press('%')">%</button><button class="op" onclick="press('/')">/</button>
      <button onclick="press('7')">7</button><button onclick="press('8')">8</button><button onclick="press('9')">9</button><button class="op" onclick="press('*')">*</button>
      <button onclick="press('4')">4</button><button onclick="press('5')">5</button><button onclick="press('6')">6</button><button class="op" onclick="press('-')">-</button>
      <button onclick="press('1')">1</button><button onclick="press('2')">2</button><button onclick="press('3')">3</button><button class="op" onclick="press('+')">+</button>
      <button class="wide" onclick="press('0')">0</button><button onclick="press('.')">.</button><button class="equal" onclick="calculate()">=</button>
    </div>
    <div class="hint">Runs offline from ESP32-C3</div>
  </div>
  <script>
    let expr = "";
    function update() { document.getElementById("expr").textContent = expr; document.getElementById("result").textContent = expr || "0"; }
    function press(v) {
      const ops = "+-*/%";
      const last = expr.slice(-1);
      if (ops.includes(v) && (expr === "" || ops.includes(last))) return;
      expr += v; update();
    }
    function clearAll() { expr = ""; update(); }
    function backspace() { expr = expr.slice(0, -1); update(); }
    function calculate() {
      try {
        if (!expr) return;
        const safe = expr.replace(/[^0-9+\-*/%.()]/g, "");
        if (safe !== expr) throw new Error("bad input");
        let ans = Function('"use strict"; return (' + safe + ')')();
        if (!Number.isFinite(ans)) throw new Error("bad math");
        ans = Math.round(ans * 1000000000) / 1000000000;
        document.getElementById("expr").textContent = expr + " =";
        document.getElementById("result").textContent = ans;
        expr = String(ans);
      } catch { document.getElementById("result").textContent = "Error"; }
    }
    update();
  </script>
</body>
</html>
)rawliteral";

const char PRESET_NOTES_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Notes</title>
  <style>
    * { box-sizing: border-box; }
    body { margin:0; background:#111; color:white; font-family:system-ui; padding:18px; }
    .wrap { max-width:760px; margin:auto; }
    .card { background:#1a1a1f; border:1px solid #333; border-radius:24px; padding:18px; margin-bottom:12px; }
    h1 { margin-top:0; }
    input, textarea, select, button {
      width:100%; padding:13px; margin:6px 0; border-radius:14px; font:inherit;
    }
    input, textarea, select { background:#0b0b0d; color:white; border:1px solid #444; }
    textarea { min-height:260px; font-family:ui-monospace, monospace; }
    button { border:0; background:white; color:black; font-weight:900; }
    .danger { background:#ff4d4d; color:black; }
    .dark { background:#2c2c34; color:white; border:1px solid #444; }
    .grid { display:grid; grid-template-columns:1fr 1fr; gap:8px; }
    .note { padding:10px; border-bottom:1px solid #333; }
    .note:last-child { border-bottom:0; }
    .name { font-weight:900; }
    .meta { color:#888; font-size:13px; }
    .status { color:#aaa; font-size:13px; margin-top:8px; overflow-wrap:anywhere; }
  </style>
</head>
<body>
  <div class="wrap">
    <div class="card">
      <h1>Notes</h1>
      <p class="meta">Phone mode saves only on this browser. ESP32 mode saves shared notes into ESP32 storage.</p>

      <select id="mode" onchange="loadAll()">
        <option value="phone">Phone/browser storage</option>
        <option value="esp">ESP32 storage</option>
      </select>

      <input id="title" placeholder="Note title">
      <textarea id="body" placeholder="Write note..."></textarea>

      <div class="grid">
        <button onclick="saveNote()">Save Note</button>
        <button class="dark" onclick="newNote()">New</button>
      </div>
      <button class="danger" onclick="deleteNote()">Delete Current Note</button>

      <div class="status" id="status">Ready.</div>
    </div>

    <div class="card">
      <h2>Saved Notes</h2>
      <div id="list">Loading...</div>
    </div>
  </div>

  <script>
    const PHONE_KEY = "esp32_multi_notes_v1";
    const ESP_FILE = "notes-data.json";
    const MAX_BYTES = 16000;
    let notes = [];
    let currentId = null;

    function id() { return String(Date.now()) + "-" + Math.floor(Math.random() * 9999); }
    function status(s) { document.getElementById("status").textContent = s; }
    function bytes(s) { return new TextEncoder().encode(s).length; }
    function mode() { return document.getElementById("mode").value; }

    async function loadAll() {
      if (mode() === "phone") {
        try { notes = JSON.parse(localStorage.getItem(PHONE_KEY) || "[]"); }
        catch { notes = []; }
        status("Loaded phone notes.");
        render();
        return;
      }

      try {
        const res = await fetch("/app?name=" + encodeURIComponent(ESP_FILE) + "&t=" + Date.now());
        if (!res.ok) {
          notes = [];
          status("No ESP32 notes yet.");
        } else {
          notes = JSON.parse(await res.text() || "[]");
          status("Loaded ESP32 notes.");
        }
      } catch (e) {
        notes = [];
        status("Failed to load ESP32 notes: " + e);
      }

      render();
    }

    async function persist() {
      const data = JSON.stringify(notes);

      if (mode() === "phone") {
        localStorage.setItem(PHONE_KEY, data);
        return true;
      }

      if (bytes(data) > MAX_BYTES) {
        status("Too much note data for safe ESP32 save. Limit: " + MAX_BYTES + " bytes.");
        return false;
      }

      const form = new URLSearchParams();
      form.append("name", ESP_FILE);
      form.append("content", data);

      const res = await fetch("/api/save?t=" + Date.now(), {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: form
      });

      return res.ok;
    }

    function render() {
      const list = document.getElementById("list");

      if (!notes.length) {
        list.innerHTML = '<div class="meta">No notes yet.</div>';
        return;
      }

      list.innerHTML = notes.map(n => `
        <div class="note" onclick="openNote('${n.id}')">
          <div class="name">${escapeHtml(n.title || "Untitled")}</div>
          <div class="meta">${new Date(n.updated || n.created).toLocaleString()}</div>
        </div>
      `).join("");
    }

    function openNote(noteId) {
      const n = notes.find(x => x.id === noteId);
      if (!n) return;

      currentId = n.id;
      document.getElementById("title").value = n.title || "";
      document.getElementById("body").value = n.body || "";
      status("Opened note.");
    }

    async function saveNote() {
      const title = document.getElementById("title").value.trim() || "Untitled";
      const body = document.getElementById("body").value;

      if (!currentId) currentId = id();

      let n = notes.find(x => x.id === currentId);

      if (!n) {
        n = { id: currentId, created: Date.now() };
        notes.unshift(n);
      }

      n.title = title;
      n.body = body;
      n.updated = Date.now();

      const ok = await persist();

      if (ok) {
        status("Saved.");
        render();
      } else {
        status("Save failed.");
      }
    }

    function newNote() {
      currentId = null;
      document.getElementById("title").value = "";
      document.getElementById("body").value = "";
      status("New note.");
    }

    async function deleteNote() {
      if (!currentId) return;
      if (!confirm("Delete this note?")) return;

      notes = notes.filter(x => x.id !== currentId);
      currentId = null;
      document.getElementById("title").value = "";
      document.getElementById("body").value = "";

      await persist();
      render();
      status("Deleted.");
    }

    function escapeHtml(s) {
      return String(s).replace(/[&<>"']/g, c => ({
        "&":"&amp;", "<":"&lt;", ">":"&gt;", '"':"&quot;", "'":"&#039;"
      }[c]));
    }

    loadAll();
  </script>
</body>
</html>
)rawliteral";

const char PRESET_PIXEL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Pixel Pad</title>
  <style>
    body { margin:0; background:#111; color:white; font-family:system-ui; padding:18px; }
    .wrap { max-width:520px; margin:auto; }
    .card { background:#1a1a1f; border:1px solid #333; border-radius:24px; padding:18px; }
    h1 { margin-top:0; }
    canvas { width:100%; image-rendering:pixelated; background:white; border-radius:14px; touch-action:none; }
    input, button { width:100%; padding:13px; margin:6px 0; border-radius:14px; font:inherit; }
    input { border:1px solid #444; background:#0b0b0d; color:white; }
    button { border:0; background:white; color:black; font-weight:900; }
    .grid { display:grid; grid-template-columns:1fr 1fr; gap:8px; }
    .danger { background:#ff4d4d; color:black; }
  </style>
</head>
<body>
  <div class="wrap">
    <div class="card">
      <h1>Pixel Pad</h1>
      <canvas id="c" width="16" height="16"></canvas>
      <input id="color" type="color" value="#000000">
      <div class="grid">
        <button onclick="save()">Save</button>
        <button onclick="load()">Load</button>
      </div>
      <button class="danger" onclick="clearCanvas()">Clear</button>
    </div>
  </div>

  <script>
    const canvas = document.getElementById("c");
    const ctx = canvas.getContext("2d");
    const color = document.getElementById("color");
    let drawing = false;

    function init() {
      ctx.fillStyle = "#ffffff";
      ctx.fillRect(0, 0, 16, 16);
    }

    function point(e) {
      const r = canvas.getBoundingClientRect();
      const x = Math.floor((e.clientX - r.left) / r.width * 16);
      const y = Math.floor((e.clientY - r.top) / r.height * 16);
      return { x, y };
    }

    function draw(e) {
      const p = point(e);
      ctx.fillStyle = color.value;
      ctx.fillRect(p.x, p.y, 1, 1);
    }

    canvas.addEventListener("pointerdown", e => { drawing = true; draw(e); });
    canvas.addEventListener("pointermove", e => { if (drawing) draw(e); });
    canvas.addEventListener("pointerup", () => drawing = false);
    canvas.addEventListener("pointerleave", () => drawing = false);

    function save() {
      localStorage.setItem("esp32_pixel_pad", canvas.toDataURL());
      fetch("/api/blink?times=1&t=" + Date.now());
    }

    function load() {
      const data = localStorage.getItem("esp32_pixel_pad");
      if (!data) return;
      const img = new Image();
      img.onload = () => ctx.drawImage(img, 0, 0, 16, 16);
      img.src = data;
    }

    function clearCanvas() {
      if (!confirm("Clear drawing?")) return;
      init();
    }

    init();
    load();
  </script>
</body>
</html>
)rawliteral";

const char PRESET_DICE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Dice</title>
  <style>
    body { margin:0; min-height:100vh; background:#111; color:white; font-family:system-ui; display:grid; place-items:center; padding:20px; }
    .card { width:min(100%,420px); background:#1b1b20; border:1px solid #333; border-radius:24px; padding:22px; text-align:center; }
    .dice { font-size:90px; font-weight:900; margin:20px 0; }
    button, select { width:100%; padding:16px; border:0; border-radius:16px; font-size:18px; font-weight:900; margin-top:10px; }
    select { background:#111; color:white; border:1px solid #444; }
    button { background:white; color:black; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Dice</h1>
    <select id="sides">
      <option value="6">D6</option>
      <option value="10">D10</option>
      <option value="20">D20</option>
      <option value="100">D100</option>
    </select>
    <div class="dice" id="result">?</div>
    <button onclick="roll()">Roll</button>
    <button onclick="fetch('/api/blink?times=2&t='+Date.now())">Blink ESP LED</button>
  </div>
  <script>
    function roll() {
      const sides = Number(document.getElementById("sides").value);
      document.getElementById("result").textContent = Math.floor(Math.random() * sides) + 1;
    }
    roll();
  </script>
</body>
</html>
)rawliteral";

const char PRESET_TIMER_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Timer</title>
  <style>
    body { margin:0; min-height:100vh; background:#0d0d0f; color:white; font-family:system-ui; display:grid; place-items:center; padding:20px; }
    .card { width:min(100%,430px); background:#1a1a1f; border:1px solid #333; border-radius:24px; padding:22px; text-align:center; }
    .time { font-size:64px; font-weight:900; margin:20px 0; font-variant-numeric:tabular-nums; }
    input, button { width:100%; padding:15px; margin-top:10px; border-radius:16px; border:0; font-size:18px; font-weight:900; }
    input { background:#111; color:white; border:1px solid #444; }
    button { background:white; color:black; }
    .danger { background:#ff4d4d; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Timer</h1>
    <input id="minutes" type="number" value="1" min="0" placeholder="Minutes">
    <input id="seconds" type="number" value="0" min="0" max="59" placeholder="Seconds">
    <div class="time" id="display">01:00</div>
    <button onclick="startTimer()">Start</button>
    <button onclick="pauseTimer()">Pause</button>
    <button class="danger" onclick="resetTimer()">Reset</button>
  </div>
  <script>
    let left = 60, running = false, interval = null;
    function format(sec) {
      const m = Math.floor(sec / 60), s = sec % 60;
      return String(m).padStart(2, "0") + ":" + String(s).padStart(2, "0");
    }
    function update() { document.getElementById("display").textContent = format(left); }
    function readInputs() {
      const m = Number(document.getElementById("minutes").value || 0);
      const s = Number(document.getElementById("seconds").value || 0);
      left = Math.max(1, m * 60 + s);
      update();
    }
    function startTimer() {
      if (running) return;
      if (left <= 0) readInputs();
      running = true;
      interval = setInterval(() => {
        left--; update();
        if (left <= 0) {
          clearInterval(interval);
          running = false;
          fetch("/api/blink?times=10&t=" + Date.now());
        }
      }, 1000);
    }
    function pauseTimer() { running = false; clearInterval(interval); }
    function resetTimer() { pauseTimer(); readInputs(); }
    document.getElementById("minutes").addEventListener("input", readInputs);
    document.getElementById("seconds").addEventListener("input", readInputs);
    readInputs();
  </script>
</body>
</html>
)rawliteral";

const char PRESET_MORSE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Morse LED</title>
  <style>
    body { font-family:system-ui; background:#111; color:white; padding:24px; }
    input, select, button { width:100%; padding:14px; margin:8px 0; border-radius:14px; border:0; font:inherit; }
    button { font-weight:900; }
    pre { background:#222; padding:16px; border-radius:14px; white-space:pre-wrap; }
  </style>
</head>
<body>
  <h1>Morse LED</h1>
  <input id="text" value="SOS">
  <select id="unit">
    <option value="80">Fast</option>
    <option value="120" selected>Normal</option>
    <option value="200">Slow</option>
  </select>
  <button onclick="encode()">Preview</button>
  <button onclick="play()">Blink LED</button>
  <pre id="out">...</pre>
  <script>
    async function encode() {
      const text = document.getElementById("text").value;
      const res = await fetch("/api/morse/encode?text=" + encodeURIComponent(text) + "&t=" + Date.now());
      const d = await res.json();
      document.getElementById("out").textContent = d.morse || "(nothing)";
    }
    async function play() {
      const text = document.getElementById("text").value;
      const unit = document.getElementById("unit").value;
      document.getElementById("out").textContent = "Playing...";
      const res = await fetch("/api/morse/play?text=" + encodeURIComponent(text) + "&unit=" + unit + "&t=" + Date.now());
      const d = await res.json();
      document.getElementById("out").textContent = d.morse || "(nothing)";
    }
    encode();
  </script>
</body>
</html>
)rawliteral";

const char PRESET_CLOCK_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Clock</title>
  <style>
    body { font-family:system-ui; background:#111; color:white; padding:24px; text-align:center; }
    .clock { font-size:48px; font-weight:900; margin-top:40px; }
    .date { color:#aaa; font-size:18px; }
  </style>
</head>
<body>
  <h1>Clock</h1>
  <div class="clock" id="clock">--:--:--</div>
  <div class="date" id="date">...</div>
  <script>
    function tick() {
      const d = new Date();
      document.getElementById("clock").textContent = d.toLocaleTimeString();
      document.getElementById("date").textContent = d.toLocaleDateString();
    }
    tick();
    setInterval(tick, 1000);
  </script>
</body>
</html>
)rawliteral";

const char PRESET_TERMINAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>API Terminal</title>
  <style>
    body { font-family:system-ui; background:#111; color:white; padding:24px; }
    button { padding:14px; border-radius:14px; border:0; margin:5px; font-weight:900; }
    pre { background:#222; padding:16px; border-radius:14px; white-space:pre-wrap; overflow-wrap:anywhere; }
  </style>
</head>
<body>
  <h1>API Terminal</h1>
  <button onclick="call('/api/status')">Status</button>
  <button onclick="call('/api/led/toggle')">Toggle LED</button>
  <button onclick="call('/api/blink?times=3')">Blink</button>
  <button onclick="call('/api/scan')">Scan Wi-Fi</button>
  <pre id="out">Ready.</pre>
  <script>
    async function call(path) {
      document.getElementById("out").textContent = "Loading " + path + "...";
      try {
        const res = await fetch(path + (path.includes("?") ? "&" : "?") + "t=" + Date.now());
        const text = await res.text();
        try { document.getElementById("out").textContent = JSON.stringify(JSON.parse(text), null, 2); }
        catch { document.getElementById("out").textContent = text; }
      } catch (e) {
        document.getElementById("out").textContent = "Failed: " + e;
      }
    }
  </script>
</body>
</html>
)rawliteral";
