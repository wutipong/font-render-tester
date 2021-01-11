#include <SDL2/SDL.h>
#include <imgui.h>

#include "context.hpp"
#include "imgui_impl_sdl.h"
#include "imgui_sdl/imgui_sdl.h"
#include "main_scene.hpp"
#include "scene.hpp"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *window = SDL_CreateWindow("Test Window", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                                        SDL_WINDOW_RESIZABLE);
  SDL_SetWindowMinimumSize(window, WIDTH, HEIGHT);
  SDL_Renderer *renderer;
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  io.Fonts->AddFontDefault();
  /*

  io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJKjp-Regular.otf", 16.0f, NULL,
                               io.Fonts->GetGlyphRangesJapanese());

  io.Fonts->AddFontFromFileTTF("fonts/NotoSansThai-Regular.ttf", 16.0f, NULL,
                               io.Fonts->GetGlyphRangesThai());

  io.Fonts->AddFontFromFileTTF("fonts/NotoSans-Regular.ttf", 16.0f, NULL);
  */

  ImGuiSDL::Initialize(renderer, WIDTH, HEIGHT);
  ImGui_ImplSDL2_Init(window);

  Context c{renderer};

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        break;
    }

    SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
    SDL_RenderClear(renderer);

    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    Scene::TickCurrent(c);

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
    SDL_Delay(1);
  }

  ImGui_ImplSDL2_Shutdown();
  ImGuiSDL::Deinitialize();
  ImGui::DestroyContext();

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
