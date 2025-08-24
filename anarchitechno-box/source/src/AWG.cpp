#include "AWG.h"
#include <math.h>
#include <PWMhelper.h>
// Replace this with your actual PWM object
extern PWMHelper pwm; // must have pwm.setPWM(pin, freq, duty)

AWG::AWG(int pwmFreq, int maxDuty, int updateRate)
    : _pwmFreq(pwmFreq), _maxDuty(maxDuty), _updateRate(updateRate), _prevMicros(0) {}

void AWG::addChannel(int pin, WaveType type, float freq, int amp, float phase) {
    Channel ch;
    ch.pin = pin;
    ch.type = type;
    ch.frequency = freq;
    ch.amplitude = constrain(amp, 0, _maxDuty);
    ch.phase = phase;
    ch.t = 0;
    _channels.push_back(ch);
}

void AWG::begin() {
    // Initialize PWM if needed
    // e.g., pwm.begin();
}

void AWG::update() {
    unsigned long now = micros();
    float dt = (now - _prevMicros) / 1000000.0; // delta time in seconds
    if (dt <= 0) return; // protect against no time elapsed

    _prevMicros = now;

    for (auto &ch : _channels) {
        ch.t += dt;                      // increment by actual elapsed time
        float value = computeWaveform(ch);
        int duty = value * ch.amplitude;
        pwm.setPWM(ch.pin, _pwmFreq, duty);
    }
}

float AWG::computeWaveform(Channel &ch) {
    float angle = 2 * PI * ch.frequency * ch.t + radians(ch.phase);
    switch (ch.type) {
        case SINE:
            return (sin(angle) + 1.0) / 2.0;
        case TRIANGLE:
            return 2.0 * fabs(fmod(angle / (2*PI), 1.0) - 0.5);
        case SAW:
            return fmod(angle / (2*PI), 1.0);
        case SQUARE:
            return sin(angle) >= 0 ? 1.0 : 0.0;
        default:
            return 0;
    }
}

// --- Dynamic setters ---
void AWG::setWaveform(int idx, WaveType type) {
    if (idx >= 0 && idx < _channels.size()) _channels[idx].type = type;
}

void AWG::setFrequency(int idx, float freq) {
    if (idx >= 0 && idx < _channels.size()) _channels[idx].frequency = freq;
}

void AWG::setAmplitude(int idx, int amp) {
    if (idx >= 0 && idx < _channels.size())
        _channels[idx].amplitude = constrain(amp, 0, _maxDuty);
}

void AWG::setPhase(int idx, float phase) {
    if (idx >= 0 && idx < _channels.size()) _channels[idx].phase = phase;
}
