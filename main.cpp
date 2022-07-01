#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include "context.hpp"
#include "main_scene.hpp"
#include "test_scene.hpp"
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

  const char *glsl_version = "#version 450";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_Window *window = SDL_CreateWindow(
      "font-render-tester", ctx.windowBound.x, ctx.windowBound.y,
      ctx.windowBound.w, ctx.windowBound.h,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
  SDL_SetWindowMinimumSize(window, WIDTH, HEIGHT);

  std::string imguiIniStr = imguiIniPath.string();

  SDL_GLContext glCtx = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, glCtx);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  gl3wInit();

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

  ImGui_ImplSDL2_InitForOpenGL(window, glCtx);
  ImGui_ImplOpenGL3_Init(glsl_version);

  Scene::ChangeScene<MainScene>(ctx);

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        break;
    }
    SDL_GetWindowSize(window, &ctx.windowBound.w, &ctx.windowBound.h);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    Scene::Current()->DoUI(ctx);

    ImGui::EndFrame();
    ImGui::Render();

    Scene::Current()->Tick(ctx);

    glViewport(0, 0, ctx.windowBound.w, ctx.windowBound.h);
    glDisable(GL_SCISSOR_TEST);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
    SDL_Delay(1);
  }

  SaveContext(ctx, contextIniPath);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(glCtx);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
