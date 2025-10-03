// The IP address of your ESP32 (change to match your device)
var ESP32_IP = "http://172.20.10.12";

// These variables remember if things are ON or OFF
var isProbe1On = false;  // false means OFF
var isProbe2On = false;  // false means OFF

// These variables remember if we are showing Celsius or Fahrenheit
var isProbe1InCelsius = false;   // false means showing F by default
var isProbe2InCelsius = false;   // same here

// Get references to the HTML elements
// Sets the IDs to a variable
var probe1Display = document.getElementById("probe1Display");      // text for probe 1 temp
var probe2Display = document.getElementById("probe2Display");      // text for probe 2 temp
var probe1ToggleButton = document.getElementById("probe1ToggleButton"); // ON/OFF button for probe 1
var probe2ToggleButton = document.getElementById("probe2ToggleButton"); // ON/OFF button for probe 2
// This is the C/F button. It starts off by showing C because the starting unit is in F
var probe1UnitButton = document.getElementById("probe1UnitButton");   // °F/°C button for probe 1
// Same thing here
var probe2UnitButton = document.getElementById("probe2UnitButton");   // °F/°C button for probe 2

// const element = document.getElementById("id01");
// element.innerHTML = "New Heading";

// Store the last data we got from ESP32
var latestTemperatureData = null;

// ===============================
// Function: fetchTemperatureData
// Ask ESP32 for new temperature data
// ===============================
function fetchTemperatureData() {
    fetch(ESP32_IP + "/data")  // ask ESP32 at /data
        .then(function(response) {
            console.log("Turning reply into JSON");
            return response.json(); // turn reply into JSON
        })
        .then(function(data) {
            console.log("Getting data");
            // Save it so we can use it later
            latestTemperatureData = data;

            if (isProbe1On) {
                probe1Display.innerHTML = isProbe1InCelsius
                    ? data.probe1.C.toFixed(2) + " °C"
                    : data.probe1.F.toFixed(2) + " °F";
            } else {
                probe1Display.innerHTML = "--";
            }

            // Only show temperature if probe 2 is ON
            if (isProbe2On) {
                probe2Display.innerHTML = isProbe2InCelsius
                    ? data.probe2.C.toFixed(2) + " °C"
                    : data.probe2.F.toFixed(2) + " °F";
            } else {
                probe2Display.innerHTML = "--";
            }
        })
        .catch(function(error) {
            console.log("Error getting data:", error);
        });
}

// ===============================
// Function: toggleProbe1Power
// Turn GPIO 18 ON or OFF
// ===============================
function toggleProbe1Power() {
    // Decide what action to send to ESP32
    var action = isProbe1On ? "off" : "on";

    // Send command to ESP32
    fetch(ESP32_IP + "/18/" + action)
        .then(function() {
            // Flip the state after successful request
            isProbe1On = !isProbe1On;

            // Update button label
            probe1ToggleButton.innerHTML = isProbe1On ? "OFF" : "ON";

            // Update temperature display
            fetchTemperatureData();
        })
        .catch(function(error) {
            console.log("Error toggling Probe 1:", error);
        });
}

// ===============================
// Function: toggleProbe2Power
// Turn GPIO 19 ON or OFF
// ===============================
function toggleProbe2Power() {
    var action = isProbe2On ? "off" : "on";

    fetch(ESP32_IP + "/19/" + action)
        .then(function() {
            isProbe2On = !isProbe2On;
            probe2ToggleButton.innerHTML = isProbe2On ? "OFF" : "ON";
            fetchTemperatureData();
        })
        .catch(function(error) {
            console.log("Error toggling Probe 2:", error);
        });
}


// ===============================
// Function: toggleProbe1Unit
// Switch between F and C for probe 1
// ===============================
function toggleProbe1Unit() {
    // Flip the unit state
    isProbe1InCelsius = !isProbe1InCelsius;

    // Update the button label
    probe1UnitButton.innerHTML = isProbe1InCelsius ? "F" : "C";

    // Update the temperature display
    fetchTemperatureData();
}

// ===============================
// Function: toggleProbe2Unit
// Switch between F and C for probe 2
// ===============================
function toggleProbe2Unit() {
    // Flip the unit state
    isProbe2InCelsius = !isProbe2InCelsius;
    // update the button label
    probe2UnitButton.innerHTML = isProbe2InCelsius ? "F" : "C";
    // update the temperature sensor
    fetchTemperatureData();
}

// ===============================
// Hook up the buttons to functions
// ===============================
probe1ToggleButton.addEventListener("click", toggleProbe1Power);
probe2ToggleButton.addEventListener("click", toggleProbe2Power);
probe1UnitButton.addEventListener("click", toggleProbe1Unit);
probe2UnitButton.addEventListener("click", toggleProbe2Unit);

// ===============================
// Keep refreshing data every 1 sec
// ===============================
setInterval(fetchTemperatureData, 500);

// Is called immediately, run once at start
// So as soon as the page is open
fetchTemperatureData();

// ===============================
// Two-Probe Graph (Fixed Y-Axis + 300s Time Window + Missing Data)
// ===============================
const ctx = document.getElementById("temperatureChart").getContext("2d");

// Store timestamps & values
let timestamps = [];

const temperatureChart = new Chart(ctx, {
    type: "line",
    data: {
        labels: [], // seconds ago
        datasets: [
            {
                label: "Probe 1 Temperature (°C)",
                borderColor: "red",
                backgroundColor: "rgba(255,0,0,0.2)",
                data: [],
                spanGaps: false   // IMPORTANT: leave gaps when data is missing
            },
            {
                label: "Probe 2 Temperature (°C)",
                borderColor: "blue",
                backgroundColor: "rgba(0,0,255,0.2)",
                data: [],
                spanGaps: false   // don't connect missing points
            }
        ]
    },
    options: {
        responsive: true,
        animation: false,
        scales: {
            x: {
                title: { display: true, text: "Seconds Ago" },
                ticks: {
                    callback: function(value) {
                        return this.getLabelForValue(value) + "s";
                    }
                }
            },
            y: {
                title: { display: true, text: "Temperature (°C)" },
                min: 10,
                max: 50
            }
        }
    }
});

// Update chart with probe1 & probe2 values every 2s
async function updateChart() {
    const now = Date.now();
    let probe1Value = null;
    let probe2Value = null;

    try {
        const response = await fetch(ESP32_IP + "/data");
        const data = await response.json();

        // If probes are ON, record data, otherwise leave as null
        if (isProbe1On && data.probe1) {
            probe1Value = isProbe1InCelsius ? data.probe1.C : data.probe1.F;
            probe1Display.innerHTML = probe1Value.toFixed(2) + (isProbe1InCelsius ? " °C" : " °F");
        } else {
            probe1Display.innerHTML = "--";
        }

        if (isProbe2On && data.probe2) {
            probe2Value = isProbe2InCelsius ? data.probe2.C : data.probe2.F;
            probe2Display.innerHTML = probe2Value.toFixed(2) + (isProbe2InCelsius ? " °C" : " °F");
        } else {
            probe2Display.innerHTML = "--";
        }
    } catch (err) {
        console.error("Error fetching data:", err);
        // Keep nulls to represent missing data
        probe1Display.innerHTML = "--";
        probe2Display.innerHTML = "--";
    }

    // Save timestamp and push data (null allowed)
    timestamps.push(now);
    temperatureChart.data.datasets[0].data.push(probe1Value);
    temperatureChart.data.datasets[1].data.push(probe2Value);

    // Compute seconds ago
    const secondsAgo = timestamps.map(t => Math.round((now - t) / 1000));

    // Keep only last 300s of data
    while (secondsAgo.length > 0 && secondsAgo[0] > 300) {
        secondsAgo.shift();
        timestamps.shift();
        temperatureChart.data.datasets[0].data.shift();
        temperatureChart.data.datasets[1].data.shift();
    }

    // Update labels
    temperatureChart.data.labels = secondsAgo;

    // Update dataset labels with units
    temperatureChart.data.datasets[0].label =
        "Probe 1 Temperature (" + (isProbe1InCelsius ? "°C" : "°F") + ")";
    temperatureChart.data.datasets[1].label =
        "Probe 2 Temperature (" + (isProbe2InCelsius ? "°C" : "°F") + ")";

    // Adjust y-axis depending on unit
    if (isProbe1InCelsius || isProbe2InCelsius) {
        temperatureChart.options.scales.y.min = 10;
        temperatureChart.options.scales.y.max = 50;
        temperatureChart.options.scales.y.title.text = "Temperature (°C)";
    } else {
        temperatureChart.options.scales.y.min = 50;
        temperatureChart.options.scales.y.max = 122;
        temperatureChart.options.scales.y.title.text = "Temperature (°F)";
    }

    temperatureChart.update();
}

setInterval(updateChart, 1000);
