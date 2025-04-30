cmake_minimum_required(VERSION 3.30)
project(projet_synth)

set(CMAKE_CXX_STANDARD 23)

# === Include headers
include_directories(
        "libraries/imgui/backends"
        "libraries/imgui"
        "libraries/sdl/include"
        "libraries/portaudio/include"
)

# === Lien vers les libs .a (nécessaire pour libSDL3.dll.a)
link_directories(
        ${CMAKE_SOURCE_DIR}/libraries/sdl/lib
)

# === Executable
add_executable(votre_projet
        main.cpp
        MainWindow.cpp

        libraries/imgui/imgui.cpp
        libraries/imgui/imgui_demo.cpp
        libraries/imgui/imgui_draw.cpp
        libraries/imgui/imgui_widgets.cpp
        libraries/imgui/imgui_tables.cpp
        libraries/imgui/backends/imgui_impl_sdl3.cpp
        libraries/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

if (APPLE)
    target_link_libraries(votre_projet PRIVATE
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/macos/SDL3.framework"
            "-framework CoreFoundation"
            "-framework AudioToolbox"
            "-framework CoreAudio"
            "-framework Carbon"
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/macos/libportaudio.a"
    )

elseif (WIN32)
    # ✅ On lie via le .a (nécessite que libSDL3.dll.a soit bien présent)
    target_link_libraries(votre_projet PRIVATE
            mingw32
            SDL3
    )

    # ✅ On copie SDL3.dll (pas .a) dans le build
    add_custom_command(TARGET votre_projet POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/libraries/sdl/bin/SDL3.dll"
            "$<TARGET_FILE_DIR:votre_projet>"
    )


elseif (UNIX)
    target_link_libraries(votre_projet PRIVATE
            "-ldl"
            "-ljack"
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/linux-x86_64/libSDL3.a"
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/linux-x86_64/libportaudio.a"
    )
endif()
