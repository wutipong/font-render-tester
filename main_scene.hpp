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

class MainScene : public Scene {
public:
  virtual bool Init(Context &context);
  virtual void Tick(Context &context);
  virtual void Cleanup(Context &context);

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

  ImGui::FileBrowser dirChooser{ImGuiFileBrowserFlags_SelectDirectory, };

  Font font{};

  static constexpr std::array<hb_script_t, 6> scripts = {
      HB_SCRIPT_COMMON,   HB_SCRIPT_THAI, HB_SCRIPT_HIRAGANA,
      HB_SCRIPT_KATAKANA, HB_SCRIPT_HAN,  HB_SCRIPT_HANGUL/*,
      HB_SCRIPT_ARABIC*/};

  static constexpr std::array<char *, 6> scriptStrs = {
      "Common", "Thai", "Hiragana", "Katakana", "Han", "Hangul" /*, "Arabic"*/};

  static constexpr std::array<hb_direction_t, 2> directions = {
      HB_DIRECTION_LTR /*, HB_DIRECTION_RTL*/, HB_DIRECTION_TTB};

  static constexpr std::array<char *, 2> directionStrs = {
      "Left to Right" /*, "Right To Left"*/, "Top to Bottom"};

  int selectedScript = 0;
  int selectedDirection = 0;
};
