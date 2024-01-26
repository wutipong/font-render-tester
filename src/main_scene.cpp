#include "main_scene.hpp"

#include "colors.hpp"
#include "font.hpp"
#include "io_util.hpp"
#include "settings.hpp"
#include "text_renderer.hpp"
#include "version.hpp"
#include <IconsForkAwesome.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>
#include <magic_enum_all.hpp>
#include <magic_enum_containers.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <utf8/cpp20.h>

#include <imfilebrowser.h>

namespace {
constexpr int toolbarWidth = 400;
constexpr int padding = 30;

constexpr std::string_view exampleText{
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non \n"
    "turpis justo. Etiam luctus vulputate ante ac congue. Nunc vitae \n"
    "ultricies turpis, eu mollis libero. Quisque eu faucibus neque. \n"
    "Aliquam risus urna, ullamcorper sit amet arcu id, feugiat semper \n"
    "dolor. Maecenas commodo turpis orci, vel laoreet felis placerat \n"
    "in. In nec metus tincidunt sem sagittis dapibus ut eget magna. \n"
    "Aenean efficitur felis sed metus mollis varius."};

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

const std::vector<ScriptPair> scripts{
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
  const std::string_view name;
  const std::string_view code;
};

const std::vector<LanguagePair> languages{
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

std::vector<std::filesystem::path>
ListFontFiles(const std::filesystem::path &path) {
  std::vector<std::filesystem::path> output;
  for (auto &p : std::filesystem::directory_iterator(path)) {
    if (!p.is_regular_file()) {
      continue;
    }

    auto entryPath = p.path();
    auto extension = entryPath.extension().string();

    static auto compare = [](const char &c1, const char &c2) -> bool {
      return std::tolower(c1) == std::tolower(c2);
    };

    if (!std::equal(extension.begin(), extension.end(), ".otf", compare) &&
        !std::equal(extension.begin(), extension.end(), ".ttf", compare)) {
      continue;
    }

    output.push_back(p);
  }

  return output;
}

void OnDirectorySelected(Context &ctx, const std::filesystem::path &path) {
  std::filesystem::path newPath = path;

  if (!std::filesystem::exists(newPath)) {
    newPath = std::filesystem::absolute("fonts");
  }
  fontDirPath = path.string();
  fontFilePaths = ListFontFiles(fontDirPath);
}

void RenderText(SDL_Renderer *renderer, bool isShaping, const char *language,
                hb_script_t script, TextDirection direction, Context &ctx) {
  if (!font.IsValid())
    return;

  std::string str(buffer.data());

  SDL_Color sdlColor = foregroundColor;

  if (!isShaping) {
    TextRenderNoShape(renderer, ctx, font, str, sdlColor);
    return;
  }

  switch (direction) {
  case TextDirection::LeftToRight:
    TextRenderLeftToRight(renderer, ctx, font, str, sdlColor, language, script);
    return;

  case TextDirection::TopToBottom:
    TextRenderTopToBottom(renderer, ctx, font, str, sdlColor, language, script);
    return;

#ifdef ENABLE_RTL
  case TextDirection::RightToLeft:
    TextRenderRightToLeft(renderer, ctx, font, str, sdlColor, language, script);
    return;
#endif
  }
};
} // namespace

bool SceneInit(Context &context) {
  if (!Font::Init())
    return false;

  auto [fontPath] = LoadSettings();
  fontDirPath = fontPath.string();

  dirChooser.SetTitle("Browse for font directory");
  OnDirectorySelected(context, fontDirPath);
  dirChooser.SetPwd(fontDirPath);

  std::copy(std::cbegin(exampleText), std::cend(exampleText), buffer.begin());

  return true;
}

void SceneTick(SDL_Renderer *renderer, Context &ctx) {
  SDL_Rect viewport = ctx.windowBound;

  viewport.x += padding;
  viewport.y += padding;
  viewport.w -= (toolbarWidth + padding * 2);
  viewport.h -= 2 * padding;

  SDL_RenderSetViewport(renderer, &viewport);

  SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                         backgroundColor.b, backgroundColor.a);
  SDL_RenderClear(renderer);

  font.SetFontSize(fontSize);

  auto language = languages[selectedLanguage].code;
  auto script = scripts[selectedScript].script;

  RenderText(renderer, isShaping, language.data(), script, selectedDirection,
             ctx);

  SDL_RenderGetViewport(renderer, nullptr);
}

void SceneCleanUp(Context &context) {
  Font::CleanUp();
  SaveSettings({.fontPath = fontDirPath});
}

void SceneDoUI(Context &context) {
  int newSelected = selectedFontIndex;
  bool showAbout = false;
  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("File##menu")) {
      if (ImGui::MenuItem("Change font directory##file-menu")) {
        dirChooser.Open();
      }

      if (ImGui::MenuItem("Re-scan font directory##file-menu")) {
        fontFilePaths = ListFontFiles(fontDirPath);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        SDL_Event ev{
            .quit{
                .type = SDL_QUIT,
                .timestamp = SDL_GetTicks(),
            },
        };
        SDL_PushEvent(&ev);
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View##menu")) {
      ImGui::MenuItem("Text editor##view-menu", "", &context.showTextEditor);
      ImGui::MenuItem("Debug##view-menu", "", &context.debug);

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help##menu")) {
      if (ImGui::MenuItem("About##help-menu")) {
        showAbout = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (showAbout) {
    ImGui::OpenPopup("About##dialog");
    showAbout = false;
  }

  if (ImGui::BeginViewportSideBar(
          "status bar", nullptr, ImGuiDir_Down, ImGui::GetFrameHeight(),
          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
              ImGuiWindowFlags_MenuBar)) {
    if (ImGui::BeginMenuBar()) {
      ImGui::LabelText(ICON_FK_FOLDER " Font Directory", fontDirPath.c_str());
      ImGui::EndMenuBar();
    }
  }
  ImGui::End();

  if (ImGui::BeginViewportSideBar("toolbar", nullptr, ImGuiDir_Right,
                                  toolbarWidth,
                                  ImGuiWindowFlags_NoSavedSettings)) {

    if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_DefaultOpen)) {
      constexpr int noFontSelected = -1;
      auto currentFile =
          selectedFontIndex == noFontSelected
              ? "<None>"
              : fontFilePaths[selectedFontIndex].filename().string();

      if (ImGui::BeginCombo("Font file", currentFile.c_str())) {
        for (int i = 0; i < fontFilePaths.size(); i++) {
          auto isSelected = i == selectedFontIndex;
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }

          if (ImGui::Selectable(fontFilePaths[i].filename().string().c_str(),
                                isSelected)) {
            newSelected = i;
          }
        }

        ImGui::EndCombo();
      }

      ImGui::LabelText("Family name", font.GetFamilyName().c_str());
      ImGui::LabelText("Sub-family name", font.GetSubFamilyName().c_str());
    }

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SliderInt("Font Size", &fontSize, 0, 128);

      ImGui::SeparatorText("Variations");
      ImGui::BeginDisabled(!font.IsVariableFont());

      bool axisChanged = false;

      constexpr magic_enum::containers::array<VariationAxis, const char *>
          axisLabel = {
              "Italic##axis", "Optical size##axis", "Slant##axis",
              "Weight##axis", "Width##axis",
          };

      magic_enum::enum_for_each<VariationAxis>(
          [&axisChanged, &axisLabel](const VariationAxis &axis) {
            if (axisLimits[axis].has_value()) {
              [[maybe_unused]] auto [min, max, _] = *axisLimits[axis];
              axisChanged |= ImGui::DragFloat(axisLabel[axis], &axisValue[axis],
                                              1.0f, min, max);
            } else {
              ImGui::LabelText(axisLabel[axis], "N/A");
            }
          });

      if (axisChanged) {
        font.SetVariationValues(axisValue);
      }

      ImGui::EndDisabled();

      ImGui::SeparatorText("Font metrics");

      ImGui::LabelText("Ascend", "%.3f", font.Ascend());
      ImGui::LabelText("Descend", "%.3f", font.Descend());
      ImGui::LabelText("Line gap", "%.3f", font.LineGap());
      ImGui::LabelText("Line height", "%.3f", font.LineHeight());

      ImGui::SeparatorText("OpenType text shaping");
      ImGui::Checkbox("Enable##Shape Text", &isShaping);

      ImGui::BeginDisabled(!isShaping);
      if (ImGui::BeginCombo("Language",
                            languages[selectedLanguage].name.data())) {
        for (size_t i = 0; i < languages.size(); i++) {
          if (ImGui::Selectable(languages[i].name.data(),
                                i == selectedLanguage)) {
            selectedLanguage = i;
          }
        }
        ImGui::EndCombo();
      }

      if (ImGui::BeginCombo("Script", scripts[selectedScript].name)) {
        for (size_t i = 0; i < scripts.size(); i++) {
          if (ImGui::Selectable(scripts[i].name, i == selectedScript)) {
            selectedScript = i;
          }
        }
        ImGui::EndCombo();
      }

      constexpr magic_enum::containers::array<TextDirection, const char *>
          directionLabels{
              "Left to right",
              "Top to bottom",
#ifdef ENABLE_RTL
              "Right to left",
#endif
          };

      if (ImGui::BeginCombo("Direction", directionLabels[selectedDirection])) {
        magic_enum::enum_for_each<TextDirection>(
            [&directionLabels](const auto &dir) {
              if (ImGui::Selectable(directionLabels[dir],
                                    dir == selectedDirection)) {
                selectedDirection = dir;
              }
            });

        ImGui::EndCombo();
      }
      ImGui::EndDisabled();
    }

    if (ImGui::CollapsingHeader("Draw colors")) {
      auto f4Foreground = SDLColorToFloat4(foregroundColor);
      if (ImGui::ColorPicker3("Foreground##color", f4Foreground.data(),
                              ImGuiColorEditFlags_InputRGB)) {
        foregroundColor = Float4ToSDLColor(f4Foreground);
      };
      if (ImGui::Button("Reset##foreground color")) {
        foregroundColor = defaultForegroundColor;
      }

      auto f4Background = SDLColorToFloat4(backgroundColor);
      if (ImGui::ColorPicker3("Background##color", f4Background.data(),
                              ImGuiColorEditFlags_InputRGB)) {
        backgroundColor = Float4ToSDLColor(f4Background);
      };
      if (ImGui::Button("Reset##background color")) {
        backgroundColor = defaultBackgroundColor;
      }
    }
  }
  ImGui::End();

  if (context.showTextEditor) {
    if (ImGui::Begin("Input text", &context.showTextEditor)) {
      ImGui::InputTextMultiline("##InputText", buffer.data(), buffer.size());
    }
    ImGui::End();
  }

  if (context.debug) {
    if (ImGui::Begin("Debug", &context.debug)) {
      if (ImGui::CollapsingHeader("Features", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Baseline", &context.debugBaseline);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Baseline", SDLColorToImVec4(debugBaselineColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Caret Positions", &context.debugCaret);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Caret Positions", SDLColorToImVec4(debugCaretColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Glyph Bound", &context.debugGlyphBound);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Glyph Bound", SDLColorToImVec4(debugGlyphBoundColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Ascend", &context.debugAscend);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Ascend", SDLColorToImVec4(debugAscendColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Descend", &context.debugDescend);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Descend", SDLColorToImVec4(debugDescendColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);
      }
    }
    ImGui::End();
  }

  dirChooser.Display();
  if (dirChooser.HasSelected()) {
    OnDirectorySelected(context, dirChooser.GetSelected());
    dirChooser.ClearSelected();
    newSelected = -1;
  }

  if (newSelected != selectedFontIndex) {
    if (newSelected == -1) {
      font = Font();
      selectedFontIndex = newSelected;
    } else {
      Font newFont;
      if (!newFont.LoadFile(fontFilePaths[newSelected].string())) {
        ImGui::OpenPopup("InvalidFont");
      } else {
        font = newFont;
        axisLimits = font.GetAxisInfos();

        magic_enum::enum_for_each<VariationAxis>([](const VariationAxis &axis) {
          if (!axisLimits[axis].has_value())
            return;

          axisValue[axis] = axisLimits[axis]->defaultValue;
        });

        selectedFontIndex = newSelected;
      }
    }
  }

  if (ImGui::BeginPopupModal("InvalidFont")) {
    ImGui::Text("Invalid Font File");
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopupModal("About##dialog")) {
    ImGui::Text("Font-Render-Tester %d.%d.%d", majorVersion, minorVersion,
                patchVersion);

    ImGui::Text("http://github.com/wutipong/font-render-tester");

    ImGui::SeparatorText("Fonts");
    ImGui::Text("Fork Awesome");
    ImGui::Text("Noto Sans Family");

    ImGui::SeparatorText("Dependencies");

    ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
    ImGui::Text("FreeType %d.%d.%d", FREETYPE_MAJOR, FREETYPE_MINOR,
                FREETYPE_PATCH);
    ImGui::Text("Harfbuzz %s", HB_VERSION_STRING);
    ImGui::Text("imgui-filebrowser");
    ImGui::Text("Magic-Enum %d.%d.%d", MAGIC_ENUM_VERSION_MAJOR,
                MAGIC_ENUM_VERSION_MINOR, MAGIC_ENUM_VERSION_PATCH);
    ImGui::Text("NLOHMANM-JSON %d.%d.%d", NLOHMANN_JSON_VERSION_MAJOR,
                NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
    ImGui::Text("SDL %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION,
                SDL_PATCHLEVEL);
    ImGui::Text("spdlog %d.%d.%d", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR,
                SPDLOG_VER_PATCH);
    ImGui::Text("UTF8-CPP");

    ImGui::Separator();

    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}