#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <hardware/pio.h>
#include "quadrature.pio.h"   // your PIO quadrature code

// Encoder pins (adjust if needed)
#define QUADRATURE_A_PIN 20
#define QUADRATURE_B_PIN 21
#define PUSH_BUTTON      22

enum UiState { BASE_SCREEN, MENU_MAIN, MENU_CHANNEL, MENU_EDIT };

struct ChannelConfig {
  bool isPWM;
  int freq;
  int duty;
};

class OledMenu {
public:
  OledMenu(int width=128, int height=32);

  void begin(uint8_t i2cAddr=0x3C);

  // Call once per loop: handles encoder + button + redraw
  void pollInputs();

  // MIDI activity (array of 16 levels 0–31)
  void setMidiActivity(const uint8_t activity[16]);

  int getMidiChannel() const { return midiChannel; }
  void setMidiChannel(int ch) { midiChannel = ch; }

  ChannelConfig& channel(int i) { return channels[i]; }

private:
  Adafruit_SSD1306 display;
  UiState uiState;

  int midiChannel;
  ChannelConfig channels[16];

  int menuIndex;
  int editValue;
  uint8_t activityLevels[16];

  int32_t lastEncPos;         // internal tracking for delta
  bool needsRedraw;           
  unsigned long lastDraw;     

  // Encoder + PIO state
  PIO pio;
  unsigned int sm;
  int32_t encPosAbs;

  // Button state
  bool buttonState;
  bool lastButtonReading;
  unsigned long lastDebounceTime;

  // Internal helpers
  void onEncoderPosition(int32_t newPos);
  void onButtonPress();
  void update();

  void draw();
  void drawBaseScreen();
  void drawMainMenu();
  void drawChannelMenu();
  void drawEditMenu();
};
