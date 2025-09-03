# foundscape electronics

## anarchitechno box (formerly known as 'pico dimmer')

The boxes are still called 'dimmer' since all channels can either be switched (solenoids, pneumatic valves) where the control parameter is a pulse duration (so short sharp tap or long, plungy tap - and everything in between) or a PWM value (used typically for LED brightness or motor velocity),

During a session at Koppelting (Amersfoort) in August 2025 we found that a very interesting additiion in just 'dimming' channels is also PWM (frequency) control and AWG / synthesis to make actuators sound even more. During the session the requirements below have been updated and start has been made with a new (schematic) design. 

On the wishlist for the next design (using for example the atmega version or the teensy version as starting point)

### power
- multiple voltages for output, would be nice to change per channel, but for now typically 1 output voltage (i.e. 12V) per channel is fine. It would be nice when the system can work on 12V, but also as low as 5V and at least at 24V
- 8 channels, individual control mode per channel for one-shot (solenoids, timing), PWM (frequency, duty cycle) and AWG (saw/sine/triangle/block/noise, frequency, phase, amplitude)
- polyfuse per channel, Fuse indicator
- 1 LED per channel (indication)
- flyback diodes per channel
- TVS, power supply protection, main fuse

not yet implemented but nice to have
- power monitoring (voltage / current)
- reverse voltage protection

For now the output section consisting of IRFZ44 and TC4423 has been working ok. Might be room to revise for newer, cheaper and SMD version. See if gate driver is necessary, but might be for higher switching frequencies. I checked other gate drivers, so far the 4427 seems a good (SMD) follow up with a good balance in price, performance and size

Right now I tend to put in a Traco power TC2450 or similar to do the 12-24V to 5V conversion. Might be up for a change too. 

### galvanic separation
- using isolators it is important to separate the high/power (24V) side from the logic side. This means digital isolators as well as an isiolated 5V->5.5V supply. The USB Host needs 5V, the pico can do 5V->3.3V with its on-board converter

### brains
- pico (2W) - might be the standard board, or just the RP2040 or RP2350 chip + necessary peripherals
- OLED (small 32x128 pix) is very useful for menu, status info etc
- incremental encoder (3 switches) - pio code works fine. good for changing menu options
- simple basic LEDs on power and communication lines (so you can always check com, also when you are fiddeling with a menu), also from a distance. 
- WS8211 RGB leds (string), 1 per channel (channel mode selection) and communication status (per socket / unit)

### connection
- USB (midi, serial)  -> existing USB micro
- USB Host (midi, gamepad, HID) -> add USB A socket. Think about necessary series resistors and power components. 
- RS485 (DMX: neutrix 3/5 pin, 'dynamixel' bus) -> 3way(A, B, GND) and 4way (A, B, Power, GND) jst socket. For (my) RJ45 and a neutrik socket we can make an adapter? Or have the RJ45 system since it is so easy to stack through.. 
- ethernet: WSD5500 + socket + mag
- MIDI: old fashioned 5-pin DIN midi (CNY138 optocoupler) (can also be adapter cable from 5-way jst socket)
- A pico could in theory also do everything WIFI / Bluetooth / BLE wise. .. 
- HW sync input (optocoupler?)