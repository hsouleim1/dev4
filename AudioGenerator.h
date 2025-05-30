#ifndef AUDIOGENERATOR_H
#define AUDIOGENERATOR_H

#include <portaudio.h>
#include <mutex>
#include "Oscillator.h"
#include "Envelope.h"
#include "WaveType.h"
#include "Filter.h"


#include "Constants.h"


class AudioGenerator {
public:
    static AudioGenerator& getInstance();

    float generateSample(); // plus besoin de temps t
    void setOsc1(bool active, WaveType type, float offset);
    void setOsc2(bool active, float offset);
    void setEnvelope(float attack, float release);
    void setCutoff(float freq);
    void setResonance(float r);
    void setNote(int index);
    bool init();

    void setKeyPressed(bool pressed);
    bool isKeyPressed();

    void setFilter(float cutoff, float resonance);

private:
    AudioGenerator() = default;
    ~AudioGenerator();
    Filter filter;
    float cutoff = 1000.f;
    float resonance = 0.5f;

    static int paCallback(const void*, void*, unsigned long,
                          const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);

    PaStream* stream = nullptr;

    Oscillator osc1, osc2;
    Envelope envelope;

    bool osc1Active = true, osc2Active = false;
    float offset1 = 0.0f, offset2 = 0.0f;

    int noteIndex = -1;
    bool keyPressed = false;

    std::mutex paramMutex;
};

#endif
