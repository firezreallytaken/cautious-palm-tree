#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-C3 Web Lab v2</title>
  <style>
    * { box-sizing: border-box; }

    body {
      margin: 0;
      font-family: system-ui, -apple-system, BlinkMacSystemFont, sans-serif;
      background: #0d0d0f;
      color: white;
      padding: 18px;
    }

    .wrap {
      max-width: 760px;
      margin: 0 auto;
    }

    h1 {
      margin: 8px 0 4px;
      font-size: 30px;
    }

    .sub {
      color: #aaa;
      margin-bottom: 18px;
    }

    .grid {
      display: grid;
      gap: 12px;
    }

    .card {
      background: #1a1a1f;
      border: 1px solid #333;
      border-radius: 22px;
      padding: 16px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.25);
    }

    .title {
      font-weight: 900;
      margin-bottom: 10px;
      font-size: 18px;
    }

    .row {
      display: flex;
      justify-content: space-between;
      gap: 12px;
      padding: 8px 0;
      border-bottom: 1px solid #2b2b31;
    }

    .row:last-child { border-bottom: 0; }

    .key { color: #aaa; }

    .value {
      text-align: right;
      font-weight: 800;
      overflow-wrap: anywhere;
    }

    .buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
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

    button:active {
      transform: scale(0.97);
    }

    input, textarea {
      width: 100%;
      border: 1px solid #444;
      background: #111;
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

    .app {
      padding: 10px 0;
      border-bottom: 1px solid #2b2b31;
    }

    .app:last-child { border-bottom: 0; }

    .appname {
      font-weight: 900;
      overflow-wrap: anywhere;
    }

    .meta {
      color: #aaa;
      font-size: 13px;
      margin-top: 3px;
    }

    .appactions {
      display: flex;
      gap: 8px;
      margin-top: 8px;
    }

    .appactions a, .appactions button {
      flex: 1;
      text-align: center;
      text-decoration: none;
      border-radius: 12px;
      padding: 10px;
      font-size: 13px;
      font-weight: 900;
    }

    .appactions a {
      background: white;
      color: black;
    }

    code {
      background: #25252b;
      padding: 2px 6px;
      border-radius: 8px;
    }

    .notice {
      color: #aaa;
      font-size: 13px;
      line-height: 1.45;
    }
  </style>
</head>
<body>
  <div class="wrap">
    <h1>ESP32-C3 Web Lab v2</h1>
    <div class="sub">Tiny offline app launcher and control panel.</div>

    <div class="grid">
      <div class="card">
        <div class="title">Status</div>
        <div class="row"><div class="key">Board</div><div class="value" id="board">...</div></div>
        <div class="row"><div class="key">Wi-Fi Name</div><div class="value" id="ssid">...</div></div>
        <div class="row"><div class="key">IP</div><div class="value" id="ip">...</div></div>
        <div class="row"><div class="key">Uptime</div><div class="value" id="uptime">...</div></div>
        <div class="row"><div class="key">Free heap</div><div class="value" id="heap">...</div></div>
        <div class="row"><div class="key">Storage</div><div class="value" id="storage">...</div></div>
        <div class="row"><div class="key">LED</div><div class="value" id="led">...</div></div>
      </div>

      <div class="card">
        <div class="title">Controls</div>
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
        <div class="title">Mini Apps</div>
        <div class="notice">
          Upload a single-file HTML app, then open it from the ESP32 without rebuilding firmware.
        </div>
        <br>
        <input type="file" id="fileInput" accept=".html,.htm,.css,.js,.json,.txt">
        <button onclick="uploadFile()">Upload File</button>
        <div id="apps" style="margin-top: 12px;">Loading...</div>
      </div>

      <div class="card">
        <div class="title">Create Mini App</div>
        <input id="newName" value="hello.html">
        <textarea id="newContent"><!DOCTYPE html>
<html>
<head>
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
        <button onclick="saveMiniApp()">Save Mini App</button>
      </div>

      <div class="card">
        <div class="title">Change ESP32 Wi-Fi</div>
        <div class="notice">
          After saving, press reboot. Your phone will disconnect and you must connect to the new Wi-Fi name.
        </div>
        <br>
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
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#039;'
      }[c]));
    }

    async function loadStatus() {
      try {
        const res = await fetch('/api/status');
        const d = await res.json();

        document.getElementById('board').textContent = d.board + ' / ' + d.chip_model;
        document.getElementById('ssid').textContent = d.ssid;
        document.getElementById('ip').textContent = d.ip;
        document.getElementById('uptime').textContent = formatTime(d.uptime);
        document.getElementById('heap').textContent = formatBytes(d.free_heap);
        document.getElementById('storage').textContent =
          formatBytes(d.fs_used) + ' / ' + formatBytes(d.fs_total);
        document.getElementById('led').textContent = d.led;

        document.getElementById('wifiName').value = d.ssid;
      } catch (e) {
        document.getElementById('board').textContent = 'Disconnected';
      }
    }

    async function api(path) {
      await fetch(path);
      await loadStatus();
    }

    async function loadFiles() {
      const box = document.getElementById('apps');

      try {
        const res = await fetch('/api/files');
        const files = await res.json();

        if (!files.length) {
          box.innerHTML = '<div class="notice">No mini apps yet.</div>';
          return;
        }

        box.innerHTML = files.map(f => `
          <div class="app">
            <div class="appname">${escapeHtml(f.name)}</div>
            <div class="meta">${formatBytes(f.size)} · ${escapeHtml(f.path)}</div>
            <div class="appactions">
              <a href="${escapeHtml(f.url)}" target="_blank">Open</a>
              <button class="danger" onclick="deleteFile('${escapeHtml(f.path)}')">Delete</button>
            </div>
          </div>
        `).join('');
      } catch (e) {
        box.textContent = 'Failed to load files.';
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

      const res = await fetch('/api/upload', {
        method: 'POST',
        body: data
      });

      const text = await res.text();
      console.log(text);

      await loadFiles();
      await loadStatus();
    }

    async function saveMiniApp() {
      const name = document.getElementById('newName').value;
      const content = document.getElementById('newContent').value;

      const data = new FormData();
      data.append('name', name);
      data.append('content', content);

      const res = await fetch('/api/save', {
        method: 'POST',
        body: data
      });

      const d = await res.json();

      if (d.url) {
        alert('Saved. Open: ' + d.url);
      } else {
        alert('Save failed.');
      }

      await loadFiles();
      await loadStatus();
    }

    async function deleteFile(path) {
      if (!confirm('Delete ' + path + '?')) return;

      const res = await fetch('/api/delete?path=' + encodeURIComponent(path), {
        method: 'POST'
      });

      console.log(await res.text());

      await loadFiles();
      await loadStatus();
    }

    async function saveSettings() {
      const ssid = document.getElementById('wifiName').value;
      const pass = document.getElementById('wifiPass').value;

      const data = new FormData();
      data.append('ssid', ssid);
      data.append('pass', pass);

      const res = await fetch('/api/settings', {
        method: 'POST',
        body: data
      });

      const text = await res.text();
      alert(text);
    }

    async function scanWifi() {
      const box = document.getElementById('scan');
      box.textContent = 'Scanning... wait a few seconds';

      try {
        const res = await fetch('/api/scan');
        const list = await res.json();

        if (!list.length) {
          box.textContent = 'No networks found.';
          return;
        }

        box.innerHTML = list.map(w => `
          <div class="app">
            <div class="appname">${escapeHtml(w.ssid || '(hidden network)')}</div>
            <div class="meta">RSSI ${w.rssi} dBm · Channel ${w.channel} · ${w.secure ? 'Secured' : 'Open'}</div>
          </div>
        `).join('');
      } catch (e) {
        box.textContent = 'Scan failed.';
      }
    }

    function reboot() {
      fetch('/api/reboot');
      document.body.innerHTML =
        '<div style="padding:24px;font-family:system-ui;color:white;background:#111;min-height:100vh">' +
        '<h1>Rebooting...</h1>' +
        '<p>Reconnect to the ESP32 Wi-Fi if it disappears.</p>' +
        '</div>';
    }

    loadStatus();
    loadFiles();
    setInterval(loadStatus, 3000);
  </script>
</body>
</html>
)rawliteral";
