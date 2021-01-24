#include <SDL2/SDL.h>
#include <imgui.h>

#include "context.hpp"
#include "imgui_impl_sdl.h"
#include "imgui_sdl/imgui_sdl.h"
#include "main_scene.hpp"
#include "scene.hpp"

constexpr char imguiIni[] = "imgui.ini";
constexpr char contextJson[] = "context.json";

int main(int argc, char **argv) {
  Context ctx{};
  auto preferencePath = std::filesystem::path(
      SDL_GetPrefPath("sleepyheads.info", "font-render-tester"));
  auto imguiIniPath = preferencePath / imguiIni;
  auto contextIniPath = preferencePath / contextJson;

  LoadContext(ctx, contextIniPath);

  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window *window = SDL_CreateWindow(
      "font-render-tester", ctx.windowBound.x, ctx.windowBound.y, ctx.windowBound.w,
      ctx.windowBound.h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_SetWindowMinimumSize(window, WIDTH, HEIGHT);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

  std::string imguiIniStr = imguiIniPath.string();

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = imguiIniStr.c_str();

  io.Fonts->AddFontFromFileTTF("fonts/NotoSans-Regular.ttf", 20.0f);

  ImFontConfig config;
  config.MergeMode = true;

  io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJKjp-Regular.otf", 20.0f,
                               &config, io.Fonts->GetGlyphRangesJapanese());
  io.Fonts->AddFontFromFileTTF("fonts/NotoSansThai-Regular.ttf", 20.0f, &config,
                               io.Fonts->GetGlyphRangesThai());
  io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJKkr-Regular.otf", 20.0f,
                               &config, io.Fonts->GetGlyphRangesKorean());
  io.Fonts->AddFontFromFileTTF(
      "fonts/NotoSansCJKsc-Regular.otf", 20.0f, &config,
      io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

  io.Fonts->Build();

  ImGuiSDL::Initialize(renderer, ctx.windowBound.w, ctx.windowBound.h);
  ImGui_ImplSDL2_Init(window);

  Scene::ChangeScene<MainScene>(ctx);

  while (true) {
    ctx.renderer = renderer;
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        break;
    }
    SDL_GetWindowSize(window, &ctx.windowBound.w, &ctx.windowBound.h);

    SDL_SetRenderDrawColor(renderer, ctx.backgroundColor.r,
                           ctx.backgroundColor.g, ctx.backgroundColor.b,
                           ctx.backgroundColor.a);
    SDL_RenderClear(renderer);

    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    Scene::TickCurrent(ctx);

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
    SDL_Delay(1);
  }

  SaveContext(ctx, contextIniPath);

  ImGui_ImplSDL2_Shutdown();
  ImGuiSDL::Deinitialize();
  ImGui::DestroyContext();

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
