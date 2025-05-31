#include "Filter.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>  // Pour std::clamp, std::min

// Met à jour la fréquence de coupure du filtre (en Hz) avec des limites de sécurité.
void Filter::setCutoff(float newCutoff) {
    cutoff = std::clamp(newCutoff, 20.0f, 8000.0f);  // clamp évite les valeurs extrêmes
}

// Met à jour la résonance du filtre, en la bornant pour éviter des effets audio indésirables.
void Filter::setResonance(float newResonance) {
    resonance = std::clamp(newResonance, 0.0f, 0.9f);  // > 0.9 risque de sifflement/instabilité
}

// Applique le filtre passe-bas à deux étages sur l'échantillon d'entrée et retourne le résultat filtré.
float Filter::process(float input) {
    // Calcul du facteur de coupe (dépend du cutoff et de la fréquence d’échantillonnage)
    float f = 2.0f * sinf(PI * cutoff / SAMPLE_RATE);
    f = std::min(f, 0.99f);  // sécurité pour éviter un feedback trop élevé

    // Calcul du feedback basé sur la résonance, avec petite sécurité pour éviter division par 0
    feedback = resonance + resonance / (1.0f - f + 1e-5f);

    // Application d’un filtre passe-bas de type Moog (simplifié)
    buf0 += f * (input - buf0 + feedback * (buf0 - buf1));
    buf1 += f * (buf0 - buf1);

    // Retourne la sortie du deuxième tampon (sortie filtrée)
    return buf1;
}
