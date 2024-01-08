Certainly! Below is a README for your curtain automation project with detailed pin configurations:

---

# Curtain Automation Project README

## Overview

This project automates curtains using an ESP32 and an Arduino Nano. The ESP32 sends signals to the Arduino Nano, which controls the curtain motors based on feedback from limit switches. Additionally, the curtains can be remotely controlled via Google Assistant using IFTTT integration.

## Components

- **ESP32:** Sends control signals to the Arduino Nano.
- **Arduino Nano:** Controls the curtain motors based on signals received from the ESP32 and limit switch feedback.
- **Limit Switches:** Provide feedback to the Arduino Nano about the curtain position.
- **IFTTT:** Enables remote control of curtains via Google Assistant.

## Wiring

### Arduino Nano

- **Motor A:**
  - in1: Pin 12
  - in2: Pin 11

- **Motor B:**
  - in3: Pin 10
  - in4: Pin 9

- **Limit Switches:**
  - lo: Pin 2
  - lc: Pin 3
  - ro: Pin 4
  - rc: Pin 5

- **Input Pin:**
  - in: Pin 13

### ESP32

- **Output Pin:**
  - arduino: Pin 23

### Additional Notes

- Ensure proper power supply for the motors and components.

## Installation

1. **Arduino Nano:**
   - Upload the provided Arduino Nano code to the board.
   - Ensure correct wiring based on the pin configurations mentioned.

2. **ESP32:**
   - Upload the provided ESP32 code to the board.
   - Update Wi-Fi credentials and Adafruit.io details in the ESP32 code.

3. **IFTTT Integration:**
   - Create an account on IFTTT.
   - Set up an applet to link Google Assistant commands to ESP32 actions.

## Usage

1. **Local Control:**
   - Use the physical switches or buttons connected to the ESP32 to adjust the curtain position.
   - Ensure proper feedback from limit switches.

2. **Remote Control (IFTTT):**
   - Set up voice commands on Google Assistant through IFTTT.
   - Use voice commands to open or close the curtains remotely.

## Customization

- Adjust the Arduino Nano code for different curtain motor types or additional features.
- Modify the ESP32 code for specific communication protocols or integration with other smart home systems.

## Troubleshooting

- Check wiring, power supply, and communication between the ESP32 and Arduino Nano.
- Refer to the Arduino IDE serial monitor for debugging messages.

---

