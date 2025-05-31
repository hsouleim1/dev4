#include "Filter.h"
#include <cmath>
#include "Constants.h"

void Filter::setCutoff(float newCutoff) {
    // Clamp entre 20 Hz et 8000 Hz (au lieu de 20000)
    cutoff = fmaxf(20.0f, fminf(newCutoff, 8000.0f));
}

void Filter::setResonance(float newResonance) {
    // Clamp entre 0.0 et 0.9 pour éviter les sifflements/saturation
    resonance = fmaxf(0.0f, fminf(newResonance, 0.9f));
}

float Filter::process(float input) {
    // Calcule un facteur f stable et limité
    float f = 2.0f * sinf(PI * cutoff / SAMPLE_RATE);
    f = fminf(f, 0.99f); // sécurité pour éviter un feedback infini

    feedback = resonance + resonance / (1.0f - f + 1e-5f); // évite division par zéro

    // filtre passe-bas à deux étages (style Moog simplifié)
    buf0 += f * (input - buf0 + feedback * (buf0 - buf1));
    buf1 += f * (buf0 - buf1);

    return buf1;
}
