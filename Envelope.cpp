#include "Envelope.h"
#include <algorithm>

void Envelope::setAttackRelease(float a, float r) {
    attack = a;
    release = r;
}

float Envelope::process(bool pressed, float dt) {
    if (pressed) {
        amplitude += dt / std::max(attack, 0.001f);
        amplitude = std::min(amplitude, 1.f);
    } else {
        amplitude -= dt / std::max(release, 0.001f);
        amplitude = std::max(amplitude, 0.f);
    }
    return amplitude;
}
