#ifndef CONSTANTS_H
#define CONSTANTS_H

// Fréquence d'échantillonnage
constexpr float SAMPLE_RATE = 44100.0f;

// Pi pour les calculs trigonométriques
constexpr float PI = 3.14159265f;

// Nombre de notes gérées
constexpr int NUM_NOTES = 13;

// Fréquences des 13 notes (Do → Do aigu)
constexpr float NOTE_FREQUENCIES[NUM_NOTES] = {
    130.81f, 138.59f, 146.83f, 155.56f, 164.81f,
    174.61f, 185.00f, 196.00f, 207.65f, 220.00f,
    233.08f, 246.94f, 261.63f
};

// Valeurs limites pour les effets
constexpr float MIN_DELAY_TIME = 0.1f;
constexpr float MAX_DELAY_TIME = 2.0f;
constexpr float MIN_DELAY_MIX = 0.0f;
constexpr float MAX_DELAY_MIX = 1.0f;

#endif // CONSTANTS_H
