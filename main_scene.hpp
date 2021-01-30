#pragma once

#include "scene.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <filesystem>
#include <string>

#include "imgui.h"

#include "imgui-filebrowser/imfilebrowser.h"

#include "font.hpp"

class MainScene : public Scene {
public:
  virtual bool Init(Context &context) override;
  virtual void Tick(Context &context) override;
  virtual void Cleanup(Context &context) override;
  virtual void DoUI(Context &context) override;

  void OnDirectorySelected(Context &context, const std::filesystem::path &path);

  static std::vector<std::filesystem::path>
  ListFontFiles(const std::filesystem::path &path);

private:
  std::array<char, 4096> buffer = {0};
  glm::vec4 color = {0, 0, 0, 1.0f};
  int fontSize = 64;
  bool isShape = false;

  int selectedFontIndex = -1;
  std::vector<std::filesystem::path> fontPaths;

  std::vector<unsigned char> fontData;

  ImGui::FileBrowser dirChooser{
      ImGuiFileBrowserFlags_SelectDirectory,
  };

  Font font{};

  // clang-format off
  static constexpr std::array<std::pair<const char *, const hb_script_t>, 6> scripts = {
    std::pair<const char *, const hb_script_t>{"Common", HB_SCRIPT_COMMON},
    std::pair<const char *, const hb_script_t>{"Thai", HB_SCRIPT_THAI},
    std::pair<const char *, const hb_script_t>{"Hiragana", HB_SCRIPT_HIRAGANA},
    std::pair<const char *, const hb_script_t>{"Katakana", HB_SCRIPT_KATAKANA},
    std::pair<const char *, const hb_script_t>{"Han", HB_SCRIPT_HAN},
    std::pair<const char *, const hb_script_t>{"Hangul", HB_SCRIPT_HANGUL},
    // std::pair<const char*, hb_script_t>{"Arabic" HB_SCRIPT_ARABIC},
  };

  static constexpr std::array<std::pair<const char *, const hb_direction_t>, 2> directions = {
    std::pair<const char *, const hb_direction_t>{"Left to Right", HB_DIRECTION_LTR,},
    // std::pair<const char*, const hb_direction_t>{ "Right To Left",HB_DIRECTION_RTL, },
    std::pair<const char *, const hb_direction_t>{"Top to Bottom", HB_DIRECTION_TTB,},
  };

  static constexpr std::array< std::pair<const char *, const char*>, 7> languages = {
    std::pair<const char*, const char*>{ "None",                    "",      },
    std::pair<const char*, const char*>{ "English US",              "en-US", },
    std::pair<const char*, const char*>{ "Thai Thailand",           "th-TH", },
    std::pair<const char*, const char*>{ "Japanese Japan",          "ja-JP", },
    std::pair<const char*, const char*>{ "Korean Republic of Korea","ko-KR", },
    std::pair<const char*, const char*>{ "Chinese China",           "zh-CN", },
    std::pair<const char*, const char*>{ "Chinese Taiwan",          "zh-TW", },
    //std::pair<const char *, const char*>{ "Arabic Saudi Arabia", "ar-SA",  },
  };
  // clang-format on

  int selectedScript = 0;
  int selectedDirection = 0;
  int selectedLanguage = 0;
};
