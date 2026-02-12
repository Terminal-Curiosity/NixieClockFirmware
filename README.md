# Nixie Clock — Firmware

Firmware for the ESP32-based control system driving a 4-digit Nixie tube clock.

This repository contains the embedded software responsible for timekeeping, display driving, brightness control, and peripheral coordination.

Hardware design files, enclosure work, and build documentation live in the main project repository.

---

## Scope of This Repository

This repo covers firmware only:

- System timekeeping via ESP32 time subsystem
- Nixie digit control
- I2C Port expander communication
- Status LED signaling
- Brightness sensing + scaling
- High-voltage enable control

For hardware schematics and mechanical design, see the parent project repository.

---

## Feature Overview

- System time derived from ESP32 `time()`
- BCD digit encoding for nixie drivers
- Dual PCF8574 I2C Port expanders to minimise ESP32 GPIO usage
- Simple Ambient-light auto-dimming
- Cathode poisoning prevention refresh - once daily
- RGB LED animations 
- Once-daily time correction using wifi NTP

---

## Firmware Architecture


## Execution Model

The firmware operates using a non-blocking main loop coordinated around system time. 
The loop cycles through performing core functions and each is time-gated according to the current time as measured with millis()

### Timekeeping

Clock time is derived from the ESP32’s built-in `time()` function.

---

### Display Refresh

Display updates occur when the reported second value changes.
A once-daily cathode conditioning routine runs from 3:30am - 3:45am to cycle through all cathodes at 1Hz.

---

### Main Loop Responsibilities

- Update displayed digits
- Sample ambient light
- Drive RGB decoration LEDs
- Request NTP time update if ready

All operations are non-blocking.

---

## Display Driving

Digits are encoded as 4-bit BCD values.

Signal path:

ESP32 → I²C → PCF8574 → K155ID1 → Nixie cathodes

Each digit update writes the corresponding decoder input value via the expanders.

Digits are mapped from software to hardware pins using a digit_to_pin_mapping[] look up array that has been configured based on this specific wiring.

---

## Brightness Control

Ambient light is measured via an LDR on an ADC input.

ADC readings are mapped into a brightness percentage range:

```
MIN_BRIGHTNESS_PERCENT → MAX_BRIGHTNESS_PERCENT
```

This value sets LED brightness high (100%) or low (20%) correspondingly.
This feature was added so that the LEDs will not be distracting in a dark room, for example the user's bedroom.

---

## High Voltage Control

The nixie HV supply is gated by a firmware-controlled enable pin.

This allows:

- Safe startup sequencing
- Tube blanking modes
- Future power-saving states

---

### GPIO Pin Configuration

| Pin | Function |
|-----|----------|
| 4   | SDA1 (I2C Bus 1) |
| 15  | SCL1 (I2C Bus 1) |
| 21  | SDA2 (I2C Bus 2) |
| 22  | SCL2 (I2C Bus 2) |
| 19  | INT2 (Interrupt, unused) |
| 32  | RGB LED (NeoPixel) |
| 23  | HV Enable (High Voltage supply) |
| 33  | LDR (Analog input) |


I2C bus 2 reserved but currently unused.

---

## Build Environment

PlatformIO is used for dependency management and compilation.

### Requirements

- PlatformIO CLI or VS Code extension
- Python 3.6+

---

### Build

```
pio run
```

### Upload

```
pio run -t upload
```

### Serial Monitor

```
pio device monitor
```

---

## Configuration

Primary firmware constants live in:

```
src/config.h
```

## Related Repositories

Hardware design, enclosure, and build documentation are maintained in the main Nixie Clock project repository.
