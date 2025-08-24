// this controlbox can use multiple radio systems.
// on th RF spot there is place for an
// APC220. using RX and Tx
// link RF-set to gnd and write 'RD' or 'WR'
// with a specific configuration string
// for BetaFPV/ELRS/CrossFire you can send and
// receive a CRSF string. make sure remote and receiver
// are paired (see betafpv manual), flashed with 
// firmware v3.4.3 or up (same for Tx and Tx). Set 
// packet rate to 333 (or100), switches to 16/2 ch and telemetry to 1:128
// 
// TODO: for the animaltroniek the air or driving edition have not been 
// added yet.
//
// resources:
// https://arduino-pico.readthedocs.io/en/latest/index.html
// edit the config.h to set the specifics for a used robot or vehicle
#include <Arduino.h> // the EarlPhilHower Arduino port of Pico SDK
#include <Wire.h>    // the I2C communication lib for the display, PCA9685 etc
#include <config.h>  // the specifics for the controlled robot or vehicle
#include <hardware/watchdog.h>
// hardware on every board: the relay sockets
#include <PicoRelay.h>
PicoRelay relay;
// Feed MIDI activity
uint8_t levels[8];

/// MIDI functions (USB MIDI, see Adafruit config in platformio.ini)
#include <MIDI.h>
#include "Adafruit_TinyUSB.h"
// USB MIDI object
Adafruit_USBD_MIDI usb_midi;
// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
/////////// function prototypes //////////////////
void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);
void handlePitchBend(byte channel, int bend);
void handleControlChange(byte channel, byte control, byte value);
//////////// That's the USB MIDI bit ////////////////////////////

///// now for the multi channel PWM timing
#include <PWMhelper.h>
PWMHelper pwm;
////////////////////

#include "AWG.h"

AWG awg(16000, 255, 1000);

// and the bookkeeping for the polyphony
// Voice structure
struct Voice {
  int note = -1;        // MIDI note number, -1 = free
  int pin;              // GPIO pin used
  bool active = false;  
};
// Only 5 pins → 5 voices
int voicePins[3] = { 26, 28,24};
Voice voices[3];
// Global modulation + pitchbend state
int16_t pitchBendValue = 0;   // -8192…+8191
int pitchBendRange = 2;       // semitones up/down
uint8_t modulationValue = 64; // 0…127
//////////////////////////////////////////////////////////////////////////////////////////////
// the one and only global channel array containing the received values from RF
// at present 14 of the 16 channels are used. Enter the save values (FAILSAFE) in these arrays
//                               0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
int channels[NUM_CHANNELS] =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// for most of the exoot:        X  Y nb kp vo sw sw sw sw
//////////////////////////////////////////////////////////////////////////////////////////////

#include <OledMenu.h>

// global menu instance
OledMenu menu;



// running modes
#define IDLE 0
#define ACTIVE 1
#define PLAYBACK 2

// for triggers or tracks on DFRobot players. Note: they have to be installed
// otherwise the initialisation will hang
#ifdef USE_AUDIO
DFRobot_DF1201S player1,player2;
SoftwareSerial player1port(7, 6);
SoftwareSerial player2port(17, 16);  //RX  TX ( so player TX, player RX)
void processAudio();
#endif
// matching function between keypad/button register and call-back check from action list
// currently using one button channel (characters '0' and higher)
// and 32 switch positions (in 4 bytes)
bool getRemoteSwitch(char button) {
  if((button >='0' && button<='9') || button=='*' || button=='#'){ // check keypad buttons
    #ifdef KEYPAD_CHANNEL
    if(channels[KEYPAD_CHANNEL] == button) return true;
    #endif
  }
  #ifdef SWITCH_CHANNEL
  else if(button >=0 && button < 8) {
    if((channels[SWITCH_CHANNEL]) & 1<<button) return true;
  }
  else if(button >=8 && button < 16) {
    if((channels[SWITCH_CHANNEL+1]) & 1<<(button-8)) return true;
  }
  else if(button >=16 && button < 24) {
    if((channels[SWITCH_CHANNEL+2]) & 1<<(button-16)) return true;
  }
  else if(button >=24 && button < 32) {
    if((channels[SWITCH_CHANNEL+3]) & 1<<(button-24)) return true;
  }
  #endif
  return false;
}
// radio communication: for now either the CRSF (ELRS) or the (old) APC220 434 MHz system
#ifdef USE_CRSF
#include "CRSF.h"
CRSF crsf;
#else
#include "Radio.h"
#endif
//////////////////////////////////////////////////////////////////////////////////////////
// and here the program starts
//////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
 

relay.begin();

  // radio on Serial2: CRSF or APC RF:
#ifdef USE_CRSF
  crsf.begin();
#else
  RFinit();
  RFsetSettings(2);
#endif
// and now start up the channel buffer!
  for (int n = 0; n < NUM_CHANNELS; n++) {
        channels[n] = saveValues[n];
  }
  watchdog_enable(200, 1);  // 100 ms timeout, pause_on_debug = true

  menu.begin();


}


void loop() {
  static int mode = IDLE;  // check the status
  static bool startedUp = false; // to avoid responding to the inital timeout (zero) 
// the following are important to make sure the brakes are switched on after a second of inactivity
  static bool brakeState = 1;
  static unsigned long brakeTimer;

/// encoder sampling
//      pio_sm_exec_wait_blocking(pio, sm, pio_encode_in(pio_x, 32));
//  int position = pio_sm_get_blocking(pio, sm);
/// 



  // Menu handles encoder, button, and screen update
  menu.pollInputs();
  awg.update();
// poll functions outside the 20Hz main loop
// -----------------------------------------------------------------------------
// the 20 Hz main loop starts here!
// -----------------------------------------------------------------------------
  static unsigned long looptime;
  if (millis() > looptime + 19) {
    looptime = millis();

    crsf.GetCrsfPacket();
    if(crsf.crsfData[1] == 24) startedUp = true; // so now we can respond to a timeout
    if (crsf.crsfData[1] == 24 && mode == ACTIVE) {
      if (digitalRead(LED_BUILTIN)) digitalWrite(LED_BUILTIN, LOW);
      else digitalWrite(LED_BUILTIN, HIGH);
      // in 16 channel mode the last two channels are used by ELRS for other things
      // check https://github.com/ExpressLRS/ExpressLRS/issues/2363

      if(true){

        for (int n = 0; n < 16; n++) {
          channels[n] = constrain(map(crsf.channels[n], CRSF_CHANNEL_MIN-CRSF_CHANNEL_OFFSET, CRSF_CHANNEL_MAX-CRSF_CHANNEL_OFFSET, 0, 255),0,255);  //write
        }
      }
      // channels[8] contains the animation start and stop, so is always written by remote
      channels[8] = constrain(map(crsf.channels[8], CRSF_CHANNEL_MIN-CRSF_CHANNEL_OFFSET, CRSF_CHANNEL_MAX, 0, 255),0,255);  //write
      crsf.UpdateChannels();
    }
    if (crsf.getTimeOut() > 9 && mode == ACTIVE) {

      mode = IDLE;
      digitalWrite(LED_BUILTIN, HIGH);

        if(true){

      for (int n = 0; n < NUM_CHANNELS; n++) {
        channels[n] = saveValues[n];
      }
    }
    }

    else if (crsf.getTimeOut() < 1 && mode == IDLE) {


      if(startedUp) mode = ACTIVE;
    }
// this is where the mapping to Relays and sounds takes place

/////////// kick the time out checker! //////////
    crsf.nudgeTimeOut();

// and the motor bit: timer for the brakeState

// important: when an animation is playing (is checked in the animation class)

// now the RF processing
  watchdog_update();
  }  
// ------------------------------------------------------------------
// the end of the 20Hz loop
// ------------------------------------------------------------------
// finally, different timer: screen update

  unsigned long screentimer;
  if (millis() > screentimer + 99) {
    screentimer = millis();



  menu.setMidiActivity(levels);

    //processScreen(mode, 0);
  }
}  // end of main
  
    
double midiNoteToFreq(int note) {
    return 440.0 * pow(2.0, (note - 69) / 12.0);
}


// Apply pitchbend scaling
double applyPitchBend(double baseFreq) {
  double bendSemitones = (pitchBendValue / 8192.0) * pitchBendRange;
  return baseFreq * pow(2.0, bendSemitones / 12.0);
}

// Map modulation (0…127) → duty cycle (0…255)
uint8_t applyModulation() {
  return map(modulationValue, 0, 127, 32, 223); 
  // keeps some headroom, avoids extreme duty cycles
}

// Voice allocation
int allocateVoice(int note) {
  for (int i = 0; i < 3; i++) {
    if (!voices[i].active) {
      voices[i].active = true;
      voices[i].note = note;
      return i;
    }
  }
  // No free voice → simple steal policy: reuse voice 0
  voices[0].note = note;
  return 0;
}

void releaseVoice(int note) {
  for (int i = 0; i < 3; i++) {
    if (voices[i].active && voices[i].note == note) {
      voices[i].active = false;
      pwm.setPWM(voices[i].pin, 1000, 255); // stop output
      voices[i].note = -1;
    }
  }
}

// Update all active voices (for pitchbend/mod changes)
void updateAllVoices() {
  for (int i = 0; i < 3; i++) {
    if (voices[i].active) {
      double baseFreq = midiNoteToFreq(voices[i].note);
      double newFreq  = applyPitchBend(baseFreq);
      uint8_t duty    = applyModulation();
      pwm.setPWM(voices[i].pin, (uint)newFreq, 255-duty);
    }
  }
}




// 
// now on the other core we run the USB joystick bit AND the audio controllers
void setup1(){
 if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  //Serial.begin(115200);  // not useful when using USB MIDI - it is not a Teensy :)
  // THE FOLLOWING LINE CAUSES THE SYSTEM TO NOT RUN: 
  // usb_midi.setStringDescriptor("TinyUSB MIDI");

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // Attach the handleNoteOn function to the MIDI Library. It will
  // be called whenever the Bluefruit receives MIDI Note On messages.
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.setHandleControlChange(handleControlChange);

    for (int i = 0; i < 3; i++) {
    voices[i].pin = voicePins[i];
    voices[i].active = false;
    voices[i].note = -1;
  }

    awg.begin();

  awg.addChannel(18, AWG::SQUARE, 440, 200, 0);

}

void loop1(){
  static unsigned long looptime1;
    // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }
awg.update();
    MIDI.read();
}
// end of core1 code
//--------------------------------------------------------------------------------
// the following function is called when RS485 data is received. This is currently
// not in use
#ifdef USE_RS485
void ProcessRS485Data(int ID, int dataLength, unsigned char *Data) {
}
#endif
// for the other type of radio (not CRSF)
#ifndef USE_CRSF
void ProcessRadioData(int ID, int dataLength, unsigned char *Data) {
  if (digitalRead(LED_BUILTIN)) digitalWrite(LED_BUILTIN, LOW);
  else digitalWrite(LED_BUILTIN, HIGH);
  if (Data[0] == 0x03) {
    for (int n = 0; n < NUM_CHANNELS; n++) {
      channels[n] = Data[n + 1];  //write
    }
  }
}
#endif
// and now for the display function, a number of possible menu visualisations (for now set to menu 1)
#ifdef WORTELTAART
void processScreen(int mode, int position) {
  static int menu = 1;
  display.clearDisplay();
  if (menu == 0) {
    display.fillRect(124, 0, 4, position, SSD1306_WHITE);
    display.setTextSize(1);               // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  // Draw white text
  } else if (menu == 1) {
    display.setTextSize(1);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);  // Start at top-left corner
    if (mode == ACTIVE) display.println(F("ACTIVE"));
    if (mode == IDLE) display.println(F("IDLE"));
    // print keypad char
    display.setCursor(50, 0);
    #ifdef KEYPAD_CHANNEL
    if (channels[KEYPAD_CHANNEL] > 1) display.print((char)(channels[KEYPAD_CHANNEL]));
    #endif
    display.setCursor(70, 0);
#ifdef ANIMATION_KEY
    if (animation.isPlaying()){ 
      display.print (F("anim run"));
    }
    else display.print (F("anim stop"));
#endif
    // print bars
    for (int n = 0; n < NUM_CHANNELS-2; n++) {
      display.fillRect(n * 6, 32 - channels[n] / 8, 4, 32, SSD1306_INVERSE);
    }
    display.fillRect(124, 0, 4, position, SSD1306_WHITE);
  } else if (menu == 2) {
    display.setCursor(0, 0);  // Start at top-left corner
    display.setTextSize(1);   // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.println(F("1234567890 actions"));
    #ifdef NUM_ACTIONS
    for (int i = 0; i < NUM_ACTIONS; i++) {
      if (myActionList[i].getState() == 1) display.fillRect(i * 6, 9, 5, 5, SSD1306_INVERSE);
      else display.drawRect(i * 6, 9, 5, 5, SSD1306_INVERSE);
    }
    #endif
  }
  display.display();
}
#endif
// and the audio processing for a.o. Lumi. 
// current issue is that checking player1.isPlaying() waits for milliseconds, so cannot be used to check file status
// also starting up a file takes more than 50 mS (so you will see a dip in message rate)
// it would be advisable to run this on the other core, were it not for the fact that that would conflict
// with the USB joystick functionality. 
#ifdef USE_AUDIO
void processAudio(void){
  static int isPlaying = 0;
     static int volume1;
     //static int playTimer = 0; 
     int trackToPlay = channels[13]/8;
     if(trackToPlay == 0 && isPlaying && channels[6]<100){
       player1.pause();
       isPlaying = 0;
     }
     else if (trackToPlay >0 && trackToPlay < (NUM_TRACKS+1) && trackToPlay !=isPlaying && channels[6]<100){
       player1.playSpecFile(tracklist[trackToPlay-1]);
       //player1.playFileNum(trackToPlay);
       isPlaying = trackToPlay;
     }
     if(channels[4]!=volume1) player1.setVol(map(channels[4],0,255,0,32));
     volume1 = channels[4];
     // the separate samples:
     static int playingSample;
     static int volume2;
     if(channels[7]!=volume2) player2.setVol(map(channels[7],0,255,0,32));
     volume2 = channels[7];
     
     if(channels[9]>(127+30) && (playingSample !=2)){
      playingSample = 2;
      player2.playFileNum(1);
      // player2.playSpecFile("/mp3/02-ang.mp3");
      //       Serial.print("file: ");
      // Serial.println(player2.getCurFileNumber());
      //playTimer = 20;
     }
     else if (channels[9]<(127-30) && (playingSample !=5)){
      player2.playFileNum(6);
      // player2.playSpecFile("/mp3/05-noo.mp3");
      // Serial.print("file: ");
      // Serial.println(player2.getCurFileNumber());
      playingSample = 5;
      //playTimer = 20;
     }
     else if ((channels[11]&16) && (playingSample != 3)){
      //player2.playSpecFile("/mp3/03-slp.mp3");
      playingSample = 3; 
      player2.playFileNum(3);
      //       Serial.print("file: ");
      // Serial.println(player2.getCurFileNumber());
      //playTimer = 20;
     }
     else if ((channels[11]&64) && (playingSample != 6)){
      //player2.playSpecFile("/mp3/06-yes.mp3");
      playingSample = 6; 
      player2.playFileNum(7);
      //       Serial.print("file: ");
      // Serial.println(player2.getCurFileNumber());
      //playTimer = 20;
     }
     else if ((channels[0]<100) && (playingSample != 4)){
      // player2.playSpecFile("/mp3/04-mov.mp3");
      //       Serial.print("file: ");
      // Serial.println(player2.getCurFileNumber());
      player2.playFileNum(4);
      playingSample = 4; 
      //playTimer = 20;
     }   
      else if (((channels[12]&16) || (channels[12]&32)) && (playingSample != 1)){
        player2.playFileNum(8);
      // player2.playSpecFile("/mp3/01-alm.mp3");
      //       Serial.print("file: ");
      // Serial.println(player2.getCurFileNumber());
      playingSample = 1; 
      //playTimer = 20;
     }
     //else playingSample = 0;
      //if(playTimer>0) playTimer --; 
      //if(playTimer ==0) playingSample = 0;  
}
#endif


// MIDI Handlers
void handleNoteOn(byte channel, byte note, byte velocity) {
  if (velocity == 0) { handleNoteOff(channel, note, velocity); return; }
  int v = allocateVoice(note);
  voices[v].note = note;
  voices[v].active = true;
  updateAllVoices(); // recalc with current pitchbend/mod
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  releaseVoice(note);
}

void handlePitchBend(byte channel, int bend) {
  pitchBendValue = bend; // -8192…+8191
  updateAllVoices();
}

void handleControlChange(byte channel, byte control, byte value) {
  if (control == 1) { // Modulation wheel
    modulationValue = value; // 0…127
    updateAllVoices();
  }
}