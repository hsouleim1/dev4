#include "AudioGenerator.h"
#include <cmath>

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
    float* out = static_cast<float*>(output);
    auto* gen = static_cast<AudioGenerator*>(userData);

    for (unsigned long i = 0; i < frameCount; ++i) {
        float sample = gen->generateSample();
        *out++ = sample;
        *out++ = sample;
    }

    return paContinue;
}

float AudioGenerator::generateSample() {
    std::lock_guard<std::mutex> lock(paramMutex);

    if (noteIndex < 0) return 0.0f;
    float freq = noteFrequencies[noteIndex];

    osc1.setFrequency(freq + offset1);
    osc2.setFrequency(freq + offset2);

    float sample = 0.0f;
    if (osc1Active) sample += osc1.generate();
    if (osc2Active) sample += osc2.generate();

    sample *= envelope.getAmplitude(keyPressed);
    return sample * 0.3f;
}

void AudioGenerator::setOsc1(bool active, WaveType type, float offset) {
    std::lock_guard<std::mutex> lock(paramMutex);
    osc1Active = active;
    osc1.setType(type);
    offset1 = offset;
}

void AudioGenerator::setOsc2(bool active, float offset) {
    std::lock_guard<std::mutex> lock(paramMutex);
    osc2Active = active;
    offset2 = offset;
    osc2.setType(WaveType::SAW);
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
