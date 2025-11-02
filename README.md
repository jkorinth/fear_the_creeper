# Creeper Gadget

![Creeper][creeper.png]

A toy gadget that produces light + sound effects like a Creeper in Minecraft.
Fun little project which covers software, electronics, mechanical design elements.
And Mincraft. :)

## Hardware

* 35mm x 35mm 2-layer PCB w/dedicated ground plane
* Microcontroller: ATtiny85
* CR23xx battery
* Accelerometer: IIS328DQTR (low power, interrupt)
* piezzo buzzer
* 2x discrete LEDs (LP T67F)
* battery holder
* On/Off switch

## Mechanical

* 3D printed 40mm x 40mm x 10mm case
* cutout at Creeper "face" reveals black PCB underneath
* snap-fit lid for battery replacement
* hanger bracket for keyrings or lanyards

## Software

* IRQ-triggered sequence on motion detection
* fuse ignition sounds
* accelerating blinking
* explosion sound

## Tools

* Electronic Design: KiCAD
* Mechanical Design: FreeCAD
* Software Design: Arduino

## Current State ✅❌

### Summary
* i2c via I2CTinyBB: ✅
* irqs from IIS328DQ: ✅
* ISP: ✅
* LEDs: ✅
* KiBot CI/CD: kinda
* buzzer: ❌
* wake from sleep: ❌
* tilt switch: ❌

### Issues / Next Steps
Idea with the analog switch (74HC4066BQ) was nice, but it creates several new
problems: In closed state it has a small resistance (70R-180R), which creates
an accidental voltage divider with a pull-up on the line. Signal integrity was
good enough for i2c, strange enough, but both INT and BUZ suffered severe
issues when traversing the switch. The wake-up from deep sleep works fine when
bridging the `I2C` and `I2C_MCU` nets, but then ISP no longer works (which was
the whole point of having the analog switch in the first place).

The tilt switch has the issue that it generates only very short pulses, which
don't reliably wake up the MCU. Simplest solution would be a small capacitor
which slows down the rising edge, but I haven't tried that yet. Or a flip-flop
but that seems like total overkill.

This ISP issue is bugging me and I want to solve it. My next attempt would be
to use MOSFETs: 1x P-channel for the +3V pullups on SCL and SDA, 3x N-channel
for the SCL, SDA and INT signals. Costs more board space, but if I can use sth
like SOT-323, it may not be significant (also, the 74HC4066BQ footprint would
be gone, freeing up some little space).

Other issues:
  1. The power switch doesn't break the circuit from the debug connector.
  2. Add testpoints to make debugging less painful.
