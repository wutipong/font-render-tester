#define SDL_MAIN_USE_CALLBACKS

#include "io_util.hpp"
#include "main_scene.hpp"
#include <IconsForkAwesome.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <memory>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

static constexpr char IMGUI_INI[] = "imgui.ini";
static constexpr char LOGFILE[] = "log.txt";

static constexpr int WINDOW_MINIMUM_WIDTH = 1280;
static constexpr int WINDOW_MINIMUM_HEIGHT = 720;

static constexpr size_t MAX_LOG_FILE_SIZE = 5 * 1024 * 1024;
static constexpr size_t MAX_LOG_FILE = 3;

namespace {
SDL_Renderer *renderer = nullptr;
SDL_Window *window = nullptr;
} // namespace

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  const auto logFilePath = GetPreferencePath() / LOGFILE;
  const auto logger = spdlog::rotating_logger_mt(
      "logger", logFilePath.string(), MAX_LOG_FILE_SIZE, MAX_LOG_FILE);
  logger->flush_on(spdlog::level::info);
  spdlog::set_default_logger(logger);

  SDL_Init(SDL_INIT_VIDEO);

  // High DPI is currently not working properly on MacOS
  window = SDL_CreateWindow(
      "font-render-tester", WINDOW_MINIMUM_WIDTH, WINDOW_MINIMUM_HEIGHT,
      SDL_WINDOW_RESIZABLE /* | SDL_WINDOW_HIGH_PIXEL_DENSITY*/);

  SDL_SetWindowMinimumSize(window, WINDOW_MINIMUM_WIDTH, WINDOW_MINIMUM_HEIGHT);

  renderer = SDL_CreateRenderer(window, nullptr);

  const auto imguiIniPath = GetPreferencePath() / IMGUI_INI;
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

    return SDL_APP_FAILURE;
  };

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  ImGui_ImplSDL3_NewFrame();
  ImGui_ImplSDLRenderer3_NewFrame();

  ImGui::NewFrame();

  SceneDoUI();

  ImGui::EndFrame();
  ImGui::Render();

  SceneTick(renderer);

  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  ImGui_ImplSDL3_ProcessEvent(event);
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  SceneCleanUp();

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
