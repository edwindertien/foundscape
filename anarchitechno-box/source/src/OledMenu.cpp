#include <OledMenu.h>
#include <Arduino.h>

OledMenu::OledMenu(int width, int height)
: display(width, height, &Wire, -1) {
  uiState = BASE_SCREEN;
  midiChannel = 1;
  menuIndex = 0;
  editValue = 0;
  lastEncPos = 0;
  lastDraw = 0;
  needsRedraw = true;
  encPosAbs = 0;
  buttonState = HIGH;
  lastDebounceTime = 0;
  for (int i=0; i<16; i++) {
    channels[i].isPWM = true;
    channels[i].freq = 1000;
    channels[i].duty = 50;
    activityLevels[i] = 0;
  }
}

void OledMenu::begin(uint8_t i2cAddr) {
  if (!display.begin(SSD1306_SWITCHCAPVCC, i2cAddr)) {
    for(;;); // hang if not found
  }
  display.clearDisplay();
  display.display();

  // --- Init pins ---
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  pinMode(QUADRATURE_A_PIN, INPUT_PULLUP);
  pinMode(QUADRATURE_B_PIN, INPUT_PULLUP);

  // --- Init PIO quadrature ---
  pio = pio0;
  sm = pio_claim_unused_sm(pio, true);
  unsigned int offset = pio_add_program(pio, &quadratureA_program);
  quadratureA_program_init(pio, sm, offset, QUADRATURE_A_PIN, QUADRATURE_B_PIN);
}

void OledMenu::pollInputs() {
  // ---- Encoder ----
  //pio_sm_exec_wait_blocking(pio, sm, pio_encode_in(pio_x, 32));
  //int32_t newPos = pio_sm_get_blocking(pio, sm);
  static int encoderPosition;
  while (!pio_sm_is_rx_fifo_empty(pio, sm)) {
    int newPosition = pio_sm_get_blocking(pio, sm); // or non-blocking variant
    int delta = newPosition - encoderPosition;      // compute delta
    encoderPosition = newPosition;

      Serial.println(newPosition);
  onEncoderPosition(newPosition);
}

  


 bool cur = !digitalRead(PUSH_BUTTON); // true = pressed
if (cur != lastButtonReading) {
    lastDebounceTime = millis();  // reset debounce timer on any change
}
lastButtonReading = cur; // store for next loop

if ((millis() - lastDebounceTime) > 20) { // button stable for 20 ms
    if (cur != buttonState) {  // only fire on actual change
        buttonState = cur;     // update stable state
        if (buttonState) {    // pressed
            onButtonPress();
        }
    }
}

  // ---- Screen update ----
  update();
}

void OledMenu::onEncoderPosition(int32_t newPos) {
  int delta = newPos - lastEncPos;
  if (delta == 0) return;
  switch (uiState) {
    case MENU_MAIN:
      menuIndex = constrain(menuIndex + delta/4, 0, 16);
      break;
    case MENU_EDIT:
      editValue += delta/4;
      break;
    default: break;
  }
  lastEncPos = newPos;
  needsRedraw = true;
}

void OledMenu::onButtonPress() {
  Serial.print("State change: ");
  switch (uiState) {
    case BASE_SCREEN: uiState = MENU_MAIN; Serial.println("BASE->MENU_MAIN"); break;
    case MENU_MAIN:   uiState = MENU_CHANNEL; Serial.println("MAIN->CHANNEL"); break;
    case MENU_CHANNEL:uiState = MENU_EDIT; Serial.println("CHANNEL->EDIT"); break;
    case MENU_EDIT:   uiState = MENU_MAIN; Serial.println("EDIT->MAIN"); break;
  }
  needsRedraw = true;
}

void OledMenu::update() {
  unsigned long now = millis();
  if (needsRedraw || (now - lastDraw >= 100)) { // ~10 fps
    draw();
    lastDraw = now;
    needsRedraw = false;
  }
}

void OledMenu::setMidiActivity(const uint8_t activity[16]) {
  for (int i=0; i<16; i++) {
    activityLevels[i] = activity[i];
  }
  if (uiState == BASE_SCREEN) needsRedraw = true;
}

void OledMenu::draw() {
  display.clearDisplay();
  switch (uiState) {
    case BASE_SCREEN: drawBaseScreen(); break;
    case MENU_MAIN:   drawMainMenu(); break;
    case MENU_CHANNEL:drawChannelMenu(); break;
    case MENU_EDIT:   drawEditMenu(); break;
  }
  display.display();
}

void OledMenu::drawBaseScreen() {
  for (int i = 0; i < 16; i++) {
    int barHeight = activityLevels[i];
    if (barHeight > 31) barHeight = 31;
    display.fillRect(i*8, 31 - barHeight, 6, barHeight, SSD1306_WHITE);
  }
}

void OledMenu::drawMainMenu() {
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("MIDI Ch: "); display.print(midiChannel);

  display.setCursor(0,10);
  display.print("Channels...");

  display.drawRect(0, menuIndex*10, 127, 10, SSD1306_WHITE);
}

void OledMenu::drawChannelMenu() {
  display.setCursor(0,0);
  display.print("Channel "); display.print(menuIndex+1);
  display.setCursor(0,10);
  display.print(channels[menuIndex].isPWM ? "PWM" : "Pulse");
  display.setCursor(0,20);
  display.print("F:"); display.print(channels[menuIndex].freq);
  display.print(" D:"); display.print(channels[menuIndex].duty);
}

void OledMenu::drawEditMenu() {
  display.setCursor(0,0);
  display.print("Edit value: ");
  display.print(editValue);
}
