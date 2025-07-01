Simulate the hardware with help of LVGL

Overview
********

The digital twin aims to provide a near 1:1 functionality to the hardware.

The source code shows how to:

#. Get a pin specification from the :ref:`devicetree <dt-guide>`
#. Configure the GPIO pin as an input
#. Use keyboard as GPIO pin

.. _digital_twin-requirements:

Requirements
************

- Ubuntu 20.04 +
- WSL2 does not work, as the LVGL display goes black if its being updated in a thread
- Zephyr 4.0 or lower (for LVGL 8.4)
- Keyboard to control the pins
- Mouse to control the pins also works

Building and Running
********************

Build and run digital_twin as follows:

.. zephyr-app-commands::
   :zephyr-app: applications/StartWeekAvans25/native_sim
   :board: native_sim
   :goals: "./build/zephyr/zephyr.exe" or "west build -t run"
   :compact:

After running, a Zephyr display window pops up. 
If a runtime error occurs, the sample exits with printing to the console.

Build errors
************

See troubleshooting.md

Other errors
************

Also troubleshooting.md

Progress
************
ButtonMatrix: Done

Buzzers: Check possibilities for emulation

CircleMatrix: Done, compare with hardware

GenericGPIO: Done

GyroCompass: Not done, workaround should be possible with sensor_emul or manual function rewriting

LCD: Done, not the same as hardware (Hardware is limited to 32 characters)

LedMatrix: Done

Potmeter: Not done

SevenSegment: Done, no dot

Key differences with integrated version
************
The file structure is different across both versions.

This version has no support for building for hardware.

This version has a simpler functionality with less states, allowing easier testing of just the minigames.

This version has less issues with timers, which means it can play games like snake which dont work on the integrated version.