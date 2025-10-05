/*
   MIDI to RS485 dimmer board for Arduino Leonardo + RS485 adapter.

   MIDI USB Created: 4/6/2015 10:47:08 AM
   Author: gurbrinder grewal
   Modified by Arduino LLC (2015)

   Uses RS485 at 115200 (or any other set rate)
   Option to use Softwareserial or HW serial (arduino micro), check pindefs

   A program mode (activated by the sustain pedal) is used to send a 'program'
   message to a dimmer board to set its starting note. In total 4 MIDI channels are used in the system
   which can be selected on the dimmer board using dip switch

  


*/

#include "MIDIUSB.h"
//#define SOFTSERIAL (1)

#ifdef SOFTSERIAL
#include "SoftwareSerial.h"
SoftwareSerial RS485(10, 11); // RX, TX
#define RS485_SR 7
#else
#define RS485_SR 2
#endif

#define DEBUG (1)
int mode = 0;

#define PASS 0
#define PROGRAM 1

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
#ifdef SOFTSERIAL
  RS485.begin(115200);
#else
  Serial1.begin(115200);
#endif
  pinMode(RS485_SR, OUTPUT);
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
#ifdef DEBUG
      Serial.print(rx.byte1);
      Serial.print(',');
      Serial.print(rx.byte2);
      Serial.print(',');
      Serial.println(rx.byte3);
#endif
      if (mode == PASS) {
        if ((rx.byte1 & 0xF0) == 0x90) DimmerBoardWrite(rx.byte1 - 0x90, rx.byte2, constrain(2 * rx.byte3, 0, 255));
        if ((rx.byte1 & 0xF0) == 0x80) DimmerBoardWrite(rx.byte1 - 0x80, rx.byte2, 0);
        if ((rx.byte1 & 0xF0) == 0xB0) DimmerBoardWrite(rx.byte1 - 0xB0, rx.byte2, constrain(2 * rx.byte3, 0, 255));
      }
      if (mode == PROGRAM) {
        if ((rx.byte1 & 0xF0) == 0x90) ProgramMessage(rx.byte1 - 0x90, rx.byte2);
      }
      if ((rx.byte1 & 0xF0) == 176 && rx.byte2 == 64 && rx.byte3 == 127) {
        mode = PROGRAM;
        Serial.println("programming mode");
      }
      if ((rx.byte1 & 0xF0) == 176 && rx.byte2 == 64 && rx.byte3 == 0) {
        mode = PASS;
        Serial.println("passthrough mode");
      }
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
    (byte)checksum
  };

  digitalWrite(RS485_SR, HIGH);
#ifdef SOFTSERIAL
  RS485.write(buffer, 8);
  RS485.flush();
#else
  Serial1.write(buffer, 8);
  Serial1.flush();
#endif
  digitalWrite(RS485_SR, LOW);
}

void ProgramMessage(int board, int value)
{
  int checksum = ~( board + 0x04 + 0x06 + value);
  unsigned char buffer[] = {
    (byte)0xFF, (byte)0xFF, // Header
    (byte)board, // Slave ID
    (byte)0x04, // data length (=number of parameter bytes + 2)
    (byte)0x06, // Write command
    (byte)0x00,
    (byte)value, // Parameter 2
    (byte)checksum
  };
  digitalWrite(RS485_SR, HIGH);
#ifdef SOFTSERIAL
  RS485.write(buffer, 8);
  RS485.flush();
#else
  Serial1.write(buffer, 8);
  Serial1.flush();
#endif
  digitalWrite(RS485_SR, LOW);
}
