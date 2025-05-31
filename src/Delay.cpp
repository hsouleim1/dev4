#include "Delay.h"
#include <algorithm>  // pour std::clamp, std::min

// Constructeur : initialise le buffer circulaire avec une taille max définie par la durée de delay
Delay::Delay(int sampleRate, float maxDelaySeconds)
    : sampleRate(sampleRate), writeIndex(0) {
    maxSamples = static_cast<int>(maxDelaySeconds * sampleRate);  // taille maximale du buffer
    buffer.resize(maxSamples, 0.0f);  // initialisation à zéro
}

// Met à jour les paramètres du delay : durée et taux de mixage entre son direct et écho
void Delay::setParameters(float delayTimeSeconds, float mixAmount) {
    std::lock_guard<std::mutex> lock(delayMutex);  // thread-safety
    delayTime = std::clamp(delayTimeSeconds, 0.1f, 2.0f);  // clamp de sécurité (évite delay trop court/long)
    mix = std::clamp(mixAmount, 0.0f, 1.0f);  // clamp du mix direct/retardé
}

// Applique l'effet de delay à un échantillon audio et retourne le résultat mixé
float Delay::process(float inputSample) {
    std::lock_guard<std::mutex> lock(delayMutex);  // protection thread-safe

    // Nombre d'échantillons correspondant au délai actuel
    int delaySamples = static_cast<int>(delayTime * sampleRate);
    delaySamples = std::min(delaySamples, maxSamples - 1);  // sécurité contre dépassement du buffer

    // Calcul de l'index de lecture circulaire (en arrière par rapport au writeIndex)
    int readIndex = (writeIndex - delaySamples + maxSamples) % maxSamples;
    float delayed = buffer[readIndex];  // échantillon retardé

    float feedbackDecay = 0.5f;  // contrôle le feedback/rétroaction : valeur >1.0f = instable
    buffer[writeIndex] = inputSample + delayed * feedbackDecay;  // feedback dans le buffer

    writeIndex = (writeIndex + 1) % maxSamples;  // avancer l’index d’écriture circulaire

    // Mix entre le signal direct et l’écho
    return inputSample + delayed * mix;
}
