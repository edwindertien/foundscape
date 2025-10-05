Install the hardware (ottantotto) by copying the 'hardware' dir to arduino's sketchfolder
the boards mainly use ATmega88, programmed by an AVR-ISP (so no bootloader)

Alternatively an ATmega328p can be used, with the 'normal' bootloader for Arduino Uno. This means that for a first go still the AVR-ISP has to be used. Since there is no serial port on the dimmer, it does not make much sense using the bootloader anyway :)

The dimmerboard source can do soft-pwm on 8 outputs, as well as one-shot control (for valves / solenoids)

Four jumpers can set different aspects. The first two jumpers (closest to the output side) select the channel the boards respond to (so four channels can be selected). The third jumper switches between one-shot (time) and pwm mode (off: pwm, on: one-shot). the last (fourth) jumper, closest to the input side of the board, slects high (1000000) or low (115200) baudrate. off: 115200, on: 1000000

