#ifndef FILTER_H
#define FILTER_H

class Filter {
public:
    void setCutoff(float cutoff);
    void setResonance(float resonance);
    float process(float input);

private:
    float cutoff = 1000.0f;
    float resonance = 0.5f;

    float buf0 = 0.0f;
    float buf1 = 0.0f;

    float feedback = 0.0f;
};

#endif
