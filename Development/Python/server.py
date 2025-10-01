# server.py
from flask import Flask, request, jsonify, render_template_string
import time

app = Flask(__name__)

# Store last readings
latest_data = {"probe1": {"C": None, "F": None},
               "probe2": {"C": None, "F": None},
               "timestamp": None}

# Store GPIO states
gpio_states = {"GPIO18": "OFF", "GPIO19": "OFF"}

# ------------------ API ------------------

@app.route("/api/data", methods=["POST"])
def receive_data():
    global latest_data
    data = request.json
    if not data:
        return jsonify({"error": "No JSON received"}), 400
    latest_data = data
    latest_data["timestamp"] = time.strftime("%Y-%m-%d %H:%M:%S")
    # return GPIO states so ESP32 knows what to do
    return jsonify(gpio_states)

@app.route("/api/latest")
def get_latest():
    return jsonify({"sensors": latest_data, "gpio": gpio_states})

@app.route("/api/control", methods=["POST"])
def control_gpio():
    global gpio_states
    data = request.json
    if not data:
        return jsonify({"error": "No JSON received"}), 400
    if "GPIO18" in data:
        gpio_states["GPIO18"] = data["GPIO18"]
    if "GPIO19" in data:
        gpio_states["GPIO19"] = data["GPIO19"]
    return jsonify({"status": "updated", "gpio": gpio_states})

# ------------------ Frontend ------------------

@app.route("/")
def index():
    page = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>ESP32 Temperature + GPIO Control</title>
        <meta charset="UTF-8">
        <style>
            body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
            h1 { color: #333; }
            .temp-box, .gpio-box { display: inline-block; margin: 20px; padding: 20px; border: 1px solid #ccc; border-radius: 10px; }
            .value { font-size: 1.5em; color: #007BFF; }
            button { padding: 10px 20px; margin: 5px; }
        </style>
    </head>
    <body>
        <h1>ESP32 Monitor + Control</h1>

        <div class="temp-box">
            <h2>Probe 1</h2>
            <div class="value" id="p1">-- °C / -- °F</div>
        </div>
        <div class="temp-box">
            <h2>Probe 2</h2>
            <div class="value" id="p2">-- °C / -- °F</div>
        </div>

        <p>Last update: <span id="time">--</span></p>

        <div class="gpio-box">
            <h2>GPIO Controls</h2>
            <div>
                GPIO 18: <span id="gpio18">OFF</span><br>
                <button onclick="toggle('GPIO18','ON')">ON</button>
                <button onclick="toggle('GPIO18','OFF')">OFF</button>
            </div>
            <div>
                GPIO 19: <span id="gpio19">OFF</span><br>
                <button onclick="toggle('GPIO19','ON')">ON</button>
                <button onclick="toggle('GPIO19','OFF')">OFF</button>
            </div>
        </div>

        <script>
            async function fetchData() {
                let res = await fetch("/api/latest");
                let data = await res.json();
                document.getElementById("p1").innerText = data.sensors.probe1.C + " °C / " + data.sensors.probe1.F + " °F";
                document.getElementById("p2").innerText = data.sensors.probe2.C + " °C / " + data.sensors.probe2.F + " °F";
                document.getElementById("time").innerText = data.sensors.timestamp;
                document.getElementById("gpio18").innerText = data.gpio.GPIO18;
                document.getElementById("gpio19").innerText = data.gpio.GPIO19;
            }

            async function toggle(pin, state) {
                await fetch("/api/control", {
                    method: "POST",
                    headers: {"Content-Type": "application/json"},
                    body: JSON.stringify({[pin]: state})
                });
                fetchData();
            }

            setInterval(fetchData, 2000);
            fetchData();
        </script>
    </body>
    </html>
    """
    return render_template_string(page)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
