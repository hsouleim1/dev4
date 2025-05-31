#include "MainWindow.h"
#include "AudioGenerator.h"
#include <../libraries/portaudio/include/portaudio.h>
#include <iostream>
#include "Constants.h"

static int audioCallback(const void*, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
                         void*) {
    float* out = static_cast<float*>(outputBuffer);
    static float time = 0.0f;
    const float dt = 1.0f / SAMPLE_RATE;  // tu peux remplacer par une constante si tu veux

    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float sample = AudioGenerator::getInstance().generateSample();
        time += dt;
        out[2 * i] = sample; // Canal gauche
        out[2 * i + 1] = sample; // Canal droit
    }

    return paContinue;
}

int main() {

    // === Initialisation de PortAudio ===

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Erreur PortAudio : " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    PaStream* stream; // Lancer le flux audio

    // === Lancement de la fenêtre principale ===
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

    Pa_StartStream(stream); // Lancer le flux audio

    // === Lancement de la fenêtre principale ===
    MainWindow mainWindow;
    mainWindow.init();
    mainWindow.run();

    // === Nettoyage ===
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
