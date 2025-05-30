#include "Oscillator.h"
#include <cmath>

constexpr float PI = 3.14159265f;

void Oscillator::setType(WaveType type) {
    waveType = type;
}

void Oscillator::setFrequency(float f) {
    frequency = f;
}

float Oscillator::generate() {
    float value = 0.0f;

    switch (waveType) {
    case WaveType::SINE:
        value = std::sin(phase);
        break;
    case WaveType::SQUARE:
        value = std::sin(phase) > 0 ? 1.0f : -1.0f;
        break;
    case WaveType::SAW:
        value = 2.0f * (phase / (2.0f * PI) - std::floor(phase / (2.0f * PI) + 0.5f));
        break;
    }

    phase += 2.0f * PI * frequency / SAMPLE_RATE;
    if (phase >= 2.0f * PI)
        phase -= 2.0f * PI;

    return value * 0.5f; // Pour rester dans [-0.5 ; 0.5]
}
