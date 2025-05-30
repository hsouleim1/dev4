#ifndef ENVELOPE_H
#define ENVELOPE_H

class Envelope {
public:
    void setAttackRelease(float atk, float rel) {
        attack = atk;
        release = rel;
    }

    float getAmplitude(bool keyDown) {
        float dt = 1.0f / 44100.0f; // SAMPLE_RATE
        if (keyDown) {
            amplitude += dt / (attack > 0.001f ? attack : 0.001f);
            if (amplitude > 1.0f) amplitude = 1.0f;
        } else {
            amplitude -= dt / (release > 0.001f ? release : 0.001f);
            if (amplitude < 0.0f) amplitude = 0.0f;
        }
        return amplitude;
    }

private:
    float attack = 0.1f;
    float release = 0.5f;
    float amplitude = 0.0f;
};

#endif
