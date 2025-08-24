// Code to use the PWM timers assigned to the different 
// slices (always two pins grouped to a slice, so careful
// selection is necessary)
// these functions use the normal Pico SDK instead of the 
// Arduino analogwrite derivative


#include "PWMhelper.h"

PWMHelper::PWMHelper() {
    for (int i = 0; i < 30; i++) {
        states[i].active = false;
        states[i].lastFreq = 0;
        states[i].wrap = 0;
    }
}

void PWMHelper::setPWM(uint pin, uint freq, uint8_t duty) {
    // Constrain values
    freq = constrainFreq(freq);
    duty = constrainDuty(duty);

    for(int i = 0; i<3; i++){
        if(pin==voicePins[3]) levels[i] = duty;
    }
    // Configure pin for PWM if first time
    if (!states[pin].active) {
        gpio_set_function(pin, GPIO_FUNC_PWM);
        states[pin].active = true;
    }

    // Get slice and channel
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel   = pwm_gpio_to_channel(pin);

    uint32_t wrap;

    if (freq != states[pin].lastFreq) {
        const uint32_t clock = 125000000; // system clock

        // Try to find suitable divider and wrap
        uint32_t divider16 = 16; // start at 1.0 (8.4 fixed point)
        wrap = 0;

        while (divider16 < (255 << 4)) {
            wrap = (clock / (freq * (divider16 >> 4))) - 1;
            if (wrap <= 0xFFFF) break; // fits in 16-bit
            divider16 += 16; // increment by 1.0
        }

        if (wrap > 0xFFFF) wrap = 0xFFFF; // clamp just in case

        // Apply settings
        pwm_set_clkdiv_int_frac(slice_num, divider16 >> 4, divider16 & 0xF);
        pwm_set_wrap(slice_num, wrap);

        // Store state
        states[pin].lastFreq = freq;
        states[pin].wrap = wrap;

        pwm_set_enabled(slice_num, true);
    } else {
        wrap = states[pin].wrap;
    }

    // Scale duty (0–255 → 0–wrap)
    uint32_t level = ((uint32_t)duty * (wrap + 1)) / 255;
    pwm_set_chan_level(slice_num, channel, level);
}

uint32_t PWMHelper::constrainFreq(uint32_t freq) {
    if (freq < 1) freq = 1;
    if (freq > 16000) freq = 16000; // limited to your requested range
    return freq;
}

uint8_t PWMHelper::constrainDuty(uint8_t duty) {
    if (duty > 255) duty = 255;
    return duty;
}
