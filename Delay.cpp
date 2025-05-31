#include "Delay.h"
#include <algorithm>

Delay::Delay(int sampleRate, float maxDelaySeconds)
    : sampleRate(sampleRate), writeIndex(0) {
    maxSamples = static_cast<int>(maxDelaySeconds * sampleRate);
    buffer.resize(maxSamples, 0.0f);
}

void Delay::setParameters(float delayTimeSeconds, float mixAmount) {
    std::lock_guard<std::mutex> lock(delayMutex);
    delayTime = std::clamp(delayTimeSeconds, 0.1f, 2.0f);
    mix = std::clamp(mixAmount, 0.0f, 1.0f);
}

float Delay::process(float inputSample) {
    std::lock_guard<std::mutex> lock(delayMutex);

    int delaySamples = static_cast<int>(delayTime * sampleRate);
    delaySamples = std::min(delaySamples, maxSamples - 1);
    int readIndex = (writeIndex - delaySamples + maxSamples) % maxSamples;

    float delayed = buffer[readIndex];

    float output = inputSample * (1.0f - mix) + delayed * mix;

    float feedbackDecay = 0.95f;
    buffer[writeIndex] = output * feedbackDecay;

    writeIndex = (writeIndex + 1) % maxSamples;

    return output;
}
