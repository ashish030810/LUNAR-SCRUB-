# LUNAR-SCRUB
Autonomous solar panel dust cleaner — built for the moon, tested on my desk

## Why I built this

Moon dust is a real problem. Rovers on the lunar surface lose power because dust keeps piling up on their solar panels — and there's nobody up there to wipe it off. I wanted to build a prototype that solves exactly that: a rover that cleans itself so it never has to stop.

LUNAR-SCRUB is my take on an autonomous dust cleaner. It drives around, spins a brush over the solar panels, collects the debris in a bin, and dumps it out — all on its own. No human input needed once it starts.

## What it does

The rover runs on a 4WD chassis and uses two brains:
- **Arduino Uno** — watches the sensors. If something's too close (rock, crater edge, whatever), it kills the motors instantly.
- **ESP32** — handles wireless commands and drives the rover around.

The cleaning system is a DC motor spinning a brush, and a servo that tilts the dust bin to dump the debris out when it's full.

## Safety first

The ultrasonic sensor and IR sensor together make sure the rover doesn't roll off a ledge or crash into a rock. If anything gets within 15cm — full stop, instantly. It overrides every other command. The moon doesn't have guardrails so neither does this.

## Hardware

- Arduino Uno + ESP32
- 4WD rover chassis with L298N motor driver
- High-torque servo (dust bin) + DC motor (brush)
- HC-SR04 ultrasonic + IR obstacle sensor
- 12V battery pack
This is a prototype. The goal is to eventually make something that could realistically keep a solar panel clean on the lunar surface without any human help.

  
