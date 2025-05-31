#include "AudioGenerator.h"
#include <cmath>
#include <algorithm>
#include <Constants.h>



// Singleton
AudioGenerator& AudioGenerator::getInstance() {
    static AudioGenerator instance;
    return instance;
}

// Callback PortAudio : remplit le buffer de sortie
int AudioGenerator::paCallback(const void*, void* output, unsigned long frameCount,
                               const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    auto* out = static_cast<float*>(output);
    auto* gen = static_cast<AudioGenerator*>(userData);

    for (unsigned long i = 0; i < frameCount; ++i) {
        float sample = gen->generateSample();
        out[2 * i]     = sample; // Canal gauche
        out[2 * i + 1] = sample; // Canal droit (mono en stéréo)
    }

    return paContinue;
}

// Génération d'un échantillon audio
float AudioGenerator::generateSample() {
    std::lock_guard<std::mutex> lock(paramMutex);

    if (noteIndex < 0) return 0.0f;

    float freq = NOTE_FREQUENCIES[noteIndex];

    // Configuration des oscillateurs
    osc1.setFrequency(freq + offset1);
    osc2.setFrequency(freq + offset2);

    float sample = 0.0f;
    if (osc1Active) sample += osc1.generate();
    if (osc2Active) sample += osc2.generate();

    // Filtrage
    sample = filter.process(sample);

    // Enveloppe AR
    sample *= envelope.getAmplitude(keyPressed);

    // Delay
    delay.setParameters(delayTime, delayMix);
    sample = delay.process(sample);

    return sample * 0.3f; // Atténuation globale
}

// ==== Configuration des modules audio ====

void AudioGenerator::setOsc1(bool active, WaveType type, float offset) {
    std::lock_guard<std::mutex> lock(paramMutex);
    osc1Active = active;
    osc1.setType(type);
    offset1 = offset;
}

void AudioGenerator::setOsc2(bool active, float offset) {
    std::lock_guard<std::mutex> lock(paramMutex);
    osc2Active = active;
    osc2.setType(WaveType::SAW); // Oscillateur 2 toujours en saw
    offset2 = offset;
}

void AudioGenerator::setEnvelope(float attack, float release) {
    std::lock_guard<std::mutex> lock(paramMutex);
    envelope.setAttackRelease(attack, release);
}

void AudioGenerator::setNote(int index) {
    std::lock_guard<std::mutex> lock(paramMutex);
    noteIndex = (index >= 0 && index < 13) ? index : -1;
    keyPressed = (noteIndex != -1);
}

void AudioGenerator::setKeyPressed(bool pressed) {
    std::lock_guard<std::mutex> lock(paramMutex);
    keyPressed = pressed;
}

bool AudioGenerator::isKeyPressed() {
    std::lock_guard<std::mutex> lock(paramMutex);
    return keyPressed;
}

// ==== Initialisation PortAudio ====

bool AudioGenerator::init() {
    Pa_Initialize();
    return Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, 256, paCallback, this) == paNoError
        && Pa_StartStream(stream) == paNoError;
}

AudioGenerator::~AudioGenerator() {
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
    }
    Pa_Terminate();
}

// ==== Paramètres du filtre ====

void AudioGenerator::setCutoff(float c) {
    std::lock_guard<std::mutex> lock(paramMutex);
    cutoff = c;
    filter.setCutoff(cutoff);
}

void AudioGenerator::setResonance(float r) {
    std::lock_guard<std::mutex> lock(paramMutex);
    resonance = r;
    filter.setResonance(resonance);
}

// ==== Paramètres du delay ====

void AudioGenerator::setDelayTime(float seconds) {
    std::lock_guard<std::mutex> lock(paramMutex);
    delayTime = std::clamp(seconds, MIN_DELAY_TIME, MAX_DELAY_TIME);

}

void AudioGenerator::setDelayMix(float mix) {
    std::lock_guard<std::mutex> lock(paramMutex);
    delayMix = std::clamp(mix, MIN_DELAY_TIME, MAX_DELAY_TIME);
}

void AudioGenerator::setDelay(float time, float mix) {
    delay.setParameters(time, mix); // Non protégé par mutex : à usage interne uniquement
}
