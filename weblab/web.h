#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-C3 Web Lab</title>
  <style>
    * {
      box-sizing: border-box;
    }

    body {
      margin: 0;
      font-family: system-ui, -apple-system, BlinkMacSystemFont, sans-serif;
      background: #0d0d0f;
      color: white;
      padding: 18px;
    }

    .wrap {
      max-width: 720px;
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
      font-weight: 800;
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

    .row:last-child {
      border-bottom: 0;
    }

    .key {
      color: #aaa;
    }

    .value {
      text-align: right;
      font-weight: 700;
      overflow-wrap: anywhere;
    }

    .buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
    }

    button {
      border: 0;
      border-radius: 16px;
      padding: 15px;
      font-size: 16px;
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

    .wifi {
      padding: 10px 0;
      border-bottom: 1px solid #2b2b31;
    }

    .wifi:last-child {
      border-bottom: 0;
    }

    .ssid {
      font-weight: 800;
      overflow-wrap: anywhere;
    }

    .meta {
      color: #aaa;
      font-size: 13px;
      margin-top: 3px;
    }

    code {
      background: #25252b;
      padding: 2px 6px;
      border-radius: 8px;
    }
  </style>
</head>
<body>
  <div class="wrap">
    <h1>ESP32-C3 Web Lab</h1>
    <div class="sub">Tiny offline admin panel running from the ESP32.</div>

    <div class="grid">
      <div class="card">
        <div class="title">Status</div>
        <div class="row"><div class="key">Board</div><div class="value" id="board">...</div></div>
        <div class="row"><div class="key">IP</div><div class="value" id="ip">...</div></div>
        <div class="row"><div class="key">Uptime</div><div class="value" id="uptime">...</div></div>
        <div class="row"><div class="key">Free heap</div><div class="value" id="heap">...</div></div>
        <div class="row"><div class="key">CPU</div><div class="value" id="cpu">...</div></div>
        <div class="row"><div class="key">Flash</div><div class="value" id="flash">...</div></div>
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
        <div class="title">Wi-Fi Scan</div>
        <div id="scan">Tap <code>Scan Wi-Fi</code> to scan nearby networks.</div>
      </div>

      <div class="card">
        <div class="title">API</div>
        <div class="row"><div class="key">Status</div><div class="value">/api/status</div></div>
        <div class="row"><div class="key">Scan</div><div class="value">/api/scan</div></div>
        <div class="row"><div class="key">LED</div><div class="value">/api/led/toggle</div></div>
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

    async function loadStatus() {
      try {
        const res = await fetch('/api/status');
        const d = await res.json();

        document.getElementById('board').textContent = d.board + ' / ' + d.chip_model;
        document.getElementById('ip').textContent = d.ip;
        document.getElementById('uptime').textContent = formatTime(d.uptime);
        document.getElementById('heap').textContent = formatBytes(d.free_heap);
        document.getElementById('cpu').textContent = d.cpu_mhz + ' MHz';
        document.getElementById('flash').textContent = formatBytes(d.flash_size);
        document.getElementById('led').textContent = d.led;
      } catch (e) {
        document.getElementById('board').textContent = 'Disconnected';
      }
    }

    async function api(path) {
      await fetch(path);
      await loadStatus();
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
          <div class="wifi">
            <div class="ssid">${escapeHtml(w.ssid || '(hidden network)')}</div>
            <div class="meta">RSSI ${w.rssi} dBm · Channel ${w.channel} · ${w.secure ? 'Secured' : 'Open'}</div>
          </div>
        `).join('');
      } catch (e) {
        box.textContent = 'Scan failed.';
      }
    }

    function reboot() {
      fetch('/api/reboot');
      document.body.innerHTML = '<h1>Rebooting...</h1><p>Reconnect to ESP32-C3-WebLab if Wi-Fi drops.</p>';
    }

    function escapeHtml(s) {
      return s.replace(/[&<>"']/g, c => ({
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#039;'
      }[c]));
    }

    loadStatus();
    setInterval(loadStatus, 3000);
  </script>
</body>
</html>
)rawliteral";
