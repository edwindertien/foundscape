#ifndef AWG_H
#define AWG_H

#include <Arduino.h>
#include <vector>

class AWG {
public:
    enum WaveType { SINE, TRIANGLE, SAW, SQUARE };

    struct Channel {
        int pin;
        WaveType type;
        float frequency; // Hz
        int amplitude;   // 0 - maxDuty
        float phase;     // degrees
        float t;         // internal time tracker
    };

    AWG(int pwmFreq, int maxDuty = 255, int updateRate = 1000);

    void addChannel(int pin, WaveType type, float freq, int amp, float phase);
    void begin();          // initialize PWM
    void update();         // call in loop() to update all channels

    // Dynamic setters
    void setWaveform(int channelIndex, WaveType type);
    void setFrequency(int channelIndex, float freq);
    void setAmplitude(int channelIndex, int amp);
    void setPhase(int channelIndex, float phase);

private:
    int _pwmFreq;
    int _maxDuty;
    int _updateRate;
    unsigned long _prevMicros;

    std::vector<Channel> _channels;

    float computeWaveform(Channel &ch);
};

#endif
