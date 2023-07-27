#include "main_scene.hpp"

#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <ranges>

#include "imgui-filebrowser/imfilebrowser.h"
#include "text_renderer.hpp"

namespace MainScene {
std::array<char, 4096> buffer = {0};
float color[3];
int fontSize = 64;
bool isShape = false;

int selectedFontIndex = -1;
std::vector<std::filesystem::path> fontPaths;
std::vector<unsigned char> fontData;

ImGui::FileBrowser dirChooser{
    ImGuiFileBrowserFlags_SelectDirectory,
};

Font font{};

struct ScriptPair {
  const char *name;
  const hb_script_t script;
};

constexpr std::array<ScriptPair, 7> scripts = {
    ScriptPair{"Common", HB_SCRIPT_COMMON},
    ScriptPair{"Thai", HB_SCRIPT_THAI},
    ScriptPair{"Hiragana", HB_SCRIPT_HIRAGANA},
    ScriptPair{"Katakana", HB_SCRIPT_KATAKANA},
    ScriptPair{"Han", HB_SCRIPT_HAN},
    ScriptPair{"Hangul", HB_SCRIPT_HANGUL},
    ScriptPair{"Arabic", HB_SCRIPT_ARABIC},
};

struct DirectionPair {
  const char *name;
  const hb_direction_t direction;
};

constexpr std::array<DirectionPair, 3> directions = {
    DirectionPair{"Left to Right", HB_DIRECTION_LTR},
    DirectionPair{"Right To Left", HB_DIRECTION_RTL},
    DirectionPair{"Top to Bottom", HB_DIRECTION_TTB},
};

struct LanguagePair {
  const char *name;
  const char *code;
};

constexpr std::array<LanguagePair, 8> languages = {
    LanguagePair{"None", ""},
    LanguagePair{"English US", "en-US"},
    LanguagePair{"Thai Thailand", "th-TH"},
    LanguagePair{"Japanese Japan", "ja-JP"},
    LanguagePair{"Korean Republic of Korea", "ko-KR"},
    LanguagePair{"Chinese China", "zh-CN"},
    LanguagePair{"Chinese Taiwan", "zh-TW"},
    LanguagePair{"Arabic Saudi Arabia", "ar-SA"},
};

int selectedScript = 0;
int selectedDirection = 0;
int selectedLanguage = 0;

bool showDebug = false;
bool showAdjustments = false;

std::array<float, 4> inline SDLColorToF4(const SDL_Color &color) {
  return {
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f,
  };
}

std::vector<std::filesystem::path>
ListFontFiles(const std::filesystem::path &path);

void OnDirectorySelected(Context &ctx, const std::filesystem::path &path);
} // namespace MainScene

bool MainScene::Init(Context &context) {
  if (!Font::Init())
    return false;

  dirChooser.SetTitle("Browse for font directory");
  OnDirectorySelected(context, context.fontPath);
  dirChooser.SetPwd(context.fontPath);

  return true;
}

void MainScene::Tick(Context &ctx) {

  SDL_SetRenderDrawColor(ctx.renderer, ctx.backgroundColor.r,
                         ctx.backgroundColor.g, ctx.backgroundColor.b,
                         ctx.backgroundColor.a);
  SDL_RenderClear(ctx.renderer);

  auto textRender = TextRenderEnum::NoShape;
  if (isShape) {
    switch (directions[selectedDirection].direction) {
    case HB_DIRECTION_LTR:
      textRender = TextRenderEnum::LeftToRight;
      break;
    case HB_DIRECTION_TTB:
      textRender = TextRenderEnum::TopToBottom;
      break;
    case HB_DIRECTION_RTL:
      textRender = TextRenderEnum::RightToLeft;
      break;
    }
  }

  font.SetTextRenderer(textRender);

  font.SetFontSize(fontSize);

  SDL_Color sdlColor = {
      static_cast<uint8_t>(color[0] * 255.0f),
      static_cast<uint8_t>(color[1] * 255.0f),
      static_cast<uint8_t>(color[2] * 255.0f),
      0xFF,
  };

  font.RenderText(ctx, std::string(buffer.data()), sdlColor,
                  languages[selectedLanguage].code,
                  scripts[selectedScript].script);
}

void MainScene::Cleanup(Context &context) { Font::CleanUp(); }

void MainScene::DoUI(Context &context) {
  int newSelected = selectedFontIndex;

  ImGui::Begin("Menu");
  {
    ImGui::BeginDisabled(showDebug);
    if (ImGui::Button("Debug")) {
      showDebug = true;
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(showAdjustments);
    if (ImGui::Button("Adjustments")) {
      showAdjustments = true;
    }
    ImGui::EndDisabled();

    if (ImGui::CollapsingHeader("Font Directory")) {
      ImGui::Text(context.fontPath.c_str());
      if (ImGui::Button("Browse")) {
        dirChooser.Open();
      }
      if (ImGui::Button("Re-scan")) {
        fontPaths = ListFontFiles(context.fontPath);
      }
    }

    if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto currentFile = selectedFontIndex == -1
                             ? "<None>"
                             : fontPaths[selectedFontIndex].filename().string();

      if (ImGui::BeginCombo("Font File", currentFile.c_str())) {
        for (int i = 0; i < fontPaths.size(); i++) {
          auto isSelected = i == selectedFontIndex;
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }

          if (ImGui::Selectable(fontPaths[i].filename().string().c_str(),
                                isSelected)) {
            newSelected = i;
          }
        }

        ImGui::EndCombo();
      }

      auto familyName = font.GetFamilyName();
      auto subFamily = font.GetSubFamilyName();

      ImGui::InputText("Family Name", familyName.data(), familyName.size(),
                       ImGuiInputTextFlags_ReadOnly);

      ImGui::InputText("Sub Family Name", subFamily.data(), subFamily.size(),
                       ImGuiInputTextFlags_ReadOnly);
    }

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SliderInt("Font Size", &fontSize, 0, 128);
      if (ImGui::CollapsingHeader("Font Metrics", ImGuiTreeNodeFlags_None)) {
        auto ascend = font.Ascend();
        auto descend = font.Descend();
        auto lineGap = font.LineGap();
        auto lineHeight = font.LineHeight();

        ImGui::InputFloat("Ascend", &ascend, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Descend", &descend, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Line Gap", &lineGap, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Line Height", &lineHeight,
                          ImGuiInputTextFlags_ReadOnly);
      }
      ImGui::Checkbox("Shape Text", &isShape);

      if (isShape) {
        if (ImGui::BeginCombo("Language", languages[selectedLanguage].name)) {
          for (size_t i = 0; i < languages.size(); i++) {
            if (ImGui::Selectable(languages[i].name, i == selectedLanguage)) {
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
        if (ImGui::BeginCombo("Direction",
                              directions[selectedDirection].name)) {
          for (int i = 0; i < directions.size(); i++) {
            if (ImGui::Selectable(directions[i].name, i == selectedDirection)) {
              selectedDirection = i;
            }
          }
          ImGui::EndCombo();
        }
      }

      ImGui::ColorPicker3("color", color, ImGuiColorEditFlags_InputRGB);
    }
  }
  ImGui::End();

  ImGui::Begin("Input Text");
  { ImGui::InputTextMultiline("##InputText", buffer.data(), buffer.size()); }
  ImGui::End();

  if (showDebug) {
    ImGui::Begin("Debug", &showDebug);

    ImGui::Checkbox("Enabled", &context.debug);
    if (context.debug) {
      if (ImGui::CollapsingHeader("Features")) {
        auto debugGlyphBoundColor = SDLColorToF4(context.debugGlyphBoundColor);
        auto debugAscendColor = SDLColorToF4(context.debugAscendColor);
        auto debugDescendColor = SDLColorToF4(context.debugDescendColor);
        auto debugBaselineColor = SDLColorToF4(context.debugBaselineColor);
        auto debugCaretColor = SDLColorToF4(context.debugCaretColor);

        ImGui::Checkbox("Baseline", &context.debugBaseline);
        ImGui::SameLine();
        ImGui::ColorEdit4(
            "Baseline", debugBaselineColor.data(),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Caret Positions", &context.debugCaret);
        ImGui::SameLine();
        ImGui::ColorEdit4(
            "Caret Positions", debugCaretColor.data(),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Glyph Bound", &context.debugGlyphBound);
        ImGui::SameLine();
        ImGui::ColorEdit4(
            "Glyph Bound", debugGlyphBoundColor.data(),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Ascend", &context.debugAscend);
        ImGui::SameLine();
        ImGui::ColorEdit4(
            "Ascend", debugAscendColor.data(),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Descend", &context.debugDescend);
        ImGui::SameLine();
        ImGui::ColorEdit4(
            "Descend", debugDescendColor.data(),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);
      }
    }
    ImGui::End();
  }

  if (showAdjustments) {
    ImGui::Begin("Adjustments", &showAdjustments);
    ImGui::BeginDisabled(isShape);

    ImGui::Button("Open");
    ImGui::SameLine();
    ImGui::Button("Save");
    ImGui::SameLine();
    ImGui::Button("Clear");

    ImGui::Text("* Adjustments only work when shapping is off.");

    if (ImGui::CollapsingHeader("Font")) {
      ImGui::InputFloat("Ascend", &font.GetFontAdjustments().ascend);
      ImGui::InputFloat("Descend", &font.GetFontAdjustments().descend);
    }

    ImGui::EndDisabled();
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
      if (!newFont.LoadFile(fontPaths[newSelected].string())) {
        ImGui::OpenPopup("InvalidFont");
      } else {
        font = newFont;
        selectedFontIndex = newSelected;
      }
    }
  }

  if (ImGui::BeginPopup("InvalidFont")) {
    ImGui::Text("Invalid Font File");
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void MainScene::OnDirectorySelected(Context &ctx,
                                    const std::filesystem::path &path) {
  std::filesystem::path newPath = path;

  if (!std::filesystem::exists(newPath)) {
    newPath = std::filesystem::absolute("fonts");
  }
  ctx.fontPath = path.string();
  fontPaths = ListFontFiles(ctx.fontPath);
}

std::vector<std::filesystem::path>
MainScene::ListFontFiles(const std::filesystem::path &path) {
  std::vector<std::filesystem::path> output;
  std::filesystem::directory_iterator iter(path);

  std::copy_if(
      std::filesystem::begin(iter), std::filesystem::end(iter),
      std::back_inserter(output),
      [](const std::filesystem::directory_entry &p) {
        if (!p.is_regular_file()) {
          return false;
        }

        auto &entryPath = p.path();
        auto extension = entryPath.extension().string();

        static auto compare = [](char c1, char c2) -> bool {
          return std::tolower(c1) == std::tolower(c2);
        };

        if (!std::equal(extension.begin(), extension.end(), ".otf", compare) &&
            !std::equal(extension.begin(), extension.end(), ".ttf", compare)) {
          return false;
        }

        return true;
      });

  return output;
}
