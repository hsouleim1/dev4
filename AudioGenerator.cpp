#include "AudioGenerator.h"
#include <cmath>
#include <algorithm>

constexpr float PI = 3.14159265f;

// Tableau des fréquences pour 13 notes (DO3 à DO4)
const float noteFrequencies[13] = {
    261.63f, 277.18f, 293.66f, 311.13f, 329.63f,
    349.23f, 369.99f, 392.00f, 415.30f, 440.00f,
    466.16f, 493.88f, 523.25f
};

AudioGenerator& AudioGenerator::getInstance() {
    static AudioGenerator instance;
    return instance;
}

int AudioGenerator::paCallback(const void*, void* output, unsigned long frameCount,
                                const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    float* out = (float*)output;
    AudioGenerator* gen = (AudioGenerator*)userData;

    for (unsigned long i = 0; i < frameCount; ++i) {
        float t = gen->time;
        float sample = gen->generateSample(t);
        gen->time += 1.0f / SAMPLE_RATE;

        *out++ = sample;
        *out++ = sample;
    }

    return paContinue;
}

float AudioGenerator::generateSample(float t) {
    float freq = noteIndex >= 0 ? noteFrequencies[noteIndex] : 0.0f;
    float dt = 1.0f / SAMPLE_RATE;

    float phase1 = 2 * PI * (freq + offset1) * t;
    float phase2 = 2 * PI * (freq + offset2) * t;

    auto getWaveSample = [](WaveType type, float phase) -> float {
        switch (type) {
            case WaveType::SINE:   return std::sin(phase);
            case WaveType::SQUARE: return std::sin(phase) > 0 ? 1.f : -1.f;
            case WaveType::SAW:    return 2.f * (phase / (2.f * PI) - std::floor(phase / (2.f * PI) + 0.5f));
            default: return 0.f;
        }
    };

    float sample = 0.f;
    if (osc1Active) sample += getWaveSample(wave1, phase1);
    if (osc2Active) sample += getWaveSample(WaveType::SAW, phase2); // Osc2 est toujours SAW

    sample *= applyEnvelope(keyPressed, dt);
    return sample * 0.3f; // volume réduit
}

float AudioGenerator::applyEnvelope(bool pressed, float dt) {
    if (pressed) {
        amplitude += dt / std::max(attack, 0.001f);
        amplitude = std::min(amplitude, 1.f);
    } else {
        amplitude -= dt / std::max(release, 0.001f);
        amplitude = std::max(amplitude, 0.f);
    }
    return amplitude;
}

void AudioGenerator::setOsc1(bool active, WaveType type, float off) {
    osc1Active = active;
    wave1 = type;
    offset1 = off;
}

void AudioGenerator::setOsc2(bool active, float off) {
    osc2Active = active;
    offset2 = off;
}

void AudioGenerator::setEnvelope(float atk, float rel) {
    attack = atk;
    release = rel;
}

void AudioGenerator::setNote(int index) {
    noteIndex = (index >= 0 && index < 13) ? index : -1;
    keyPressed = (noteIndex != -1);
}

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
