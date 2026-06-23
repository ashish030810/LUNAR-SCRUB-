# LUNAR-SCRUB 🌙🤖
**An Autonomous Smart Lunar Dust Cleaner Rover**

---

## 🚀 Project Overview
Space dust on the Moon sticks to solar panels and blocks a rover's only power source. **LUNAR-SCRUB** solves this by using an automatic rotating brush system that sweeps dust off the panels to keep them clean. It features a 4WD motor setup to navigate terrain and a servo-controlled dust bin to collect and dump out the debris safely.

---

## 🛠️ Hardware Bill of Materials (BOM)

### 🧠 Core Electronics & Controllers
* **1x Arduino Uno** (Handles real-time sensor processing and safety overrides)
* **1x ESP32 Development Board** (Handles wireless communication and driving commands)

### ⚙️ Actuators & Drivetrain
* **1x 4WD Rover Chassis Kit** (Includes 4x DC motors and wheels)
* **1x L298N Motor Driver Module** (To power and control the 4WD motor setup)
* **1x High-Torque Servo Motor** (For controlling the dust bin collection/dumping mechanism)
* **1x DC Motor** (To spin the automatic rotating cleaning brush)

### 👁️ Sensors & Safety System
* **1x HC-SR04 Ultrasonic Distance Sensor** (For front-facing obstacle detection)
* **1x Infrared (IR) Obstacle Avoidance Sensor** (For ledge and crater detection)

### 🔋 Power & Interconnects
* **1x 7.4V / 11.1V Battery Pack** (To power the DC motors via the driver)
* **1x 5V/9V Power Source** (For clean logic power to the microcontrollers)
* **1x Breadboard & Premium Jumper Wires** (For circuit routing)

---

## 🧠 Software & Safety Logic
The system uses a dual-controller setup where the Arduino Uno constantly monitors safety sensors while the ESP32 handles driving inputs. 

* **Automatic Braking:** If the ultrasonic or IR sensors detect a rock or crater ledge closer than **15cm**, the code instantly overrides all driving inputs and hits the brakes completely to save the rover from crashing.
* **Easy Configuration:** Crucial settings like pin numbers, distance limits, and brush timers are declared as variables at the very top of the `main.ino` file for quick adjustments.

---

## 🕹️ How To Test It
1. Open the Arduino sketch (`main.ino`) and upload it to your board.
2. Open the **Serial Monitor**.
3. Type commands like `'F'` to drive forward, or `'M'` to trigger the solar panel brush.
4. Test the auto-braking system by placing an object closer than 15cm to the ultrasonic sensor—the motors will freeze instantly.
