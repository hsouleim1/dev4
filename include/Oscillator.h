#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "WaveType.h"

#include "Constants.h"


class Oscillator {
public:
    void setType(WaveType type);
    void setFrequency(float f);
    float generate();

private:
    float frequency = 440.0f;
    float phase = 0.0f;
    WaveType waveType = WaveType::SINE;
};

#endif
