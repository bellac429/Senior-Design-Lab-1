# Senior-Design-Lab-1
Digital Thermometer Project

## Questions for Professors
* After 300 seconds should the temp graph show 301+ history or delete oldest temp history?

## TO DO
* Make gant chart
* Read requirements and split up tasks
* Schematic?

## Needed Supplies
* LCD - Emma
* 2 Buttons - Emma
* Breadboard and wires - Bella
* Tape - probably Bella
* Stuff to make a thermometer box (3D print?)



---
---
---

### **1. Understand the Requirements (First Week)**

* Each team member should **read the lab document carefully** and flag unclear points (mechanical durability, web features, sensors, etc.).
* Schedule a team meeting with the TA or professor to **clarify ambiguities early**.
* Agree on the **minimum viable prototype (MVP)**:

  * Sensors read and display data locally.
  * Basic web interface shows temperature.

---

### **2. Divide Roles & Responsibilities**

Here’s one effective split for a 4-person team:

1. **Hardware Lead**

   * Select thermometer sensors and microcontroller.
   * Build/test sensor connections, buttons, power switch, connectors.
   * Ensure drop-test and unplug-replug robustness.

2. **Embedded Software Lead**

   * Program microcontroller to read sensors and drive local display.
   * Implement button functionality and error handling.
   * Provide clean data output for the web interface.

3. **Web/Interface Lead**

   * Build the PC/web app for real-time display, graphing, and unit conversion (C/F).
   * Implement notifications (text/email alerts).
   * Ensure error conditions show properly in the UI.

4. **Integration & Testing Lead (also Documentation)**

   * Make sure all subsystems integrate smoothly.
   * Run tests for timing (20 ms response, 1 Hz updates), range, robustness.
   * Lead formal documentation and design report writing.


### **3. Project Timeline**

* **Week 1–2**: Research parts, finalize design, assign tasks.
* **Week 3–4**:

  * Hardware: breadboard prototype of sensors + microcontroller.
  * Embedded: basic code for reading sensor + driving display.
  * Web: skeleton app that can show test data.
* **Week 5–6**:

  * Integrate microcontroller with PC/web.
  * Add graphing & button control.
  * Test robustness (unplugging sensors, drop tests).
* **Week 7–8**:

  * Implement text/email alerts.
  * Finalize enclosure design.
  * Documentation draft.
* **Final Week**: Debug, polish, practice presentation.

