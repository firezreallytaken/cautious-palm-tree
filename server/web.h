#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-C3</title>
  <style>
    body {
      margin: 0;
      font-family: system-ui, sans-serif;
      background: #111;
      color: white;
      display: grid;
      place-items: center;
      min-height: 100vh;
    }

    .card {
      width: min(90vw, 420px);
      background: #222;
      border-radius: 24px;
      padding: 24px;
      text-align: center;
    }

    button {
      width: 100%;
      padding: 18px;
      border: 0;
      border-radius: 16px;
      font-size: 20px;
      font-weight: 800;
    }

    #status {
      font-size: 22px;
      margin: 20px;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>ESP32-C3 Server</h1>
    <p>This HTML is stored in a separate file.</p>

    <div id="status">LED: unknown</div>
    <button onclick="toggleLED()">Toggle LED</button>
  </div>

  <script>
    async function toggleLED() {
      const res = await fetch('/toggle');
      const text = await res.text();
      document.getElementById('status').textContent = 'LED: ' + text;
    }
  </script>
</body>
</html>
)rawliteral";
