#ifndef AUDIOGENERATOR_H
#define AUDIOGENERATOR_H

constexpr float SAMPLE_RATE = 44100.0f;
#include <portaudio.h>

enum class WaveType { SINE, SQUARE, SAW };

class AudioGenerator {
public:
    static AudioGenerator& getInstance();

    float generateSample(float t);
    float applyEnvelope(bool keyDown, float dt);

    void setOsc1(bool active, WaveType type, float offset);
    void setOsc2(bool active, float offset);
    void setEnvelope(float attack, float release);
    void setNote(int index); // 0 à 12 pour 13 touches
    bool init();

    bool keyPressed = false;

private:
    AudioGenerator() = default;
    ~AudioGenerator();

    static int paCallback(const void*, void*, unsigned long,
                          const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);

    PaStream* stream = nullptr;
    float time = 0.0f;

    // Oscillateurs
    bool osc1Active = true, osc2Active = false;
    WaveType wave1 = WaveType::SINE;
    float offset1 = 0.0f, offset2 = 0.0f;

    // Enveloppe
    float attack = 0.1f;
    float release = 0.5f;
    float amplitude = 0.0f;

    // Note
    int noteIndex = -1; // Index 0-12 pour les touches, -1 si rien
};

#endif // AUDIOGENERATOR_H
