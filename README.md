# foundscape
midi control of things that tap and go boink. The basic structure consists of a midi-interface that takes data from Ableton (or similar) and sends events to 'dimmer cards' - being called like that because initially they mostly controlled LED light. 

Two behaviours exist on the main board right now which can be set using a small dip switch: either pulse-lenght or pwm duty cycle as third 'velocity' argument in a MIDI command. 

current version (still in operation) uses an ATmega328 (started out with the ATmega8 and then the ATmega88 in WinAVR. now it uses the Arduino environment). In the meantime a version has been made using Teensy, but has not been further pursued / produces (when Teensies were made of nonobtanium). On the wishlist a new version with pico

## modules
- [actuators](./actuators/) 3D printfiles for solenoid holders
- [atmegaDimmer](./atmegaDimmer/) first generation dimmer/control box, using RS485(dynamixel) protocol, using ATmega328
- [RS485adapter](./RS485adapter/) RS485 - midi interface using ATmega32U4 (Arduino micro, native USB)
- [teensyDimmer](./teensyDimmer/) update of dimmer/control box, using Teensy 4.0. build, not tested
- [picoDimmer](./picoDimmer/) new (to be designed) version of the dimmer/control box using RPI Pico 2040/2350
