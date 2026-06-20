🚀 LUNAR-SCRUB: Deep Crater Astrobiology & Automated Sampling OS

LUNAR-SCRUB is a comprehensive, multi-subsystem firmware designed for an autonomous lunar regolith-cleaning and astrobiology sampling rover. Built for the Hack Club x NASA Stardance Challenge, this custom Arduino kernel manages 16 independent hardware subsystems concurrently to navigate harsh lunar terrains, clear dust hazards, and scan for water-ice signatures.

🌌 Core Features & Mission Subsystems

The operating system coordinates a complex hardware matrix divided into five specialized layers:
- **Subsystem A (Environmental Scanning):** Implements automated ultrasonic radar collision tracking and deep-terrain scouting arrays utilizing multi-point IR sensory data.
- **Subsystem B (6WD Mobility Matrix):** Drives 6 independent, high-torque wheels with a discrete direction/speed registry for advanced hazard evasion maneuvers.
- **Subsystem C (Surface Extraction & Maintenance):** Runs lower chassis sweeping brushes, dual high-velocity vacuum turbo fans, an air-blower jet for regolith clearing, and an automated solar panel cleaning mechanism.
- **Subsystem D (Regolith Storage):** Coordinates an automated mechanical storage tank dump system triggered by IR level indicators or wireless commands.
- **Subsystem E (Astrobiology Core Drill):** Evaluates real-time soil moisture and electrical conductivity to safely deploy a high-torque subsurface rock drill when water-ice boundaries are detected.

🛠️ Complete 16-Subsystem Hardware Register

| Motor/Device ID | Assigned Component Function |
| :--- | :--- |
| **Motors 1 - 3** | Left Side Drive Wheel Matrix (Front, Middle, Rear Channels) |
| **Motors 4 - 6** | Right Side Drive Wheel Matrix (Front, Middle, Rear Channels) |
| **Motor 7** | Solar Panel Top Cleaning Brush Actuator |
| **Motor 8 & 9** | Lower Chassis Road Sweeping Brushes (A & B Arrays) |
| **Motor 10 & 11** | Vacuum Suction Turbo Fan Assembly (Dual Extraction Lines) |
| **Motor 12** | High-Velocity Dust Dispersion Air Blower Jet |
| **Actuator 13** | Dirt Storage Tank Mechanical Trapdoor Servo |
| **Device 14** | 16x2 I2C LCD Character Diagnostic Telemetry Screen |
| **Device 15** | HC-05 / HC-06 High-Frequency Wireless Bluetooth Transceiver |
| **Sensors 16 - 18** | Advanced Scanning Array (Ultrasonic Radar, Moisture Probe, IR Tank Array) |

💻 Tech Stack & Architecture

- **Language:** C++ / Arduino Framework
- **Libraries:** Wire.h, LiquidCrystal_I2C.h
- **Design Pattern:** Monolithic Execution Kernel with fully decoupled, zero-grouping function blocks ensuring absolute asynchronous isolation between high-power motor lines and low-voltage I2C telemetry lines.
- **Development Tools:** Hackatime (Open-source development timer metrics), GitHub.

⚙️ How It Works (The Core Loop)

- **Sensing Pipeline:** The loop continuously calculates terrain distance via ultrasonic echo pulses and polls analog readings for tank capacities, soil water levels, and chassis clearances.
- **Safety Intercepts:** Immediate safety overrides trigger if the regolith tank is full or if a critical manual stop signal is beamed in through the Bluetooth terminal interface.
- **FSM Navigation Engine:**
  - *Distance > 50cm:* Enters Fast Cruise, activating extraction vacuums and traveling at peak power.
  - *Distance 25cm - 50cm:* Decelerates to Slow Prep crawl mode for delicate brushing maneuvers near craters.
  - *Distance < 25cm:* Halts all cleaning pumps, reverses, pivots right, and calculates a safe departure course.

🌟 Mission Metrics & Learning Objectives

Through building this firmware, I mastered managing complex pinout registers without line collisions, interacting with multiple I2C and analog protocols simultaneously, creating custom software-driven servo PWM pulses, and writing highly structured, non-blocking finite state machines for space automation.
