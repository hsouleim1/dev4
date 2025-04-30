#include "MainWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <thread>

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

    // Valeurs de l’interface
    bool osc1 = true, osc2 = false;
    int wave1 = 0, wave2 = 0;
    float offset1 = 0.f, offset2 = 0.f;
    float attack = 0.1f, release = 0.5f;
    float cutoff = 1000.f, resonance = 0.5f;
    float delayTime = 0.5f, delayMix = 0.5f;
    const char* waves[] = { "SINE", "SQUARE", "SAW" };

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

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
        ImGui::SameLine(150); ImGui::Text("OSC2 Waveform");
        ImGui::Combo("##wave2", &wave2, waves, IM_ARRAYSIZE(waves));
        ImGui::SameLine(400); ImGui::Text("OSC2 Frequency Offset");
        ImGui::SliderFloat("##offset2", &offset2, -5, 5, "%.2f");

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
        for (int i = 1; i <= 13; ++i) {
            ImGui::PushID(i);
            ImGui::Button(std::to_string(i).c_str(), ImVec2(40, 30));
            if (i < 13) ImGui::SameLine();
            ImGui::PopID();
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
