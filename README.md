# 51Project: 8051 Microcontroller Experiments

## Project Overview

This repository contains a collection of projects and experiments developed for the 8051 microcontroller family (specifically targeting variants like AT89C52 and STC89C52RC). The projects cover various peripheral interactions and communication protocols commonly used in embedded systems development. They were developed using the Keil uVision IDE and the Keil C51 toolchain, utilizing both C and 8051 Assembly language.

See demo video of [humid+daytime](./media/实物演示：潮湿+白天.mp4), [humid+night](./media/实物演示：潮湿+夜晚.mp4), [dry+daytime](./media/实物演示：干燥+白天.mp4) and [dry+night](./media/实物演示：干燥+夜晚.mp4)

- [51Project: 8051 Microcontroller Experiments](#51project-8051-microcontroller-experiments)
  - [Project Overview](#project-overview)
  - [Directory Structure](#directory-structure)
  - [Modules / Features](#modules--features)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Building the Projects](#building-the-projects)
  - [Usage](#usage)
  - [Documentation](#documentation)

## Directory Structure

```
51project/
├── docs/              # Project planning and documentation files (.docx)
│   ├── 工程规划.docx
│   └── 最终产品功能.docx
├── media/             # Media files
└── src/               # Source code for different modules/experiments
    ├── 电机正反转及速度控制/  # Motor Control (DC & Stepper)
    │   ├── 步进电机/
    │   └── 直流电机/
    ├── 各种芯片的字位扩展/  # Bit/Word Expansion for Chips (Specifics unclear)
    ├── 蓝牙测试/          # Bluetooth Communication Test
    ├── 流水灯/            # LED Chaser Effects
    ├── 温度传感与多机通信/  # Temperature Sensing & Multi-device Communication
    │   ├── 多机通信/      # Master/Slave Communication Code
    │   └── 温度传感器/      # Temperature Sensor Code
    ├── 最终成果/          # Final Integrated Project
    └── LCD显示/           # LCD Display Control
```

## Modules / Features

1.  **Motor Control (`src/电机正反转及速度控制/`)**
    *   Includes code for controlling DC motors (forward, reverse, speed control likely via PWM) and stepper motors.
    *   Projects: `直流电机.uvproj`, `步进电机.uvproj` (inferred).

2.  **Bluetooth Test (`src/蓝牙测试/`)**
    *   Demonstrates serial communication using a Bluetooth module (likely HC-05/HC-06).
    *   Code receives data via UART and potentially controls hardware based on commands.
    *   Project: [`BLUETOOTH.uvproj`](51project/src/蓝牙测试/BLUETOOTH.uvproj)
    *   Source: [`BLUETOOTH.c`](51project/src/蓝牙测试/BLUETOOTH.c)

3.  **LED Chaser (`src/流水灯/`)**
    *   Contains various projects implementing different LED chasing patterns (e.g., sequential lighting, patterns).
    *   Projects: [`FLOW LIGHT.uvproj`](51project/src/流水灯/流水灯/FLOW%20LIGHT/FLOW%20LIGHT.uvproj), [`TEST FLOW LIGHT.uvproj`](51project/src/流水灯/流水灯/TEST%20FLOW%20LIGHT/TEST%20FLOW%20LIGHT.uvproj), [`test.uvproj`](51project/src/流水灯/流水灯/test/test.uvproj), [`TEST1.uvproj`](51project/src/流水灯/流水灯/TEST1/TEST1.uvproj)

4.  **Temperature Sensing & Multi-device Communication (`src/温度传感与多机通信/`)**
    *   **Temperature Sensor:** Code for interfacing with a temperature sensor (likely DHT11/22, based on typical 8051 projects).
    *   **Multi-device Communication:** Implements a master-slave communication system, likely using UART. Includes separate projects for the master and slave devices.
    *   Projects: `temperature sensing.uvproj` (inferred), [`multiplecomputer communication.uvproj`](51project/src/温度传感与多机通信/多机通信/multiplecomputer%20communication.uvproj) (Master), [`从机1.uvproj`](51project/src/温度传感与多机通信/多机通信/从机1.uvproj) (Slave 1), [`从机2.uvproj`](51project/src/温度传感与多机通信/多机通信/从机2.uvproj) (Slave 2), [`LIANTIAO.uvproj`](51project/src/温度传感与多机通信/多机通信/LIANTIAO.uvproj) (Combined Master/Slave test).

5.  **LCD Display (`src/LCD显示/`)**
    *   Code for interfacing with a character LCD module (e.g., 1602, 12864) to display text or data.
    *   Project: [`LCD.uvproj`](51project/src/LCD显示/LCD.uvproj)
    *   Source: [`LCD.a51`](51project/src/LCD显示/LCD.a51)

6.  **Final Project (`src/最终成果/`)**
    *   Integrates several functionalities, likely including temperature/humidity sensing, LCD display, and serial communication (possibly Bluetooth).
    *   Project: [`BEST.uvproj`](51project/src/最终成果/BEST.uvproj)
    *   Source: [`BEST.c`](51project/src/最终成果/BEST.c)

## Hardware Requirements

*   **Microcontroller:** 8051 compatible MCU (e.g., AT89C52, STC89C52RC)
*   **Peripherals (depending on the project):**
    *   LEDs
    *   LCD Module (Character LCD)
    *   Temperature and Humidity Sensor (e.g., DHT11/DHT22)
    *   Bluetooth Module (e.g., HC-05/HC-06)
    *   DC Motor + Driver
    *   Stepper Motor + Driver
    *   Basic components (resistors, capacitors, crystal oscillator)
    *   Programmer/Debugger for 8051

## Software Requirements

*   **IDE:** Keil uVision (Version 5.25 mentioned in logs)
*   **Toolchain:** Keil PK51 Professional Developer's Kit (C51 Compiler, A51 Assembler, BL51 Linker)

## Building the Projects

1.  Ensure you have Keil uVision and the C51 toolchain installed.
2.  Navigate to the specific project directory within `src/` (e.g., `src/蓝牙测试/`).
3.  Open the corresponding `.uvproj` file (e.g., `BLUETOOTH.uvproj`) in Keil uVision.
4.  Use the "Project" -> "Build Target" option (or press F7) to compile and link the code.
5.  The output HEX file will typically be generated in the `Objects/` subdirectory within the project folder (e.g., `src/蓝牙测试/Objects/BLUETOOTH.hex`).

## Usage

1.  Build the desired project as described above.
2.  Connect the 8051 microcontroller to your programmer.
3.  Flash the generated `.hex` file onto the microcontroller using appropriate programming software (e.g., STC-ISP, TL866 software).
4.  Connect the required hardware peripherals (LEDs, sensors, motors, LCD, etc.) to the correct pins on the microcontroller according to the source code definitions.
5.  Power on the circuit. The program should start executing.
6.  For projects involving serial communication (Bluetooth, Multi-device), use a serial terminal emulator (like PuTTY, Tera Term, or a mobile Bluetooth serial app) configured with the correct COM port and baud rate (check the source code, often 9600 bps for these projects) to interact with the device.

## Documentation

Refer to the documents in the [`docs/`](51project/docs/) directory for initial project planning and functional descriptions (in Chinese).
