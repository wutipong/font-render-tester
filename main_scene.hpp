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
  struct ScriptPair {
    const char* name;
    const hb_script_t script;
  };
  static constexpr std::array<ScriptPair, 7> scripts = {
    ScriptPair{"Common", HB_SCRIPT_COMMON},
    ScriptPair{"Thai", HB_SCRIPT_THAI},
    ScriptPair{"Hiragana", HB_SCRIPT_HIRAGANA},
    ScriptPair{"Katakana", HB_SCRIPT_KATAKANA},
    ScriptPair{"Han", HB_SCRIPT_HAN},
    ScriptPair{"Hangul", HB_SCRIPT_HANGUL},
    ScriptPair{"Arabic", HB_SCRIPT_ARABIC},
  };

  struct DirectionPair {
    const char* name;
    const hb_direction_t direction;
  };
  static constexpr std::array<DirectionPair, 3> directions = {
    DirectionPair{"Left to Right", HB_DIRECTION_LTR,},
    DirectionPair{ "Right To Left",HB_DIRECTION_RTL, },
    DirectionPair{"Top to Bottom", HB_DIRECTION_TTB,},
  };

  struct LanguagePair {
    const char* name;
    const char* code;
  };
  static constexpr std::array<LanguagePair, 8> languages = {
    LanguagePair{ "None",                    "",      },
    LanguagePair{ "English US",              "en-US", },
    LanguagePair{ "Thai Thailand",           "th-TH", },
    LanguagePair{ "Japanese Japan",          "ja-JP", },
    LanguagePair{ "Korean Republic of Korea","ko-KR", },
    LanguagePair{ "Chinese China",           "zh-CN", },
    LanguagePair{ "Chinese Taiwan",          "zh-TW", },
    LanguagePair{ "Arabic Saudi Arabia",   "ar-SA",  },
  };
  // clang-format on

  int selectedScript = 0;
  int selectedDirection = 0;
  int selectedLanguage = 0;
};
