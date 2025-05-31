#ifndef ENVELOPE_H
#define ENVELOPE_H

class Envelope {
public:
    void setAttackRelease(float atk, float rel);
    float getAmplitude(bool keyDown);  // maintenant déplacée en .cpp

private:
    float attack = 0.1f;
    float release = 0.5f;
    float amplitude = 0.0f;
};

#endif
