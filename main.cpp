#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include "context.hpp"
#include "main_scene.hpp"
#include "scene.hpp"

static constexpr char imguiIni[] = "imgui.ini";
static constexpr char contextJson[] = "context.json";
static constexpr char logfile[] = "log.txt";

int main(int argc, char **argv) {
  auto preferencePath = std::filesystem::path(
      SDL_GetPrefPath("sleepyheads.info", "font-render-tester"));
  auto imguiIniPath = preferencePath / imguiIni;
  auto contextIniPath = preferencePath / contextJson;

  auto max_size = 5 * 1024 * 1024;
  auto max_files = 3;

  auto logFilePath = preferencePath / logfile;
  auto logger = spdlog::rotating_logger_mt("logger", logFilePath.string(),
                                           max_size, max_files);
  logger->flush_on(spdlog::level::info);
  spdlog::set_default_logger(logger);

  Context ctx{};
  LoadContext(ctx, contextIniPath);

  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window *window = SDL_CreateWindow(
      "font-render-tester", ctx.windowBound.x, ctx.windowBound.y,
      ctx.windowBound.w, ctx.windowBound.h,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
  SDL_SetWindowMinimumSize(window, MININUM_WIDTH, MINIMUM_HEIGHT);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

  std::string imguiIniStr = imguiIniPath.string();

  IMGUI_CHECKVERSION();
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

  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  if (!Scene::ChangeScene<MainScene>(ctx)) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Unable to initialize the new scene", window);

    return EXIT_FAILURE;
  };

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        break;
    }
    SDL_GetWindowSize(window, &ctx.windowBound.w, &ctx.windowBound.h);

    ImGui_ImplSDL2_NewFrame(window);
    ImGui_ImplSDLRenderer2_NewFrame();

    ImGui::NewFrame();

    Scene::Current()->DoUI(ctx);

    ImGui::EndFrame();
    ImGui::Render();

    Scene::TickCurrent(renderer, ctx);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
    SDL_Delay(1);
  }

  SaveContext(ctx, contextIniPath);

  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
