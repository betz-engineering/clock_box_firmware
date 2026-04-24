# clock_box
The [clock_box](https://betz-engineering.ch/open_hardware/clock_box/) is a handy little clock generator for the RF and digital electronics lab. This repo contains the firmware source-code for the CH32V203 microcontroller.

# Task list
It's a work in progress.

  - [x] OLED display
  - [x] Find the right-sized font
  - [x] Encoder and rocker switch
  - [x] Changing digits with encoder and rocker
  - [x] Write, Read, Initialize LMX
  - [x] Add back the millis() function to enable long-push button events
  - [x] Frequency plan generation
  - [x] Flash storage of last frequency
  - [x] Power control
  - [ ] Show version on startup
  - [ ] USB serial device
  - [ ] SCPI command parser

# Building it
This project requires [platform.io](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html#installation-installer-script). Once installed, compile the firmware with

```bash
pio run
```

To put the clock_box in bootloader mode, keep the `BOOT0` button pushed while plugging in the USB cable. The power LED should turn on, the OLED should stay black and it should be possible to load the firmware with

```bash
pio run -t upload
```

For development work I was using a WCH-link E dongle connected to the debug header of the clock_box. See comments in platformio.ini on how to build a debug version with printf support.
