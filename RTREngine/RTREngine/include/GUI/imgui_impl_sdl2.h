// ImGui SDL binding with OpenGL3 + shaders
// https://github.com/ocornut/imgui

#include <GUI\imgui.h>

struct SDL_Window;

bool        ImGui_ImplSDL2_Init    (SDL_Window* window);
void        ImGui_ImplSDL2_Shutdown();
bool ImGui_ImplSdl_NewFrame(SDL_Window *window);

// Use if you want to reset your rendering device without losing ImGui state.
void        ImGui_ImplSDL2_InvalidateDeviceObjects();
bool        ImGui_ImplSDL2_CreateDeviceObjects();

// SDL callbacks (installed by default if you enable 'install_callbacks' during initialization)
// Provided here if you want to chain callbacks.
// You can also handle inputs yourself and use those as a reference.
void        ImGui_ImplSDL2_KeyCallback        (int key, bool down);
void        ImGui_ImplSDL2_CharCallback       (unsigned int c);
