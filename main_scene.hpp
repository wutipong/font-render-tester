#pragma once

#include "scene.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <filesystem>
#include <stb_truetype.h>
#include <string>

#include "imgui.h"

#include "imgui-filebrowser/imfilebrowser.h"

#include "font.hpp"

#include "text_renderer.hpp"

class MainScene : public Scene {
public:
  virtual bool Init(const Context &context);
  virtual void Tick(const Context &context);
  virtual void Cleanup(const Context &context);

  void OnDirectorySelected(const std::filesystem::path &path);

  static std::vector<std::filesystem::path>
  ListFontFiles(const std::string &path);

private:
  std::array<char, 4096> buffer = {0};
  SDL_Color color = {0, 0, 0, 255};
  int fontSize = 64;
  bool isShape = false;

  std::string currentDirectory = "<none>";

  int selectedFontIndex = -1;
  std::vector<std::filesystem::path> fontPaths;

  std::vector<unsigned char> fontData;

  ImGui::FileBrowser dirChooser{ImGuiFileBrowserFlags_SelectDirectory};

  Font font{};

  static constexpr std::array<hb_script_t, 6> scripts = {
      HB_SCRIPT_COMMON,   HB_SCRIPT_THAI, HB_SCRIPT_HIRAGANA,
      HB_SCRIPT_KATAKANA, HB_SCRIPT_HAN,  HB_SCRIPT_HANGUL};

  static constexpr std::array<char *, 6> scriptStrs = {
      "Common", "Thai", "Hiragana", "Katakana", "Han", "Hangul"};

  int currentScriptIndex = 0;

  TextRendererEnum textRender = TextRendererEnum::None;
};
