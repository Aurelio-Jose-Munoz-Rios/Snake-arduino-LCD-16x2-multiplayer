# Multiplayer Snake Game on Arduino Uno

This project is a **custom implementation of the classic Snake game** on the Arduino Uno platform.  
It features **multiplayer support (2–3 players)**, **joystick controls**, **graphics rendered on a 16x2 LCD**, and a **non-blocking audio system** using a passive buzzer.  

---

## Features
- Multiplayer mode for 2–3 players.
- Snake movement, growth, and collision detection.
- LCD 16x2 screen used to render the game with custom characters.
- Joystick-based player control.
- Dynamic audio feedback for menu, gameplay, and Game Over.
- Non-blocking music system (based on `millis()`).

---

## Required Hardware

<img width="715" height="768" alt="esquematic" src="https://github.com/user-attachments/assets/cb20ac5a-8ab0-4ad0-a0a5-9487bb7a22bb" />

Try it online Wokwi Simulation: [Snake Game Simulation](https://wokwi.com/projects/442588059647901697)
## Watch demo on Youtube
[![Watch demo](https://img.youtube.com/vi/7ybJCi5B7xE/0.jpg)](https://www.youtube.com/watch?v=7ybJCi5B7xE)


- **Arduino Uno R3** (ATmega328P).
- **1x LCD 16x2** (HD44780 compatible, 4-bit mode).
- **Up to 3x Joystick modules** (KY-023 or similar).
- **1x Passive buzzer**.
- **1x 10k potentiometer** (for LCD contrast).
- Breadboard and jumper wires.

---

## Wiring Diagram (Connections)

### LCD 16x2 (in 4-bit mode)
| LCD Pin | Arduino Pin |
|---------|-------------|
| VSS     | GND         |
| VDD     | +5V         |
| V0      | Potentiometer middle pin |
| RS      | D12         |
| EN      | D11         |
| D4      | D5          |
| D5      | D4          |
| D6      | D3          |
| D7      | D2          |
| A (LED+) | +5V (with resistor ~220Ω) |
| K (LED-) | GND         |

### Joysticks
Each joystick uses **2 analog pins** (X and Y) and 1 digital pin (button, optional).

- **Joystick 1** → X: A5, Y: A4, Button: A3  
- **Joystick 2** → X: A2, Y: A1, Button: A0  
- **Joystick 3 (optional)** → Assign free analog pins  

### Buzzer
- Buzzer (+) → Arduino D8 (PWM capable pin recommended).  
- Buzzer (–) → GND.  

### Potentiometer
- One side → +5V  
- Other side → GND  
- Middle pin → LCD V0  

---

## Required Libraries
Make sure you install the following libraries in the Arduino IDE:  

- **LiquidCrystal** [LiquidCrystal by arduino](https://www.arduino.cc/en/Reference/LiquidCrystal)
  ```cpp
  #include <LiquidCrystal.h>

## Block Diagram
<img width="1757" height="815" alt="Tablero en blanco (4)" src="https://github.com/user-attachments/assets/32d987c4-f21e-4654-85ac-1f772efa3421" />



  
