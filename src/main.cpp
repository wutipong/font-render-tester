#include "io_util.hpp"
#include "main_scene.hpp"
#include <IconsForkAwesome.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <memory>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

static constexpr char imguiIni[] = "imgui.ini";
static constexpr char logfile[] = "log.txt";

static constexpr int windowMinimumWidth = 1280;
static constexpr int windowMinimumHeight = 720;

int main(int argc, char **argv) {
  const auto preferencePath = GetPreferencePath();

  constexpr auto maxLogFileSize = 5 * 1024 * 1024;
  constexpr auto maxLogFile = 3;

  const auto logFilePath = preferencePath / logfile;
  const auto logger = spdlog::rotating_logger_mt("logger", logFilePath.string(),
                                                 maxLogFileSize, maxLogFile);
  logger->flush_on(spdlog::level::info);
  spdlog::set_default_logger(logger);

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow(
      "font-render-tester",  windowMinimumWidth, windowMinimumHeight,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

  SDL_SetWindowMinimumSize(window, windowMinimumWidth, windowMinimumHeight);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

  const auto imguiIniPath = preferencePath / imguiIni;
  std::string imguiIniStr = imguiIniPath.string();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = imguiIniStr.c_str();

  io.Fonts->AddFontFromFileTTF("fonts/NotoSans-Regular.ttf", 20.0f);

  ImFontConfig config;
  config.MergeMode = true;

  ImWchar iconRangesFK[] = {ICON_MIN_FK, ICON_MAX_FK, 0};
  io.Fonts->AddFontFromFileTTF("fonts/forkawesome-webfont.ttf", 20.0f, &config,
                               iconRangesFK);
  io.Fonts->AddFontFromFileTTF("fonts/NotoSansJP-Regular.ttf", 20.0f, &config,
                               io.Fonts->GetGlyphRangesJapanese());
  io.Fonts->AddFontFromFileTTF("fonts/NotoSansThai-Regular.ttf", 20.0f, &config,
                               io.Fonts->GetGlyphRangesThai());
  io.Fonts->AddFontFromFileTTF("fonts/NotoSansKR-Regular.ttf", 20.0f, &config,
                               io.Fonts->GetGlyphRangesKorean());
  io.Fonts->AddFontFromFileTTF(
      "fonts/NotoSansSC-Regular.ttf", 20.0f, &config,
      io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

  io.Fonts->AddFontFromFileTTF("fonts/NotoSansTC-Regular.ttf", 20.0f, &config,
                               io.Fonts->GetGlyphRangesChineseFull());

  io.Fonts->Build();

  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  if (!SceneInit()) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Unable to initialize the new scene", window);

    return EXIT_FAILURE;
  };

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT)
        break;
    }

    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();

    ImGui::NewFrame();

    SceneDoUI();

    ImGui::EndFrame();
    ImGui::Render();

    SceneTick(renderer);

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(1);
  }

  SceneCleanUp();

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
