#pragma once

#include "colors.hpp"
#include "font.hpp"
#include "scene.hpp"
#include "text_renderer.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <filesystem>
#include <imgui.h>
#include <magic_enum_containers.hpp>
#include <string>

#include <imfilebrowser.h>

class MainScene : public Scene {
public:
  virtual bool Init(Context &context) override;
  virtual void Tick(SDL_Renderer *renderer, Context &ctx) override;
  virtual void CleanUp(Context &context) override;
  virtual void DoUI(Context &context) override;

private:
  void OnDirectorySelected(Context &context, const std::filesystem::path &path);

  static std::vector<std::filesystem::path>
  ListFontFiles(const std::filesystem::path &path);

  void RenderText(SDL_Renderer *renderer, Context &ctx);

  static const inline std::string exampleText =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non \n"
      "turpis justo. Etiam luctus vulputate ante ac congue. Nunc vitae \n"
      "ultricies turpis, eu mollis libero. Quisque eu faucibus neque. \n"
      "Aliquam risus urna, ullamcorper sit amet arcu id, feugiat semper \n"
      "dolor. Maecenas commodo turpis orci, vel laoreet felis placerat \n"
      "in. In nec metus tincidunt sem sagittis dapibus ut eget magna. \n"
      "Aenean efficitur felis sed metus mollis varius.";

  std::array<char, 4096> buffer = {0};

  SDL_Color foregroundColor = defaultForegroundColor;
  SDL_Color backgroundColor = defaultBackgroundColor;

  int fontSize = 64;
  bool isShaping = false;

  int selectedFontIndex = -1;
  std::vector<std::filesystem::path> fontFilePaths;

  ImGui::FileBrowser dirChooser{ImGuiFileBrowserFlags_SelectDirectory};
  std::string fontDirPath{std::filesystem::absolute("fonts").string()};

  Font font{};

  struct ScriptPair {
    const char *name;
    const hb_script_t script;
  };

  static inline std::vector<ScriptPair> scripts{
      ScriptPair{"Common", HB_SCRIPT_COMMON},
      ScriptPair{"Thai", HB_SCRIPT_THAI},
      ScriptPair{"Hiragana", HB_SCRIPT_HIRAGANA},
      ScriptPair{"Katakana", HB_SCRIPT_KATAKANA},
      ScriptPair{"Han", HB_SCRIPT_HAN},
      ScriptPair{"Hangul", HB_SCRIPT_HANGUL},
#ifdef ENABLE_RTL
      ScriptPair{"Arabic", HB_SCRIPT_ARABIC},
#endif
  };

  struct LanguagePair {
    const char *name;
    const char *code;
  };

  static inline std::vector<LanguagePair> languages{
      LanguagePair{
          "None",
          "",
      },
      LanguagePair{
          "English US",
          "en-US",
      },
      LanguagePair{
          "Thai Thailand",
          "th-TH",
      },
      LanguagePair{
          "Japanese Japan",
          "ja-JP",
      },
      LanguagePair{
          "Korean Republic of Korea",
          "ko-KR",
      },
      LanguagePair{
          "Chinese China",
          "zh-CN",
      },
      LanguagePair{
          "Chinese Taiwan",
          "zh-TW",
      },
#ifdef ENABLE_RTL
      LanguagePair{
          "Arabic Saudi Arabia",
          "ar-SA",
      },
#endif
  };

  int selectedScript = 0;
  int selectedLanguage = 0;

  TextDirection selectedDirection{TextDirection::LeftToRight};

  magic_enum::containers::array<VariationAxis, float> axisValue;
  magic_enum::containers::array<VariationAxis, std::optional<AxisInfo>>
      axisLimits;
};
