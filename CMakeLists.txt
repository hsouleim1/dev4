cmake_minimum_required(VERSION 3.30)
project(votre_projet)
set(CMAKE_CXX_STANDARD 23)

# Inclure les headers
include_directories(
        ${CMAKE_SOURCE_DIR}/libraries/imgui
        ${CMAKE_SOURCE_DIR}/libraries/imgui/backends
        ${CMAKE_SOURCE_DIR}/libraries/sdl/include
        ${CMAKE_SOURCE_DIR}/libraries/portaudio/include
)

# Répertoires où se trouvent les fichiers .lib / .a
link_directories(
        ${CMAKE_SOURCE_DIR}/libraries/sdl
        ${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/win32
)

# Fichiers sources
add_executable(votre_projet
        main.cpp
        MainWindow.cpp
        AudioGenerator.cpp
        Oscillator.cpp
        Filter.cpp
        libraries/imgui/imgui.cpp
        libraries/imgui/imgui_demo.cpp
        libraries/imgui/imgui_draw.cpp
        libraries/imgui/imgui_widgets.cpp
        libraries/imgui/imgui_tables.cpp
        libraries/imgui/backends/imgui_impl_sdl3.cpp
        libraries/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

# Lier les librairies dynamiques SDL + PortAudio
target_link_libraries(votre_projet PRIVATE
        SDL3
        portaudio_x64       # ✅ nom du .lib sans extension
)

# Copier automatiquement les DLL après compilation
add_custom_command(TARGET votre_projet POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libraries/sdl/SDL3.dll"
        "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/win32/portaudio_x64.dll"
        "$<TARGET_FILE_DIR:votre_projet>"
)
