#include "context.hpp"
#include "io_util.hpp"
#include "main_scene.hpp"
#include <IconsForkAwesome.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
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

  Context ctx{};

  SDL_Init(SDL_INIT_VIDEO);

  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

  SDL_Window *window = SDL_CreateWindow(
      "font-render-tester", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      windowMinimumWidth, windowMinimumHeight,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  SDL_SetWindowMinimumSize(window, windowMinimumWidth, windowMinimumHeight);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

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

  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  if (!SceneInit(ctx)) {
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
    ctx.windowBound = {0};
    SDL_GetWindowSize(window, &ctx.windowBound.w, &ctx.windowBound.h);

    ImGui_ImplSDL2_NewFrame(window);
    ImGui_ImplSDLRenderer2_NewFrame();

    ImGui::NewFrame();

    SceneDoUI(ctx);

    ImGui::EndFrame();
    ImGui::Render();

    SceneTick(renderer, ctx);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
    SDL_Delay(1);
  }

  SceneCleanUp(ctx);

  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
