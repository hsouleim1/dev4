#include "MainWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "AudioGenerator.h"
#include <thread>
#include <chrono>

constexpr float FRAMERATE = 60.0f;
constexpr std::chrono::duration<double, std::milli> FRAME_DURATION(1000.0f / FRAMERATE);

void MainWindow::init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Synthétiseur", 800, 600, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void MainWindow::run() {
    bool running = true;
    SDL_Event event;

    // UI
    bool osc1 = true, osc2 = false;
    int wave1 = 0;
    float offset1 = 0.f, offset2 = 0.f;
    float attack = 0.1f, release = 0.5f;
    float cutoff = 1000.f, resonance = 0.5f;
    float delayTime = 0.5f, delayMix = 0.5f;
    const char* waves[] = { "SINE", "SQUARE", "SAW" };

    // Timer pour maintenir la note
    bool noteActive = false;
    std::chrono::steady_clock::time_point noteStart;

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        // MAJ paramètres audio
        AudioGenerator::getInstance().setOsc1(osc1, (WaveType)wave1, offset1);
        AudioGenerator::getInstance().setOsc2(osc2, offset2);
        AudioGenerator::getInstance().setEnvelope(attack, release);

        // Arrêt automatique après 300 ms
        if (noteActive) {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - noteStart).count();
            if (elapsed > 0.3f) {
                AudioGenerator::getInstance().keyPressed = false;
                noteActive = false;
            }
        }

        // IMGUI
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(700, 520), ImGuiCond_Always);
        ImGui::Begin("Synthétiseur", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        ImGui::Checkbox("OSC 1", &osc1);
        ImGui::SameLine(150); ImGui::Text("OSC1 Waveform");
        ImGui::Combo("##wave1", &wave1, waves, IM_ARRAYSIZE(waves));
        ImGui::SameLine(400); ImGui::Text("OSC1 Frequency Offset");
        ImGui::SliderFloat("##offset1", &offset1, -5, 5, "%.2f");

        ImGui::Checkbox("OSC 2", &osc2);
        ImGui::SameLine(200); ImGui::Text("Type : SAW");
        ImGui::SliderFloat("OSC2 Freq Offset", &offset2, -5, 5, "%.2f");

        ImGui::Separator();

        ImGui::Text("Attack");
        ImGui::SliderFloat("##attack", &attack, 0, 1, "%.2f");
        ImGui::Text("Release");
        ImGui::SliderFloat("##release", &release, 0, 2, "%.2f");

        ImGui::Text("Filter Cutoff");
        ImGui::SliderFloat("##cutoff", &cutoff, 20, 20000, "%.0f");
        ImGui::Text("Filter Resonance");
        ImGui::SliderFloat("##resonance", &resonance, 0, 1, "%.2f");

        ImGui::Text("Delay Time");
        ImGui::SliderFloat("##delayTime", &delayTime, 0.1f, 2.0f, "%.2f");
        ImGui::Text("Delay Mix");
        ImGui::SliderFloat("##delayMix", &delayMix, 0, 1, "%.2f");

        ImGui::Separator();

        ImGui::Text("Clavier virtuel");

        const float noteFrequencies[13] = {
            261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f,
            392.00f, 415.30f, 440.00f, 466.16f, 493.88f, 523.25f
        };

        static std::chrono::steady_clock::time_point noteStart;
        static bool noteActive = false;

        for (int i = 0; i < 13; ++i) {
            ImGui::PushID(i);
            if (ImGui::Button(std::to_string(i + 1).c_str(), ImVec2(40, 30))) {
                AudioGenerator::getInstance().keyPressed = true;
                AudioGenerator::getInstance().setNote(i);
                noteStart = std::chrono::steady_clock::now();
                noteActive = true;
            }
            if (i < 12) ImGui::SameLine();
            ImGui::PopID();
        }

        // Extinction auto après 0.3s
        if (noteActive) {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - noteStart).count();
            if (elapsed > 0.3f) {
                AudioGenerator::getInstance().keyPressed = false;
                noteActive = false;
            }
        }

        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 220, 220, 255, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);

        std::this_thread::sleep_for(FRAME_DURATION);
    }
}
