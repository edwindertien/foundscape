/*
 * MIDI to RS485 dimmer board for Arduino Leonardo + RS485 adapter.
 *
 * Created: 4/6/2015 10:47:08 AM
 * Author: gurbrinder grewal
 * Modified by Arduino LLC (2015)
 * 
 * 
 * 
 */ 

#include "MIDIUSB.h"
#include "SoftwareSerial.h"
SoftwareSerial RS485(10,11);
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void setup() {
  Serial.begin(115200);
  RS485.begin(57600);
  pinMode(7,OUTPUT);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {

  if ((rx.byte1 & 0xF0)==0x90) DimmerBoardWrite(rx.byte1-0x90,rx.byte2,rx.byte3);
        
    }
  } while (rx.header != 0);
}

void DimmerBoardWrite(int board, int channel, int value)
{
  int checksum = ~( board + 0x04 + 0x03 + channel + value);
  unsigned char buffer[] = {
    (byte)0xFF, (byte)0xFF, // Header
    (byte)board, // Slave ID
    (byte)0x04, // data length (=number of parameter bytes + 2)
    (byte)0x03, // Write command
    (byte)channel, // Parameter 1
    (byte)value, // Parameter 2
    (byte)checksum};
    digitalWrite(7,HIGH); 
  RS485.write(buffer,8);
  RS485.flush();
  digitalWrite(7,LOW);  
}
