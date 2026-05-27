# Low-Power Environmental & Power Monitor Node

### STM32F303K8 | ARM Cortex-M4 Bare-Metal Embedded System

## Project Description

The objective of this project is the design and implementation of a low-power industrial embedded monitoring node using the STM32 Nucleo-32 development board featuring the STM32F303K8 ARM Cortex-M4 microcontroller.

The system operates as an autonomous environmental and power monitoring node capable of:

* monitoring environmental sensor data
* detecting vibration and motion events
* supervising power supply voltage levels
* performing low-power autonomous operation
* executing fault-tolerant embedded firmware

The architecture is implemented entirely using bare-metal programming techniques without relying on HAL libraries or external RTOS frameworks.

---

# System Features

## Environmental & Motion Monitoring

The node interfaces with an MPU9250 9-axis motion sensor through I2C/SPI communication protocols in order to measure:

* acceleration
* angular velocity
* vibration activity
* motion-triggered wake-up events

The MPU9250 is also configured to generate asynchronous Wake-on-Motion interrupts whenever abrupt movement or vibration is detected.

---

# Power Monitoring

The system continuously supervises its own power supply voltage using the internal ADC peripheral of the STM32F303K8.

A voltage divider network is used to safely scale the input voltage into the ADC operating range.

ADC sampling operations are executed using DMA transfers to eliminate CPU overhead during data acquisition.

---

# Low-Power Embedded Architecture

A major focus of the project is aggressive low-power optimization.

The Cortex-M4 MCU periodically enters:

* Sleep Mode
* Stop Mode

in order to minimize energy consumption.

The microcontroller wakes up through two independent mechanisms:

## Periodic Wake-Up

Using:

* RTC Wakeup Timer

the MCU wakes every 5 seconds to:

* acquire sensor measurements
* sample power data
* transmit telemetry
* refresh the watchdog

## Asynchronous Wake-Up

Using:

* external interrupt events from the MPU9250

the MCU immediately exits low-power mode whenever sudden vibration or motion is detected.

This architecture simulates real industrial low-power sensor nodes commonly used in:

* industrial monitoring systems
* predictive maintenance systems
* remote telemetry nodes
* battery-powered IoT devices

---

# DMA-Based Zero-CPU Data Transfers

To minimize processor utilization and improve system efficiency, all major data transfers are implemented using DMA (Direct Memory Access).

DMA is used for:

* ADC sampling
* UART transmission
* UART reception
* sensor data acquisition

This allows peripherals to exchange data directly with memory without continuous CPU intervention.

---

# Non-Blocking UART Architecture

The serial communication subsystem is implemented using:

* interrupt-driven UART
* circular ring buffers
* non-blocking transmission and reception

The firmware supports asynchronous communication without blocking the scheduler or application tasks.

Features include:

* RX interrupts
* ring-buffered UART queues
* interrupt-safe data handling
* zero polling overhead
* continuous streaming operation

---

# Embedded Scheduler Architecture

The firmware includes a lightweight custom embedded scheduler implemented entirely from scratch without using FreeRTOS or external kernels.

The scheduler architecture is based on:

* SysTick timer interrupts
* PendSV context switching
* PSP/MSP stack separation
* software task scheduling

The implementation demonstrates internal RTOS kernel mechanisms commonly found in commercial embedded systems.

---

# Scheduler Features

The scheduler supports:

* task scheduling
* task switching
* task delays
* task blocking
* ready/blocked task states
* periodic task execution
* context switching
* stack frame management
* software-managed PSP stacks

---

# Cortex-M4 Exception Usage

The firmware directly utilizes Cortex-M4 exception mechanisms including:

* SysTick
* PendSV
* HardFault
* MemManage
* BusFault

The project manually manages:

* interrupt vector tables
* exception handlers
* stack initialization
* processor fault handling

without middleware abstraction layers.

---

# Fault-Tolerant Firmware

System reliability is improved through the use of:

* Independent Watchdog Timer (IWDG)

The watchdog supervises firmware execution and automatically resets the MCU in case of:

* deadlocks
* task starvation
* software lockups
* unexpected runtime faults

This increases the robustness and fault tolerance of the embedded system.

---

# Bare-Metal Embedded Concepts Implemented

The project demonstrates advanced low-level embedded engineering concepts including:

* ARM Cortex-M4 architecture
* bare-metal firmware development
* startup code implementation
* linker script memory mapping
* stack pointer initialization
* MSP/PSP management
* context switching
* interrupt handling
* DMA architecture
* low-power modes
* ADC configuration
* UART driver implementation
* I2C/SPI communication
* watchdog supervision
* fault exception handling
* ring buffer design
* memory-mapped peripheral access
* register-level programming

---

# Development Environment

## Hardware

* STM32 Nucleo-32
* STM32F303K8 MCU
* MPU9250 Motion Sensor

## Toolchain

* arm-none-eabi-gcc
* OpenOCD
* STM32CubeIDE
* GNU Make
* PuTTY / Serial Terminal

---

# Project Goals

This project is intended to simulate the architecture of real industrial low-power embedded systems while demonstrating strong understanding of:

* embedded operating principles
* low-level ARM Cortex-M4 internals
* RTOS kernel mechanisms
* interrupt-driven firmware
* power-aware embedded design
* industrial firmware reliability techniques

The implementation emphasizes software architecture quality, low-level debugging skills, and deep understanding of embedded system internals beyond standard HAL-based application development.
