# Creeper Gadget

![Creeper][creeper.png]

A toy gadget that produces light + sound effects like a Creeper in Minecraft.
Fun little project which covers software, electronics, mechanical design elements.
And Mincraft. :)

## Hardware

* 35mm x 35mm 2-layer PCB w/dedicated ground plane
* Microcontroller: ATtiny85
* CR2450 battery
* Accelerometer: LIST2DH12TR (low power, motion detection interrupt)
* piezzo buzzer (SMT-1324-TW-1V-HT-R)
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

