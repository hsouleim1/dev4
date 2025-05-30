#include "Filter.h"
#include <cmath>
#include "Constants.h"

void Filter::setCutoff(float newCutoff) {
    cutoff = fmaxf(20.0f, fminf(newCutoff, SAMPLE_RATE / 2.0f));  // clamp
}

void Filter::setResonance(float newResonance) {
    resonance = fmaxf(0.0f, fminf(newResonance, 1.0f));  // clamp
}

float Filter::process(float input) {
    float f = 2.0f * sinf(PI * cutoff / SAMPLE_RATE);
    feedback = resonance + resonance / (1.0f - f);

    buf0 += f * (input - buf0 + feedback * (buf0 - buf1));
    buf1 += f * (buf0 - buf1);
    return buf1;
}
