#include "Oscillator.h"
#include <cmath>            // Pour std::sin, std::floor
#include "constants.h"      // Pour SAMPLE_RATE et PI

// Définir le type d'onde utilisé par cet oscillateur (sinusoïdale, carrée, dent de scie)
void Oscillator::setType(WaveType type) {
    waveType = type;
}

// Définir la fréquence (en Hz) de l'oscillateur
void Oscillator::setFrequency(float f) {
    frequency = f;
}

// Génère un échantillon audio en fonction du type d'onde sélectionné
float Oscillator::generate() {
    float value = 0.0f;

    switch (waveType) {
    case WaveType::SINE:
        // Onde sinusoïdale classique
            value = std::sin(phase);
        break;

    case WaveType::SQUARE:
        // Onde carrée (1 ou -1 selon le signe de la sinusoïde)
            value = std::sin(phase) > 0 ? 1.0f : -1.0f;
        break;

    case WaveType::SAW:
        // Onde en dent de scie : variation linéaire de -1 à 1
            value = 2.0f * (phase / (2.0f * PI) - std::floor(phase / (2.0f * PI) + 0.5f));
        break;
    }

    // Avancement de la phase pour la prochaine itération
    phase += 2.0f * PI * frequency / SAMPLE_RATE;

    // Remise à zéro de la phase si elle dépasse une période (2π)
    if (phase >= 2.0f * PI)
        phase -= 2.0f * PI;

    // Atténuer le volume pour éviter la saturation (signal dans [-0.5 ; 0.5])
    return value * 0.5f;
}
