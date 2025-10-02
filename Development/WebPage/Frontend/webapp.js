// ===============================
// Beginner-Friendly JavaScript
// Talks to the ESP32 and updates webpage
// ===============================

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

            // Update probe 1
            // if (isProbe1InCelsius) {
            //     //probe1Display.innerHTML = "New Heading";
            //     probe1Display.innerHTML = data.probe1.C.toFixed(2) + " °C";
            // } else {
            //     probe1Display.innerHTML = data.probe1.F.toFixed(2) + " °F";
            // }
            //
            // // Update probe 2
            // if (isProbe2InCelsius ) {
            //     probe2Display.innerHTML = data.probe2.C.toFixed(2) + " °C";
            // } else {
            //     probe2Display.innerHTML = data.probe2.F.toFixed(2) + " °F";
            // }

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
    // Flip our memory of its state
    //isProbe1On =! isProbe1On;

    // Update button text
    // Shows the OFF option when the button is on
    // if (isProbe1On) {
    //     probe1ToggleButton.innerHTML = "OFF";
    // }
    // else {
    //     probe1ToggleButton.innerHTML = "ON";
    // }

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


    // ============================== ARDUINO BACKEND CODE =======================
    // var action;
    // if (gpio18On) {
    //     action = "off";   // if it was on, turn it off
    // } else {
    //     action = "on";    // if it was off, turn it on
    // }
    // fetch(ESP32_IP + "/18/" + action)
    //     .then(function() {
    //         console.log("GPIO 18 is now " + action);

    //         // Flip our memory of its state
    //         gpio18On = !gpio18On;

    //         // Update button text
    //         if (gpio18On) {
    //             probe1ToggleButton.innerHTML = "OFF";
    //         } else {
    //             probe1ToggleButton.innerHTML = "ON";
    //         }
    //     });
}

// ===============================
// Function: toggleProbe2Power
// Turn GPIO 19 ON or OFF
// ===============================
function toggleProbe2Power() {
    // Flip our memory of its state
    //isProbe2On =! isProbe2On;

    // Update button text
    // Shows the OFF option when the button is on
    // if (isProbe2On) {
    //     probe2ToggleButton.innerHTML = "OFF";
    //
    // }
    // // Else shows the ON option
    // else {
    //     probe2ToggleButton.innerHTML = "ON";
    // }

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



// ============================== ARDUINO BACKEND CODE =======================
//     var action;
//     if (gpio19On) {
//         action = "off";
//     } else {
//         action = "on";
//     }

//     fetch(ESP32_IP + "/19/" + action)
//         .then(function() {
//             console.log("GPIO 19 is now " + action);
//             gpio19On = !gpio19On;

//             if (gpio19On) {
//                 probe2ToggleButton.innerHTML = "OFF";
//             } else {
//                 probe2ToggleButton.innerHTML = "ON";
//             }
//         });
}

// ===============================
// Function: toggleProbe1Unit
// Switch between F and C for probe 1
// ===============================
function toggleProbe1Unit() {
    // Check the button text and set isProbe1InCelsius accordingly
    // if (probe1UnitButton.innerHTML === "C") {
    //     // This is a test statement
    //     // It switches the Unit from C to F when the F/C button is clicked
    //     //probe1Display.innerHTML = "9.00" + " °C";
    //
    //     isProbe1InCelsius = false; // If currently "C", switch to "F"
    //     probe1UnitButton.innerHTML = "F";
    // }
    // else if (probe1UnitButton.innerHTML === "F") {
    //     // This is a test statement
    //     // It switches the Unit from C to F when the F/C button is clicked
    //     //probe1Display.innerHTML = "48.20" + " °F";
    //
    //     isProbe1InCelsius = true; // If currently "F", switch to "C"
    //     probe1UnitButton.innerHTML = "C";
    // }

    // Flip the unit state
    isProbe1InCelsius = !isProbe1InCelsius;

    // Update the button label
    probe1UnitButton.innerHTML = isProbe1InCelsius ? "F" : "C";

    // Update the temperature display
    fetchTemperatureData();

    // ============================= ARDUINO BACKEND CODE =======================
    // // Update display right away using last data
    // if (lastData != null) {
    //     if (show18C) {
    //         probe1Display.innerHTML = lastData.probe1.C.toFixed(2) + " °C";
    //     } else {
    //         probe1Display.innerHTML = lastData.probe1.F.toFixed(2) + " °F";
    //     }
    // }
}

// ===============================
// Function: toggleProbe2Unit
// Switch between F and C for probe 2
// ===============================
function toggleProbe2Unit() {
    // When temperature is showing in °C,
    // the button should show “F” (because clicking it will switch to Fahrenheit)
    // if (probe2UnitButton.innerHTML === "C") {
    //     // This is a test statement
    //     // It switches the Unit from C to F when the F/C button is clicked
    //     //probe2Display.innerHTML = "18.00" + " °C";
    //
    //     isProbe2InCelsius = false; // If currently "C", switch to "F"
    //     probe2UnitButton.innerHTML = "F";
    // }
    // // When temperature is showing in °F,
    // // the button should show “C” (because clicking it will switch to Celsius).
    // else {
    //     // This is a test statement
    //     // It switches the Unit from C to F when the F/C button is clicked
    //     //probe2Display.innerHTML = "64.40" + " °F";
    //
    //     isProbe2InCelsius = true; // If currently "F", switch to "C"
    //     probe2UnitButton.innerHTML = "C";
    // }

    // Flip the unit state
    isProbe2InCelsius = !isProbe2InCelsius;
    // update the button label
    probe2UnitButton.innerHTML = isProbe2InCelsius ? "F" : "C";
    // update the temperature sensor
    fetchTemperatureData();

    // ============================= ARDUINO BACKEND CODE =======================
    // if (lastData != null) {
    //     if (show19C) {
    //         probe2Display.innerHTML = lastData.probe2.C.toFixed(2) + " °C";
    //     } else {
    //         probe2Display.innerHTML = lastData.probe2.F.toFixed(2) + " °F";
    //     }
    // }
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
