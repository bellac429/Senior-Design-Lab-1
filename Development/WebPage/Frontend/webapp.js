// ===============================
// Beginner-Friendly JavaScript
// Talks to the ESP32 and updates webpage
// ===============================

// The IP address of your ESP32 (change to match your device)
var ESP32_IP = "http://172.20.10.12";

// These variables remember if things are ON or OFF
var gpio18On = false;  // false means OFF
var gpio19On = false;  // false means OFF

// These variables remember if we are showing Celsius or Fahrenheit
var show18C = false;   // false means showing F by default
var show19C = false;   // same here

// Get references to the HTML elements
// Sets the IDs to a variable
var probe1El = document.getElementById("probe1");      // text for probe 1 temp
var probe2El = document.getElementById("probe2");      // text for probe 2 temp
var btn18OFFON = document.getElementById("btn18OFFON"); // ON/OFF button for probe 1
var btn19OFFON = document.getElementById("btn19OFFON"); // ON/OFF button for probe 2
// This is the C/F button. It starts off by showing C because the starting unit is in F
var btn18Temp = document.getElementById("btn18Temp");   // °F/°C button for probe 1
// Same thing here
var btn19Temp = document.getElementById("btn19Temp");   // °F/°C button for probe 2

// const element = document.getElementById("id01");
// element.innerHTML = "New Heading";

// Store the last data we got from ESP32
var lastData = null;

// ===============================
// Function: updateData
// Ask ESP32 for new temperature data
// ===============================
function updateData() {
    fetch(ESP32_IP + "/data")  // ask ESP32 at /data
        .then(function(response) {
            console.log("Turning reply into JSON");
            return response.json(); // turn reply into JSON
        })
        .then(function(data) {
            console.log("Getting data");
            // Save it so we can use it later
            lastData = data;

            // Update probe 1
            if (show18C) {
                //probe1El.innerHTML = "New Heading";
                probe1El.innerHTML = data.probe1.C.toFixed(2) + " °C";
            } else {
                probe1El.innerHTML = data.probe1.F.toFixed(2) + " °F";
            }

            // Update probe 2
            if (show19C) {
                probe2El.innerHTML = data.probe2.C.toFixed(2) + " °C";
            } else {
                probe2El.innerHTML = data.probe2.F.toFixed(2) + " °F";
            }
        })
        .catch(function(error) {
            console.log("Error getting data:", error);
        });
}

// ===============================
// Function: toggle18
// Turn GPIO 18 ON or OFF
// ===============================
function toggle18() {
    // Flip our memory of its state
    gpio18On = !gpio18On;

    // Update button text
    // Shows the OFF option when the button is on
    if (gpio18On) {
        btn18OFFON.innerHTML = "OFF";
    }
    else {
        btn18OFFON.innerHTML = "ON";
    }

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
    //             btn18OFFON.innerHTML = "OFF";
    //         } else {
    //             btn18OFFON.innerHTML = "ON";
    //         }
    //     });
}

// ===============================
// Function: toggle19
// Turn GPIO 19 ON or OFF
// ===============================
function toggle19() {
    // Flip our memory of its state
    gpio19On = !gpio19On;

    // Update button text
    // Shows the OFF option when the button is on
    if (gpio19On) {

        btn19OFFON.innerHTML = "OFF";
    }
    // Else shows the ON option
    else {
        btn19OFFON.innerHTML = "ON";
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
//                 btn19OFFON.innerHTML = "OFF";
//             } else {
//                 btn19OFFON.innerHTML = "ON";
//             }
//         });
}

// ===============================
// Function: toggle18Temp
// Switch between F and C for probe 1
// ===============================
function toggle18Temp() {
    // Check the button text and set show18C accordingly
    if (btn18Temp.innerHTML === "C") {
        // This is a test statement
        // It switches the Unit from C to F when the F/C button is clicked
        //probe1El.innerHTML = "9.00" + " °C";

        show18C = false; // If currently "C", switch to "F"
        btn18Temp.innerHTML = "F";
    }
    else {
        // This is a test statement
        // It switches the Unit from C to F when the F/C button is clicked
        //probe1El.innerHTML = "48.20" + " °F";

        show18C = true; // If currently "F", switch to "C"
        btn18Temp.innerHTML = "C";
    }

    // ============================= ARDUINO BACKEND CODE =======================
    // // Update display right away using last data
    // if (lastData != null) {
    //     if (show18C) {
    //         probe1El.innerHTML = lastData.probe1.C.toFixed(2) + " °C";
    //     } else {
    //         probe1El.innerHTML = lastData.probe1.F.toFixed(2) + " °F";
    //     }
    // }
}

// ===============================
// Function: toggle19Temp
// Switch between F and C for probe 2
// ===============================
function toggle19Temp() {
    // When temperature is showing in °C,
    // the button should show “F” (because clicking it will switch to Fahrenheit)
    if (btn19Temp.innerHTML === "C") {
        // This is a test statement
        // It switches the Unit from C to F when the F/C button is clicked
        //probe2El.innerHTML = "18.00" + " °C";

        show19C = false; // If currently "C", switch to "F"
        btn19Temp.innerHTML = "F";
    }
    // When temperature is showing in °F,
    // the button should show “C” (because clicking it will switch to Celsius).
    else {
        // This is a test statement
        // It switches the Unit from C to F when the F/C button is clicked
        //probe2El.innerHTML = "64.40" + " °F";

        show19C = true; // If currently "F", switch to "C"
        btn19Temp.innerHTML = "C";
    }

    // ============================= ARDUINO BACKEND CODE =======================
    // if (lastData != null) {
    //     if (show19C) {
    //         probe2El.innerHTML = lastData.probe2.C.toFixed(2) + " °C";
    //     } else {
    //         probe2El.innerHTML = lastData.probe2.F.toFixed(2) + " °F";
    //     }
    // }
}

// ===============================
// Hook up the buttons to functions
// ===============================
btn18OFFON.addEventListener("click", toggle18);
btn19OFFON.addEventListener("click", toggle19);
btn18Temp.addEventListener("click", toggle18Temp);
btn19Temp.addEventListener("click", toggle19Temp);

// ===============================
// Keep refreshing data every 2 sec
// ===============================
setInterval(updateData, 1000);

// Run once at start
updateData();
