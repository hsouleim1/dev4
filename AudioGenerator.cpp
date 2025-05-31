#include "AudioGenerator.h"
#include <cmath>
#include <algorithm>
#include <vector>

const float noteFrequencies[13] = {
    130.81f, 138.59f, 146.83f, 155.56f, 164.81f,
    174.61f, 185.00f, 196.00f, 207.65f, 220.00f,
    233.08f, 246.94f, 261.63f
};

AudioGenerator& AudioGenerator::getInstance() {
    static AudioGenerator instance;
    return instance;
}

int AudioGenerator::paCallback(const void*, void* output, unsigned long frameCount,
                               const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    auto* out = static_cast<float*>(output);
    auto* gen = static_cast<AudioGenerator*>(userData);

    for (unsigned long i = 0; i < frameCount; ++i) {
        float sample = gen->generateSample();
        out[2 * i] = sample;
        out[2 * i + 1] = sample;
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

    sample = filter.process(sample);
    sample *= envelope.getAmplitude(keyPressed);

    delay.setParameters(delayTime, delayMix);
    sample = delay.process(sample);

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

void AudioGenerator::setDelayTime(float seconds) {
    std::lock_guard<std::mutex> lock(paramMutex);
    delayTime = std::clamp(seconds, 0.1f, 2.0f);
}

void AudioGenerator::setDelayMix(float mix) {
    std::lock_guard<std::mutex> lock(paramMutex);
    delayMix = std::clamp(mix, 0.0f, 1.0f);
}

void AudioGenerator::setDelay(float time, float mix) {
    delay.setParameters(time, mix);
}
