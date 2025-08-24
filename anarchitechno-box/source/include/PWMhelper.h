#pragma once
#include <Arduino.h>
#include "hardware/pwm.h"
#include "pico/stdlib.h"

extern uint8_t levels[8];
extern int voicePins[3];
/**
 * PWMHelper
 * =========
 * Wrapper for RP2040 PWM (Raspberry Pi Pico).
 * 
 * Limits:
 *  - Duty cycle: 0–255 (mapped to 0%–100%).
 *  - Frequency: 20 Hz – 16,000 Hz (clamped to this range).
 *  - Max unique frequencies: 8 (one per slice).
 *  - Max PWM pins: 16 (2 per slice).
 */
class PWMHelper {
public:
    PWMHelper();

    /**
     * Set PWM on a GPIO pin.
     * If frequency is unchanged for this pin, only duty is updated.
     * 
     * @param pin   GPIO number (e.g., 2, 3, ...)
     * @param freq  Frequency in Hz (20–16,000 Hz)
     * @param duty  Duty cycle (0–255 → 0%–100%)
     */
    void setPWM(uint pin, uint freq, uint8_t duty);

private:
    struct PinState {
        bool active = false;
        uint32_t lastFreq = 0;
        uint32_t wrap = 0;
    };

    PinState states[30]; // support up to GPIO 0..29

    uint32_t constrainFreq(uint32_t freq);
    uint8_t constrainDuty(uint8_t duty);
};
