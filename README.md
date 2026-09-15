# Model Rocket Location Beacon

Firmware for a location signalling beacon built for my university\u2019s rocketry team using an STM32G0.

> [!NOTE]
> This is the first version of a year-long project *(rocket will launch around July 2027)*.
> As such, I am still in the early stages and the code here is for prototyping and figuring out how best to use the microcontroller and peripherals together.

## Overview

The beacon is designed to report the rocket\u2019s location after landing so it can be recovered *(that is part of the competition criteria)*. As such the MVP:
- Main logic runs on an STM32G031K8, and I am making an active strive to replace all HAL code with register-level implementations *(currently blocking UART communications are fully replaced and interrupt/DMA-driven will be added if need arises)*.
- MCU spends most of its life in **SUSPEND** mode to conserve power, only waking up once every 60 seconds to acquire a location fix and transmit it.
- GNSS location fixes are acquired through a uBLOX Max-M10S module as it's designed for ultra-low-power applications.
- Location is transmitted wirelessly long-distance with the Ebyte E220-900T22D LoRa module.

## Roadmap

Currently, the prototype is purely on a breadboard and not a reliable, final product.

Here is what will be implemented and when:
1. **Early October 2026:** GNSS location is acquired and filtered by the MCU, transmitted to a computer over UART for testing.
2. **Mid October 2026:** MCU steps into low-power mode, only waking up every 60 seconds to acquire and transfer a new GNSS fix.
3. **Early November 2026:** LoRa module now transmits the location wirelessly every time a new fix is acquired.
4. **Early December 2026:** beacon is now battery-powered and capable of running for >24 hours undisturbed.
5. **January-February 2027:** custom 3D enclosures are 3D printed and version 1 is complete, ready for improvements towards version 2.
