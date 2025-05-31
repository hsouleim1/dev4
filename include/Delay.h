#ifndef DELAY_H
#define DELAY_H

#include <vector>
#include <mutex>

class Delay {
public:
    Delay(int sampleRate, float maxDelaySeconds = 2.0f);
    float process(float inputSample);
    void setParameters(float delayTime, float mix);

private:
    std::vector<float> buffer;
    int writeIndex;
    int sampleRate;
    int maxSamples;

    float delayTime = 0.5f;
    float mix = 0.5f;

    std::mutex delayMutex;
};

#endif // DELAY_H
