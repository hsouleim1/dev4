#include "Envelope.h"
#include <algorithm>
#include "Constants.h"

void Envelope::setAttackRelease(float a, float r) {
    attack = a;
    release = r;
}

float Envelope::getAmplitude(bool keyDown) {
    float dt = 1.0f / SAMPLE_RATE;
    if (keyDown) {
        amplitude += dt / std::max(attack, 0.001f);
        amplitude = std::min(amplitude, 1.0f);
    } else {
        amplitude -= dt / std::max(release, 0.001f);
        amplitude = std::max(amplitude, 0.0f);
    }
    return amplitude;
}
