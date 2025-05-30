#include "MainWindow.h"
#include "AudioGenerator.h"
#include <portaudio.h>
#include <iostream>

static int audioCallback(const void*, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
                         void*) {
    float* out = static_cast<float*>(outputBuffer);
    static float time = 0.0f;
    const float dt = 1.0f / 44100.0f;  // tu peux remplacer par une constante si tu veux

    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float sample = AudioGenerator::getInstance().generateSample(time);
        time += dt;
        out[2 * i] = sample;
        out[2 * i + 1] = sample;
    }

    return paContinue;
}

int main() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Erreur PortAudio : " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream,
                               0, 2,            // 0 entrées, 2 sorties (stéréo)
                               paFloat32,
                               44100,
                               256,
                               audioCallback,
                               nullptr);

    if (err != paNoError) {
        std::cerr << "Erreur ouverture stream : " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    Pa_StartStream(stream);

    MainWindow mainWindow;
    mainWindow.init();
    mainWindow.run();

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
