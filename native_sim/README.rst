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
- WSL2 can also work 
- Zephyr 4.0 (for LVGL 8.4)
- Keyboard to control the pins

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
ButtonMatrix: Done, compare with hardware

Buzzers: Check possibilities for emulation

CircleMatrix: Done, compare with hardware

GenericGPIO: Done, compare with hardware

GyroCompass: Not done, workaround should be possible with sensor_emul or manual function rewriting

LCD: Done, compare with hardware

LedMatrix: Done, compare with hardware

Potmeter: Not done

SevenSegment: Done - Need to fix bug of LCD data appearing in this