#define SDL_ENABLE_OLD_NAMES  // Doit être défini avant l'include SDL
#include <SDL3/SDL.h>

#include "MainWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "AudioGenerator.h"
#include <thread>
#include <chrono>

constexpr float FRAMERATE = 60.0f;
constexpr std::chrono::duration<double, std::milli> FRAME_DURATION(1000.0f / FRAMERATE);

// === Initialisation SDL + ImGui ===
void MainWindow::init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Synthétiseur", 800, 600, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);  // V-Sync pour éviter le tearing

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

// === Boucle principale ===
void MainWindow::run() {
    bool running = true;
    SDL_Event event;

    // Paramètres audio initiaux
    bool osc1 = true, osc2 = false;
    int wave1 = 0;
    float offset1 = 0.f, offset2 = 0.f;
    float attack = 0.1f, release = 0.5f;
    float cutoff = 1000.f, resonance = 0.5f;
    float delayTime = 0.5f, delayMix = 0.5f;
    const char* waves[] = { "SINE", "SQUARE", "SAW" };

    bool noteActive = false;
    std::chrono::steady_clock::time_point noteStart;

    while (running) {
        // === Gestion des événements SDL ===
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
                bool isDown = event.type == SDL_EVENT_KEY_DOWN;
                int note = -1;

                SDL_Keycode key = event.key.key;
                switch (key) {
                    case SDLK_a: note = 0; break;
                    case SDLK_z: note = 1; break;
                    case SDLK_e: note = 2; break;
                    case SDLK_r: note = 3; break;
                    case SDLK_t: note = 4; break;
                    case SDLK_y: note = 5; break;
                    case SDLK_u: note = 6; break;
                    case SDLK_i: note = 7; break;
                    case SDLK_o: note = 8; break;
                    case SDLK_p: note = 9; break;
                    case SDLK_q: note = 10; break;
                    case SDLK_s: note = 11; break;
                    case SDLK_d: note = 12; break;
                }

                if (note != -1) {
                    if (isDown) {
                        AudioGenerator::getInstance().setNote(note);
                        AudioGenerator::getInstance().setKeyPressed(true);
                        noteStart = std::chrono::steady_clock::now();
                        noteActive = true;
                    } else {
                        AudioGenerator::getInstance().setKeyPressed(false);
                        noteActive = false;
                    }
                }
            }

            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // === Mise à jour des paramètres audio ===
        AudioGenerator::getInstance().setOsc1(osc1, (WaveType)wave1, offset1);
        AudioGenerator::getInstance().setOsc2(osc2, offset2);
        AudioGenerator::getInstance().setEnvelope(attack, release);
        AudioGenerator::getInstance().setCutoff(cutoff);
        AudioGenerator::getInstance().setResonance(resonance);
        AudioGenerator::getInstance().setDelayTime(delayTime);
        AudioGenerator::getInstance().setDelayMix(delayMix);

        // Arrêt automatique de la note après 0.3 secondes
        if (noteActive) {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - noteStart).count();
            if (elapsed > 0.3f) {
                AudioGenerator::getInstance().setKeyPressed(false);
                noteActive = false;
            }
        }

        // === Interface graphique ImGui ===
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(700, 520), ImGuiCond_Always);
        ImGui::Begin("Synthétiseur", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        // Oscillateur 1
        ImGui::Checkbox("OSC 1", &osc1);
        ImGui::Text("OSC1 Waveform");
        ImGui::Combo("##wave1", &wave1, waves, IM_ARRAYSIZE(waves));
        ImGui::Text("OSC1 Frequency Offset");
        ImGui::SliderFloat("##offset1", &offset1, -5.0f, 5.0f, "%.2f");

        // Oscillateur 2
        ImGui::Checkbox("OSC 2", &osc2);
        ImGui::Separator();

        // Enveloppe AR
        ImGui::Text("Attack");
        ImGui::SliderFloat("##attack", &attack, 0, 1, "%.2f");
        ImGui::Text("Release");
        ImGui::SliderFloat("##release", &release, 0, 2, "%.2f");

        // Filtre passe-bas
        ImGui::Text("Filter Cutoff");
        ImGui::SliderFloat("##cutoff", &cutoff, 20, 20000, "%.0f");
        ImGui::Text("Filter Resonance");
        ImGui::SliderFloat("##resonance", &resonance, 0, 1, "%.2f");

        // Delay
        ImGui::Text("Delay Time");
        ImGui::SliderFloat("##delayTime", &delayTime, 0.0f, 2.0f, "%.2f");
        ImGui::Text("Delay Mix");
        ImGui::SliderFloat("##delayMix", &delayMix, 0, 1, "%.2f");

        ImGui::Separator();

        // Clavier virtuel
        ImGui::Text("Clavier virtuel");
        for (int i = 0; i < 13; ++i) {
            ImGui::PushID(i);
            if (ImGui::Button(std::to_string(i + 1).c_str(), ImVec2(40, 30))) {
                AudioGenerator::getInstance().setKeyPressed(true);
                AudioGenerator::getInstance().setNote(i);
                noteStart = std::chrono::steady_clock::now();
                noteActive = true;
            }
            if (i < 12) ImGui::SameLine();
            ImGui::PopID();
        }

        ImGui::End();

        // === Rendu SDL ===
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 220, 220, 255, 255); // Couleur fond
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);

        // Limite le framerate
        std::this_thread::sleep_for(FRAME_DURATION);
    }
}
